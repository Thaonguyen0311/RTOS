# FreeRTOS Mutex Challenge: Protect a Task Parameter

## 1. Modified Code

```cpp
/*
 * FreeRTOS Mutex Challenge
 * Protect a task parameter using a mutex
 */

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;

// Mutex shared between setup() and blinkLED()
SemaphoreHandle_t paramMutex;

// Blink LED using the delay passed as a parameter
void blinkLED(void *parameters) {

  // Wait until the mutex is available
  xSemaphoreTake(paramMutex, portMAX_DELAY);

  // Copy the parameter into a local variable
  int num = *(int *)parameters;

  // Release the mutex after copying
  xSemaphoreGive(paramMutex);

  // Print the copied parameter
  Serial.print("Received: ");
  Serial.println(num);

  pinMode(led_pin, OUTPUT);

  // Blink forever
  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(num));

    digitalWrite(led_pin, LOW);
    vTaskDelay(pdMS_TO_TICKS(num));
  }
}

void setup() {

  // Local variable passed to the task
  int delay_arg;

  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(1000));

  Serial.println();
  Serial.println("--- FreeRTOS Mutex Challenge ---");
  Serial.println("Enter a delay value in milliseconds:");

  // Wait for serial input
  while (Serial.available() <= 0) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  // Read and validate the input
  delay_arg = Serial.parseInt();

  if (delay_arg <= 0) {
    delay_arg = 500;
  }

  Serial.print("Sending: ");
  Serial.println(delay_arg);

  // Create the mutex
  paramMutex = xSemaphoreCreateMutex();

  if (paramMutex == NULL) {
    Serial.println("Failed to create mutex!");
    return;
  }

  // Lock the mutex before creating the task
  xSemaphoreTake(paramMutex, portMAX_DELAY);

  // Create the task with a higher priority
  BaseType_t result = xTaskCreatePinnedToCore(
    blinkLED,
    "Blink LED",
    2048,
    (void *)&delay_arg,
    2,
    NULL,
    app_cpu
  );

  if (result != pdPASS) {
    Serial.println("Failed to create task!");
    xSemaphoreGive(paramMutex);
    return;
  }

  // Allow blinkLED to proceed
  xSemaphoreGive(paramMutex);

  // Wait until blinkLED has copied the parameter
  xSemaphoreTake(paramMutex, portMAX_DELAY);

  // Release the mutex so setup() can finish
  xSemaphoreGive(paramMutex);

  Serial.println("Done! Parameter copied successfully.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
```

## 2. Changes Made to the Original Code

### Change 1: Added a mutex handle

```cpp
SemaphoreHandle_t paramMutex;
```

This handle is used by both `setup()` and `blinkLED()` to coordinate access to the task parameter.

### Change 2: Created the mutex

```cpp
paramMutex = xSemaphoreCreateMutex();
```

The mutex is created before the task starts. If creation fails, the program prints an error message.

### Change 3: Locked the mutex before creating the task

```cpp
xSemaphoreTake(paramMutex, portMAX_DELAY);
```

This prevents `blinkLED()` from reading the parameter before `setup()` releases the mutex.

### Change 4: Protected the parameter copy

```cpp
xSemaphoreTake(paramMutex, portMAX_DELAY);

int num = *(int *)parameters;

xSemaphoreGive(paramMutex);
```

The task acquires the mutex, copies the parameter into its local variable `num`, and releases the mutex immediately afterward.

### Change 5: Made `setup()` wait for the copy

```cpp
xSemaphoreGive(paramMutex);

xSemaphoreTake(paramMutex, portMAX_DELAY);

xSemaphoreGive(paramMutex);
```

After allowing the task to proceed, `setup()` attempts to acquire the mutex again. The higher-priority task should acquire it first, copy the parameter, and release it. This allows `setup()` to continue and finish.

### Change 6: Matched the parameter types

The original code used `long int delay_arg`, while the task read the value as an `int`.

The modified code uses:

```cpp
int delay_arg;
```

This matches the task's parameter type.

### Change 7: Increased the task stack size

The task stack size was increased from `1024` to `2048` to provide additional stack space for the task.

## 3. Testing

1. Upload the sketch to the ESP32-S3.
2. Open the Serial Monitor at **115200 baud**.
3. Enter `500` and press Enter.
4. Check the received parameter and observe the LED.
5. Reset the board and test another value, such as `1000`.

Expected output includes:

```text
--- FreeRTOS Mutex Challenge ---
Enter a delay value in milliseconds:
Sending: 500
Received: 500
Done! Parameter copied successfully.
```

The LED should turn on for approximately 500 ms and off for approximately 500 ms.

The relative order of the final two messages may vary depending on scheduling.

## 4. Questions and Answers

### Question 1: Why can passing the address of a local variable to a task be unsafe?

A local variable in `setup()` has a lifetime that ends when `setup()` returns. If the task accesses the pointer after that, it may read invalid or overwritten memory. Therefore, the task must copy the value before `setup()` finishes.

### Question 2: When does `blinkLED` copy the parameter?

`blinkLED()` copies the parameter after successfully taking the mutex. It stores the value in its local variable `num` and then releases the mutex. The task uses this local copy for the LED blinking delay.

### Question 3: How does the mutex make `setup()` wait?

`setup()` locks the mutex before creating the task. It then releases the mutex and attempts to acquire it again. The higher-priority `blinkLED()` task should acquire the mutex first, copy the parameter, and release the mutex. Afterward, `setup()` can acquire the mutex and finish.

### Question 4: Why is this described as a demonstration hack? What would be a better design in a real application?

Using a mutex to coordinate a parameter's lifetime is unusual. This example also depends on task priority and scheduling behavior, making the synchronization fragile.

A better design would use a parameter whose lifetime is guaranteed, such

<img width="650" height="200" alt="image" src="https://github.com/user-attachments/assets/113d3fee-f82e-42bc-89e5-d88e1469d7af" />

