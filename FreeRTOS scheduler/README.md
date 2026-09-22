# FreeRTOS Scheduler Demonstration

## 1. Assignment Purpose

This assignment investigates how the **FreeRTOS scheduler** selects tasks on an ESP32-S3.

Two FreeRTOS tasks are created and pinned to the **same CPU core**. The experiment demonstrates:

* Running, Ready, and Blocked task states
* Priority-based scheduling
* Preemption
* Equal-priority behaviour
* Task notifications

The main purpose is not only to make the program work, but to observe and explain how the scheduler behaves.

---

## 2. Hardware and Software

### Hardware

* ESP32-S3-DevKitC-1 v1.1
* USB data cable

### Software

* Arduino IDE 2.x
* ESP32 board package by Espressif Systems
* Serial Monitor
* Baud rate: **115200**

---

## 3. Task Structure

The program contains two FreeRTOS tasks.

### Task A

* Priority: **1**
* Pinned to CPU Core 0
* Prints a message character by character
* Notifies Task B in the middle of the message

```cpp
xTaskNotifyGive(taskBHandle);
```

### Task B

* Priority is changed for each experiment
* Pinned to CPU Core 0
* Initially waits for a notification
* Uses:

```cpp
ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
```

When Task A sends a notification, Task B becomes Ready and can be scheduled.

---

## 4. Important FreeRTOS Concepts

### Running

A task is **Running** when it currently has control of the CPU.

Because both tasks are running on the same CPU core, only one of them can be Running at a time.

### Ready

A task is **Ready** when it is able to run but is currently waiting for the scheduler to give it CPU time.

### Blocked

A task is **Blocked** when it cannot run because it is waiting for an event or time period.

Task B starts in the Blocked state because it is waiting here:

```cpp
ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
```

It remains Blocked until Task A sends a notification.

---

# 5. Scheduler Experiments

## Scenario A — Task B Priority = 2

| Task   | Priority |
| ------ | -------: |
| Task A |        1 |
| Task B |        2 |

### Prediction

Task B has a higher priority than Task A. When Task A calls `xTaskNotifyGive()`, Task B should become Ready.

Because Task B has the higher priority, it can preempt Task A and start running before Task A has finished printing its message.

### Actual Observation

Task B started running while Task A was still printing its message.

The output showed that Task A's message was interrupted and Task B printed its message before Task A continued.

### Result

The observation matched the prediction.

---

## Scenario B — Task B Priority = 1

| Task   | Priority |
| ------ | -------: |
| Task A |        1 |
| Task B |        1 |

### Prediction

Both tasks have the same priority.

When Task A notifies Task B, Task B becomes Ready, but it does not have a higher priority than Task A. Therefore, the behaviour is different from Scenario A.

The scheduler does not preempt Task A simply because Task B has become Ready.

### Actual Observation

With both tasks at the same priority, Task B did not immediately behave like the higher-priority task in Scenario A.

The exact output depends on timing and scheduler configuration, but Task A was able to continue executing when Task B did not have a higher priority.

### Result

The observation demonstrates that equal-priority tasks are handled differently from higher-priority tasks.

---

## Scenario C — Task B Priority = 3

| Task   | Priority |
| ------ | -------: |
| Task A |        1 |
| Task B |        3 |

### Prediction

Task B has a much higher priority than Task A.

When Task A calls `xTaskNotifyGive()`, Task B changes from Blocked to Ready.

Because Task B has the highest priority, the scheduler should select Task B immediately, causing Task B to preempt Task A.

### Actual Observation

Task B started running before Task A finished printing its message.

The interruption was clearly visible in the Serial Monitor output.

### Result

The observation matched the expected priority-based scheduling behaviour.

---

# 6. Prediction and Observation Summary

| Scenario | Task A | Task B | Prediction                      | Actual Observation                            | Match? |
| -------- | -----: | -----: | ------------------------------- | --------------------------------------------- | ------ |
| A        |      1 |      2 | B preempts A after notification | B interrupted A before A finished             | Yes    |
| B        |      1 |      1 | No priority advantage for B     | Behaviour differed from higher-priority cases | Yes    |
| C        |      1 |      3 | B preempts A after notification | B interrupted A before A finished             | Yes    |

> The exact position of Task B's message can vary because task scheduling depends on timing and system configuration.

---

# 7. Scheduler Behaviour Explanation

Task A and Task B are pinned to the same CPU core, so only one task can be in the Running state at a time. A task in the Ready state is able to run but is waiting for the scheduler to select it. A task in the Blocked state cannot run because it is waiting for an event. Task B is initially Blocked because `ulTaskNotifyTake()` makes it wait for a notification from Task A.

When Task A calls `xTaskNotifyGive()`, Task B receives the notification and becomes Ready. In Scenario A, Task B has priority 2 while Task A has priority 1. Since Task B has the higher priority, the scheduler can immediately select Task B, causing it to preempt Task A. Task A therefore stops in the middle of its message and continues later.

In Scenario C, the same principle applies even more clearly because Task B has priority 3. In Scenario B, both tasks have priority 1. Task B no longer has a priority advantage, so becoming Ready does not give it the same immediate preemption behaviour as in the higher-priority scenarios.

When Task B finishes running, Task A can continue from where it stopped. The task's execution state is maintained by FreeRTOS during the context switch. Therefore, preemption does not restart Task A from the beginning; it resumes from its previous execution point.

---

# 8. Serial Monitor Screenshot

The Serial Monitor was configured to use **115200 baud**.

The screenshot below shows the scheduler demonstration and the interaction between Task A and Task B.

<img width="440" height="346" alt="Screenshot 2026-09-22 112817" src="https://github.com/user-attachments/assets/691f7cd8-68bf-4e4d-a033-89bb9b1f022c" />

---

# 9. Files

```text
.
├── README.md
├── SchedulerDemo.ino
└── images/
    └── serial-monitor.png
```

---

# 10. Conclusion

This experiment demonstrated that the FreeRTOS scheduler selects tasks based on their state and priority.

The experiment showed that:

* Only one task can run on the same CPU core at a time.
* A task can be Ready without currently running.
* A task can be Blocked while waiting for an event.
* A notification can move Task B from Blocked to Ready.
* A higher-priority Ready task can preempt a lower-priority Running task.
* Equal-priority tasks do not have the same priority-based preemption advantage.
* After preemption, the interrupted task can continue from where it stopped.

The Serial Monitor output was used to observe and verify these scheduler behaviours.


