# RTOS


<img width="1536" height="2048" alt="image" src="https://github.com/user-attachments/assets/18d1e213-c3e6-4037-8551-46d26a0114f5" />

<img width="606" height="350" alt="image" src="https://github.com/user-attachments/assets/3fd7b6cb-b8c4-4286-97ff-07a4cdfb0686" />
# FreeRTOS Task Control and Independent Activities

## 1. Hardware and Software

### Hardware

* **Board:** ESP32-S3-DevKitC-1 v1.1
* **RGB LED:** GPIO 38

### Software

* Arduino IDE 2.x
* FreeRTOS
* Serial Monitor: 115200 baud

---

## 2. Task Design

This application contains two independent FreeRTOS tasks running on the same ESP32 core.

### Serial Task

The Serial Task:

* Reads commands from the Serial Monitor.
* Checks the user input.
* Updates the LED blink interval.
* Suspends or resumes the RGB LED Task.

Accepted blink intervals are:

```text
250
500
1000
```

The following commands are also supported:

```text
suspend
resume
```

Invalid input is rejected.

### RGB LED Task

The RGB LED Task:

* Controls the onboard RGB LED.
* Uses the latest accepted blink interval.
* Blinks the LED independently from the Serial Task.

For example, with an interval of 500 ms:

```text
LED ON
wait 500 ms
LED OFF
wait 500 ms
```

### Priorities and Core Assignment

Both tasks are created as FreeRTOS tasks and run on the same ESP32 core.

* Serial Task: [YOUR PRIORITY]
* RGB LED Task: [YOUR PRIORITY]
* Core: [YOUR CORE NUMBER]

---

## 3. Task Handles

Task handles are used to keep a reference to the created task instances.

```cpp
TaskHandle_t serialTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;
```

The LED task handle is used to suspend and resume the LED task:

```cpp
vTaskSuspend(ledTaskHandle);
```

and:

```cpp
vTaskResume(ledTaskHandle);
```

The task handle allows the program to control a specific task after the task has been created.

---

## 4. Shared Variable

The blink interval is shared between the Serial Task and the RGB LED Task.

```cpp
volatile uint32_t blinkInterval = 500;
```

The Serial Task updates this variable when the user enters a valid interval.

The RGB LED Task reads the latest value and uses it for the LED delay.

---

## 5. Suspend / Resume Test

### Suspend

When the following command is entered:

```text
suspend
```

the Serial Task executes:

```cpp
vTaskSuspend(ledTaskHandle);
```

The result is:

* The RGB LED stops blinking.
* The Serial Task continues running.
* Serial commands can still be entered.

### Resume

When the following command is entered:

```text
resume
```

the Serial Task executes:

```cpp
vTaskResume(ledTaskHandle);
```

The RGB LED Task becomes Ready again and can continue its execution.

The latest blink interval is used after the task resumes.

---

## 6. Prediction

Before testing, my predictions were:

| Situation                 | Prediction                                                           |
| ------------------------- | -------------------------------------------------------------------- |
| LED task running normally | Serial Task and LED Task can operate independently.                  |
| LED task suspended        | LED Task is Suspended, while Serial Task continues working.          |
| LED task resumed          | LED Task becomes Ready and can run again according to the scheduler. |

### Actual Observation

| Situation                 | Actual Observation       | Match?   |
| ------------------------- | ------------------------ | -------- |
| LED task running normally | [WRITE YOUR OBSERVATION] | [YES/NO] |
| LED task suspended        | [WRITE YOUR OBSERVATION] | [YES/NO] |
| LED task resumed          | [WRITE YOUR OBSERVATION] | [YES/NO] |

---

## 7. Task State Analysis

### Situation 1 – Normal Operation

**Serial Task:** [Ready / Running / Blocked]

**RGB LED Task:** [Ready / Running / Blocked]

The tasks can change between states while the application is running. For example, a task may become Blocked when it calls a delay function and become Ready again when the delay expires.

### Situation 2 – LED Task Suspended

**Serial Task:** [Ready / Running / Blocked]

**RGB LED Task:** Suspended

When `vTaskSuspend(ledTaskHandle)` is called, the LED Task is placed into the Suspended state. It is therefore not eligible for scheduling.

The Serial Task continues to operate because it is a separate FreeRTOS task.

### Situation 3 – LED Task Resumed

**Serial Task:** [Ready / Running / Blocked]

**RGB LED Task:** Ready / Running

When `vTaskResume(ledTaskHandle)` is called, the LED Task becomes Ready. It does not necessarily become Running immediately because the scheduler still decides which Ready task should execute.

---

## 8. Screenshots

### Running

The application is running normally and the RGB LED is blinking.

![Running](images/running.png)

### Suspended

The LED Task has been suspended while the Serial Task continues to accept commands.

![Suspended](images/suspended.png)

### Resumed

The LED Task has been resumed and the RGB LED starts operating again.

![Resumed](images/resumed.png)

---

## 9. Reflection

This experiment helped me understand how FreeRTOS allows different activities to run as independent tasks. I used two tasks: a Serial Task and an RGB LED Task. The Serial Task receives commands and changes the LED blink interval, while the RGB LED Task controls the onboard LED.

A task handle is important because it provides a reference to a specific task instance. With the LED task handle, the Serial Task can suspend and resume the LED Task using `vTaskSuspend()` and `vTaskResume()`.

When the LED Task is suspended, it enters the Suspended state and is no longer eligible for scheduling. However, the Serial Task can continue working because it is a separate task. When `vTaskResume()` is called, the LED Task becomes Ready again. It does not necessarily start running immediately because the FreeRTOS scheduler decides which Ready task should run.

The experiment also showed the difference between Suspended, Ready, Running, and Blocked states. This demonstrates how multiple independent activities can be managed by one FreeRTOS scheduler.

---

## 11. Conclusion

The application demonstrates:

* Two FreeRTOS tasks
* Shared data between tasks
* Task handles
* Task suspension
* Task resumption
* Independent Serial and LED activities
* FreeRTOS task states
* Scheduler-based task execution
