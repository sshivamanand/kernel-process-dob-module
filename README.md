# Kernel Process & DOB Manager – Jackfruit Module

A Linux kernel module written in C that demonstrates multi-level process creation and dynamic memory management via linked lists. This project simulates process hierarchy using kernel threads and maintains a Date of Birth (DOB) list using the Linux kernel’s `list_head` API.

---

## 📌 Features

- 🔁 **Recursive Process Tree Creation**  
  Spawns a **4-level deep child process tree**, with each level running as a kernel thread. Each node logs its **PID**, **parent PID**, and **depth**.

- 📅 **DOB Management**  
  Dynamically generates and stores **5 random DOB entries** (day/month/year) using a custom linked list and `kmalloc`.

- 🔐 **Safe Resource Handling**  
  Ensures proper cleanup of all kernel threads and allocated memory upon module removal.

---

## ⚙️ Build & Installation

### 📦 Requirements
- Linux environment (preferably a VM or development system)
- Kernel headers installed (`linux-headers-$(uname -r)`)
- Root privileges

### 🔧 Build the Module

```bash
make
```

### 📥 Insert the Module

```bash
sudo insmod jackfruit.ko
```

### 📤 Remove the Module

```bash
sudo rmmod jackfruit
```

### 🧹 Clean Build Files

```bash
make clean
```

---
