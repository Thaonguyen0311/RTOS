// SchedulerDemo.ino
//
// Task A: Priority 1
// Task B: Priority 2
//
// Both tasks are pinned to the same CPU core.
// Task A prints a message character by character.
// At a specific character, Task A notifies Task B.
// Task B wakes up and may preempt Task A.

TaskHandle_t taskAHandle = NULL;
TaskHandle_t taskBHandle = NULL;

// --------------------------------------------------
// Task A
// --------------------------------------------------
void taskA(void *parameter)
{
  const char *msg = "Task A is printing slowly...\n";

  while (true)
  {
    for (size_t i = 0; msg[i] != '\0'; i++)
    {
      // Notify Task B in the middle of the message
      if (i == 12)
      {
        xTaskNotifyGive(taskBHandle);
      }

      Serial.print(msg[i]);

      // Small busy loop to make the character output
      // visible and give us a chance to observe scheduling.
      for (volatile long j = 0; j < 500000; j++)
      {
      }
    }
  }
}

// --------------------------------------------------
// Task B
// --------------------------------------------------
void taskB(void *parameter)
{
  while (true)
  {
    // Task B waits here until Task A notifies it.
    ulTaskNotifyTake(
      pdTRUE,
      portMAX_DELAY
    );

    Serial.println("\nTask B is running!");
  }
}

// --------------------------------------------------
// Setup
// --------------------------------------------------
void setup()
{
  Serial.begin(115200);

  // Give Serial time to initialize
  delay(500);

  Serial.println();
  Serial.println("FreeRTOS Scheduler Demonstration");
  Serial.println("--------------------------------");

  // Both tasks run on the SAME core.
  //
  // ESP32-S3 Arduino:
  // Core 0 is used here.
  //
  // Task A = Priority 1
  // Task B = Priority 2

  xTaskCreatePinnedToCore(
    taskA,          // Task function
    "Task A",       // Task name
    4096,           // Stack size
    NULL,           // Parameter
    1,              // Priority
    &taskAHandle,   // Task handle
    0               // Core 0
  );

  xTaskCreatePinnedToCore(
    taskB,          // Task function
    "Task B",       // Task name
    4096,           // Stack size
    NULL,           // Parameter
    2,              // Priority
    &taskBHandle,   // Task handle
    0               // Core 0
  );
}

void loop()
{
  // Nothing here.
  // The experiment is performed entirely by the two FreeRTOS tasks.
}