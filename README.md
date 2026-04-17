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

All screenshots are available in the `OS-ss` folder in this repository.

---

## 4. Engineering Analysis

### Isolation Mechanisms

* Containers are created using Linux namespaces via `clone()` with `CLONE_NEWPID`, `CLONE_NEWUTS`, and `CLONE_NEWNS`.
* Each container has its own process tree, hostname, and filesystem view.
* `chroot()` is used to isolate the root filesystem.
* Containers share the host kernel, making them lightweight compared to VMs.

---

### Supervisor and Process Lifecycle

* The supervisor acts as a sub-reaper to clean up zombie processes.
* Uses `SIGCHLD` handler with `waitpid(-1, &status, WNOHANG)`.
* Maintains container metadata (ID, PID, state).

---

### IPC, Threads, and Synchronization

#### Logging Path

* Pipes capture container stdout/stderr.
* Producer threads push logs into a bounded buffer.

#### Control Path

* UNIX domain socket (`/tmp/mini_runtime.sock`) used for CLI communication.

#### Synchronization

* Mutex + condition variables ensure safe communication.

---

### Memory Management and Enforcement

#### Soft Limits

* Logs warnings when threshold is exceeded.

#### Hard Limits

* Container is terminated when memory exceeds limit.

---

## 5. Design Decisions and Tradeoffs

* Used namespaces instead of full virtualization for lightweight isolation.
* Single-threaded supervisor simplifies synchronization.
* Pipes + sockets provide efficient IPC.
* Kernel-space monitoring ensures strict memory enforcement.

---

## 6. Experiment Results

### Experiment 1: Basic Container Execution

| Container | Command Used         | Result  |
| --------- | -------------------- | ------- |
| alpha     | echo hello; sleep 30 | Success |
| beta      | echo hello; sleep 30 | Success |

* Containers successfully executed commands inside isolated rootfs.
* Output was captured and logged.
* `ps` correctly displayed container state.

---

### Experiment 2: Logging Verification

| Container | Command Used            | Result  |
| --------- | ----------------------- | ------- |
| alpha     | echo Logging is working | Success |

* Logs were successfully written to `logs/alpha.log`.
* Verified stdout capture.

---

### Experiment 3: Memory Monitoring (Normal Case)

| Container      | Command Used  | Result  |
| -------------- | ------------- | ------- |
| jackfruit_test | memory_hog 20 | Success |

* Container executed within limits.
* Successfully registered with kernel monitor.

---

### Experiment 4: Memory Limit Enforcement

| Container | Command Used   | Result          |
| --------- | -------------- | --------------- |
| killme    | memory_hog 512 | Failed / Killed |

* Excess memory triggered enforcement.
* Container was terminated.

---

### Experiment 5: Policy Testing

| Container   | Command Used  | Result  |
| ----------- | ------------- | ------- |
| policy_test | memory_hog 20 | Success |

* Container executed under monitoring policy.
* Successfully registered and tracked.

---

### Key Observations

* Kernel module correctly registers containers.
* Logging system works reliably.
* Memory limits are enforced effectively.
* Supervisor provides interactive control.


