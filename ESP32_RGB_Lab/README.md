# ESP32-S3 RGB LED and FreeRTOS Lab

## Purpose

This lab uses the onboard addressable RGB LED on GPIO 38 of an ESP32-S3-DevKitC-1. It demonstrates periodic FreeRTOS task timing, task priorities, queue-based communication, and `neopixelWrite()`.

## Hardware and setup

- Board: ESP32-S3-DevKitC-1 v1.1 (or Arduino IDE's ESP32S3 Dev Module)
- Install **esp32 by Espressif Systems** in Boards Manager.
- Select the board and its USB serial port. For native USB CDC, enable **USB CDC On Boot**.
- Upload `ESP32_RGB_Lab.ino` and open Serial Monitor at **115200 baud**.

The sketch uses the ESP32 Arduino core's `neopixelWrite()` function and does not need the Adafruit NeoPixel library.

## How the program works

`LedTask` has priority 2 and updates the LED every 500 ms using `vTaskDelayUntil()`. It cycles through red, green, blue, and off, then adds a custom amber, cyan, magenta, white, and off pattern. Each state index is sent to an eight-item FreeRTOS queue. `MonitorTask` has priority 1, receives those indices, and prints the color and RGB channel values to Serial Monitor. The higher-priority task can preempt the monitor task; the queue passes each LED state safely between them.

## Completed tasks and observations

- **Task A — Change the colour:** The pattern includes red, green, blue, and white. White uses equal nonzero red, green, and blue channel values (`50, 50, 50`). Each color should appear for one half-second step.
- **Task B — Change the blink rate:** `STATE_PERIOD` is 500 ms for this final sketch. To try 250 ms, change it to `pdMS_TO_TICKS(250)`. The transitions should happen twice as often as at 500 ms; compared with the original 1000 ms example, four times as often.
- **Task C — Create an RGB cycle:** Red → Green → Blue → Off, with 500 ms per state, appears at the beginning of every cycle.
- **Task D — Create your own pattern:** Amber → Cyan → Magenta → White → Off follows the RGB cycle. Brightness is limited to 50 per channel for indoor visibility.
- **Timing and scheduling:** `vTaskDelayUntil()` targets a fixed 500 ms interval. The LED task has priority 2 and the serial monitor task priority 1. Serial messages show the state sequence and channel values; verify the timing and visible LED behavior on the physical board.

## Serial Monitor screenshot

After uploading and observing the board, capture the Serial Monitor at 115200 baud and save it as `images/serial-monitor.png`. Then replace this note with:

<!-- Replace this note with: ![ESP32-S3 Serial Monitor](images/serial-monitor.png) -->

Example serial lines:

```text
ESP32-S3 FreeRTOS RGB pattern starting
LED: Red (R=50, G=0, B=0)
LED: Green (R=0, G=50, B=0)
LED: Blue (R=0, G=0, B=50)
LED: Off (R=0, G=0, B=0)
```

## Repository contents

```text
README.md
ESP32_RGB_Lab.ino
images/serial-monitor.png   # Add after capturing a real board run
```

