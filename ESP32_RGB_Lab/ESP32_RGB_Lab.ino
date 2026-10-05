#include <Arduino.h>

#define RGB_BUILTIN 38
#define RGB_BRIGHTNESS 50

struct LedState
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  const char *name;
};

// Task D adds an amber, cyan, and magenta pattern after the RGB cycle.
const LedState pattern[] = {
    {RGB_BRIGHTNESS, 0, 0, "Red"},
    {0, RGB_BRIGHTNESS, 0, "Green"},
    {0, 0, RGB_BRIGHTNESS, "Blue"},
    {0, 0, 0, "Off"},
    {RGB_BRIGHTNESS, RGB_BRIGHTNESS / 2, 0, "Amber"},
    {0, RGB_BRIGHTNESS, RGB_BRIGHTNESS, "Cyan"},
    {RGB_BRIGHTNESS, 0, RGB_BRIGHTNESS, "Magenta"},
    {RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS, "White"},
    {0, 0, 0, "Off"},
};

constexpr size_t PATTERN_LENGTH = sizeof(pattern) / sizeof(pattern[0]);
constexpr TickType_t STATE_PERIOD = pdMS_TO_TICKS(500);

QueueHandle_t ledStateQueue = nullptr;

// Higher-priority periodic producer: update the LED every 500 ms.
void ledTask(void *parameter)
{
  size_t stateIndex = 0;
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true)
  {
    const LedState &state = pattern[stateIndex];
    neopixelWrite(RGB_BUILTIN, state.red, state.green, state.blue);

    // Queue a copy of the state so the monitor task can report it.
    xQueueSend(ledStateQueue, &stateIndex, 0);
    stateIndex = (stateIndex + 1) % PATTERN_LENGTH;

    // Keep the pattern on a fixed schedule instead of adding work time
    // to every interval, as a relative vTaskDelay() would.
    vTaskDelayUntil(&lastWakeTime, STATE_PERIOD);
  }
}

// Lower-priority consumer: print each LED state as it is reported.
void monitorTask(void *parameter)
{
  size_t receivedIndex;

  while (true)
  {
    if (xQueueReceive(ledStateQueue, &receivedIndex, portMAX_DELAY) == pdTRUE)
    {
      const LedState &state = pattern[receivedIndex];
      Serial.printf("LED: %s (R=%u, G=%u, B=%u)\n",
                    state.name, state.red, state.green, state.blue);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println("ESP32-S3 FreeRTOS RGB pattern starting");

  ledStateQueue = xQueueCreate(8, sizeof(size_t));
  if (ledStateQueue == nullptr)
  {
    Serial.println("Failed to create LED state queue");
    return;
  }

  // On ESP32-S3 Arduino, xTaskCreate stack sizes are specified in bytes.
  if (xTaskCreate(ledTask, "LedTask", 2048, nullptr, 2, nullptr) != pdPASS)
  {
    Serial.println("Failed to create LED task");
  }

  if (xTaskCreate(monitorTask, "MonitorTask", 3072, nullptr, 1, nullptr) != pdPASS)
  {
    Serial.println("Failed to create monitor task");
  }
}

void loop()
{
  // Work is handled by the FreeRTOS tasks.
  vTaskDelay(portMAX_DELAY);
}

