
/*
 * FreeRTOS Mutex Challenge
 * Protect a task parameter
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

  // 1. Wait until setup() releases the mutex
  xSemaphoreTake(paramMutex, portMAX_DELAY);

  // 2. Copy the parameter while holding the mutex
  int num = *(int *)parameters;

  // 3. Release the mutex after copying
  xSemaphoreGive(paramMutex);

  // The task no longer needs the original pointer
  Serial.print("Received: ");
  Serial.println(num);

  pinMode(led_pin, OUTPUT);

  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(num));

    digitalWrite(led_pin, LOW);
    vTaskDelay(pdMS_TO_TICKS(num));
  }
}

void setup() {

  // The parameter remains a local variable
  int delay_arg;

  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(1000));

  Serial.println();
  Serial.println("--- FreeRTOS Mutex Challenge ---");
  Serial.println("Enter a delay value in milliseconds:");

  while (Serial.available() <= 0) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  delay_arg = Serial.parseInt();

  // Validate the input
  if (delay_arg <= 0) {
    delay_arg = 500;
  }

  Serial.print("Sending: ");
  Serial.println(delay_arg);

  // 4. Create the mutex
  paramMutex = xSemaphoreCreateMutex();

  if (paramMutex == NULL) {
    Serial.println("Failed to create mutex!");
    return;
  }

  // 5. Lock the mutex before creating the task
  xSemaphoreTake(paramMutex, portMAX_DELAY);

  // 6. Create blinkLED with higher priority than setup()
  BaseType_t result = xTaskCreatePinnedToCore(
    blinkLED,
    "Blink LED",
    2048,
    (void *)&delay_arg,
    2,              // Higher priority helps ensure the task
                    // copies the parameter before setup resumes
    NULL,
    app_cpu
  );

  if (result != pdPASS) {
    Serial.println("Failed to create task!");
    xSemaphoreGive(paramMutex);
    return;
  }

  // 7. Allow blinkLED to take the mutex
  xSemaphoreGive(paramMutex);

  // 8. Wait until blinkLED has copied the parameter.
  // The higher-priority task gets the mutex first,
  // copies the value, and releases it.
  xSemaphoreTake(paramMutex, portMAX_DELAY);

  // The parameter has now been copied into blinkLED's
  // local variable. setup() can safely finish.
  xSemaphoreGive(paramMutex);

  Serial.println("Done! Parameter copied successfully.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
