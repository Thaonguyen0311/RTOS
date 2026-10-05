# FreeRTOS Queue Temperature Assignment

## Purpose

This ESP32 program demonstrates how a FreeRTOS queue passes integer data from a producer task to a consumer task. The Sensor Task simulates a temperature sensor, and the Display Task prints each reading to the Serial Monitor.

## How It Works

`setup()` creates a queue with five slots, each sized to hold one `int`, and then starts the Sensor Task and Display Task. The Sensor Task sends its current temperature, starting at 20, and increases it by one every second. The Display Task waits for a queued value and prints it at 115200 baud. If the queue is full, the sender waits for space; if it is empty, the receiver waits for a value.

## Serial Monitor

Open the Serial Monitor at **115200 baud** after uploading `rtos_queue_assignment.ino` to an ESP32. Capture the running output and save the screenshot as `serial-monitor.png` in this folder, then insert it here:

<!-- Replace this note with: ![ESP32 Serial Monitor output](serial-monitor.png) -->

Expected output:

```text
Temperature: 20 C
Temperature: 21 C
Temperature: 22 C
Temperature: 23 C
Temperature: 24 C
```

## Reflection

The queue lets the Sensor Task and Display Task share temperature readings without needing to run at the same time. It safely holds values until the Display Task is ready to receive them, so the tasks stay independent.
