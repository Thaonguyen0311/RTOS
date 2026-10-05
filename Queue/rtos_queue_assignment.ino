#include <Arduino.h>

QueueHandle_t temperatureQueue;

// --------------------------------------------------
// Sensor Task (producer)
// --------------------------------------------------

void sensorTask(void *parameter)
{
  int temperature = 20;

  while (true)
  {
    // Wait for space if the queue is full, then send this reading.
    xQueueSend(temperatureQueue, &temperature, portMAX_DELAY);

    temperature++;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// --------------------------------------------------
// Display Task (consumer)
// --------------------------------------------------

void displayTask(void *parameter)
{
  int receivedTemperature;

  while (true)
  {
    // Wait until the Sensor Task places a value in the queue.
    if (xQueueReceive(temperatureQueue, &receivedTemperature, portMAX_DELAY) == pdTRUE)
    {
      Serial.printf("Temperature: %d C\n", receivedTemperature);
    }
  }
}

// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(115200);

  // Five queue slots, each large enough for one integer.
  temperatureQueue = xQueueCreate(5, sizeof(int));
  if (temperatureQueue == nullptr)
  {
    Serial.println("Failed to create temperature queue.");
    return;
  }

  if (xTaskCreate(sensorTask, "SensorTask", 2048, nullptr, 1, nullptr) != pdPASS)
  {
    Serial.println("Failed to create Sensor Task.");
  }

  if (xTaskCreate(displayTask, "DisplayTask", 2048, nullptr, 1, nullptr) != pdPASS)
  {
    Serial.println("Failed to create Display Task.");
  }
}

void loop()
{
  // The FreeRTOS tasks perform the work.
  vTaskDelay(portMAX_DELAY);
}
