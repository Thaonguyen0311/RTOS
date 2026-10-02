# ESP32 FreeRTOS Memory Allocation

## 1. Measurements

### Task stack size: 4096 bytes

| Measurement   |               Free Heap |
| ------------- | ----------------------: |
| Before Task A | **[enter value] bytes** |
| After Task A  | **[enter value] bytes** |
| After Task B  | **[enter value] bytes** |

### Task stack size: 8192 bytes

| Measurement   |               Free Heap |
| ------------- | ----------------------: |
| Before Task A | **[enter value] bytes** |
| After Task A  | **[enter value] bytes** |
| After Task B  | **[enter value] bytes** |

## 2. Questions

### a. Did the free heap change when you increased the task stack size?

Yes. The free heap decreased when the task stack size was increased from 4096 bytes to 8192 bytes.

### b. What happened to the free heap?

The available free heap became smaller. Each task was given a larger stack, so more RAM was used when the tasks were created.

### c. Why does a task need stack memory?

A task needs stack memory to store local variables, function call information, temporary data, and other data needed while the task is running.

### d. Why does creating a FreeRTOS task use RAM?

Creating a FreeRTOS task uses RAM because every task needs a stack and a Task Control Block (TCB). When using dynamic allocation, FreeRTOS allocates this memory from the available heap. Therefore, creating a task reduces the amount of free heap memory.

## 3. Conclusion

The experiment shows that increasing the task stack size from 4096 to 8192 bytes increases the amount of RAM required by each task. Therefore, the available free heap decreases.

In simple terms:

**Larger task stack → more RAM used → less free heap.**

