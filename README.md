# Multi Container Runtime

## 1. Team Details

* Shruti Sridhar - PES2UG24CS498
* Tanisha Dalmia - PES2UG24CS550

---

## 2. Step by Step Commands and Instructions

###  Build

```bash
cd boilerplate
gcc engine.c -o engine -lpthread
cd ..
```

---

###  Load Kernel Module

```bash
cd boilerplate
sudo make
sudo insmod monitor.ko
ls -l /dev/container_monitor
sudo chmod 666 /dev/container_monitor
```

---

###  Start Supervisor (Terminal 1)

```bash
sudo ./boilerplate/engine supervisor ./rootfs-base
```

You will enter interactive mode:

```
supervisor> start <id> <rootfs> <cmd>
supervisor> ps
supervisor> stop <id>
```

---

###  Launch Containers (inside supervisor)

```
start alpha ./rootfs-alpha /bin/sh -c "echo hello; sleep 30"
start beta ./rootfs-beta /bin/sh -c "echo hello; sleep 30"
```

---

###  List Containers

```
ps
```

---

###  Logging Example

```bash
cat logs/alpha.log
```

---

###  Stop Containers

```
stop alpha
stop beta
```

---

###  Kernel Logs

```bash
sudo dmesg | tail
```

---

###  Unload Module

```bash
sudo rmmod monitor
```

---

## 3. Screenshots

All screenshots are available in the `OS-ss` folder.

* Container creation and supervisor interaction
* Logging output
* Memory monitoring and enforcement
* Scheduler experiment (CPU scheduling behavior)

---

## 4. Engineering Analysis

### Isolation Mechanisms

* Containers use Linux namespaces (`CLONE_NEWPID`, `CLONE_NEWUTS`, `CLONE_NEWNS`)
* `chroot()` ensures filesystem isolation
* Each container has its own rootfs copy

---

### Supervisor and Process Lifecycle

* Supervisor acts as parent and sub-reaper
* Handles `SIGCHLD` to clean up child processes
* Maintains metadata for each container

---

### IPC and Logging

* Pipes capture stdout/stderr from containers
* Logs stored in `logs/<container>.log`
* Producer-consumer model used for logging

---

### Memory Management

* Kernel module tracks container memory usage
* Soft limit → warning
* Hard limit → container killed

---

## 5. Design Decisions and Tradeoffs

* Namespaces used instead of VMs for lightweight isolation
* Single supervisor simplifies control logic
* Kernel-space enforcement ensures reliability
* Logging pipeline prevents blocking and data loss

---

## 6. Experiment Results

### Experiment 1: Basic Container Execution

| Container | Command              | Result  |
| --------- | -------------------- | ------- |
| alpha     | echo hello; sleep 30 | Success |
| beta      | echo hello; sleep 30 | Success |

* Containers executed commands successfully
* Output captured and displayed
* Verified using `ps`

---

### Experiment 2: Logging Verification

| Container | Command                 | Result  |
| --------- | ----------------------- | ------- |
| alpha     | echo Logging is working | Success |

* Logs stored in:

  ```
  logs/alpha.log
  ```
* Verified logging pipeline works correctly

---

### Experiment 3: Memory Monitoring (Normal Case)

| Container      | Command       | Result  |
| -------------- | ------------- | ------- |
| jackfruit_test | memory_hog 20 | Success |

* Container ran within limits
* Successfully registered with kernel monitor

---

### Experiment 4: Memory Limit Enforcement

| Container | Command        | Result |
| --------- | -------------- | ------ |
| killme    | memory_hog 512 | Killed |

* Exceeded memory limit
* Kernel terminated container
* Verified via logs and behavior

---

### Experiment 5: Policy Testing

| Container   | Command       | Result  |
| ----------- | ------------- | ------- |
| policy_test | memory_hog 20 | Success |

* Container executed under monitoring policy
* Successfully tracked

---

### Experiment 6: CPU Scheduling with Different Priorities

| Process                 | nice value | Workload  | Observation                        |
| ----------------------- | ---------- | --------- | ---------------------------------- |
| cpu_hog (high priority) | 0          | CPU-bound | High CPU usage, faster progress    |
| cpu_hog (low priority)  | 19         | CPU-bound | High CPU usage but slower progress |

* Two CPU-bound workloads were executed simultaneously
* Different `nice` values were used to change priority
* The Linux scheduler distributed CPU time based on priority
* The lower-priority process progressed more slowly
* Demonstrates behavior of the Completely Fair Scheduler (CFS)

---

### Key Observations

* Multiple containers can run concurrently with isolation
* Logging system reliably captures output
* Kernel module enforces memory limits effectively
* Scheduler distributes CPU based on priority and fairness


