# ESP32 FreeRTOS Memory Allocation

## 1. Measurements

### Task stack size: 4096 bytes

| Measurement   |               Free Heap |
| ------------- | ----------------------: |
| Before Task A | **362,736 bytes** |
| After Task A  | **358,136 bytes** |
| After Task B  | **353,536 bytes** |
<img width="816" height="332" alt="image" src="https://github.com/user-attachments/assets/27d7cd8e-f9aa-4ecb-a1b8-e73def293832" />

### Task stack size: 8192 bytes

| Measurement   |               Free Heap |
| ------------- | ----------------------: |
| Before Task A | **362,736 bytes** |
| After Task A  | **353,912 bytes** |
| After Task B  | **345,088 bytes** |
<img width="836" height="206" alt="image" src="https://github.com/user-attachments/assets/f0d3c60e-1120-4dd7-884c-d3cd598a66ca" />
<img width="840" height="340" alt="image" src="https://github.com/user-attachments/assets/c29295dc-3ee8-4a74-b92c-34a2078de3ef" />

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

