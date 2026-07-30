# mem.h 

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)

**mem.h** is a lightweight, modern C++20 header-only class designed to make interacting with external Windows processes safe, easy, and efficient. It uses RAII (Resource Acquisition Is Initialization) to guarantee that process handles are safely closed, preventing resource leaks.

## 📑 Table of Contents
- [Features](#-features)
- [Prerequisites](#-prerequisites)
- [Installation](#-installation)
- [Quick Start Guide](#-quick-start-guide)
- [API Reference](#-api-reference)
- [Best Practices](#-best-practices)
- [Disclaimer](#-disclaimer)

---

## ✨ Features

- 📦 **Header-Only:** No static libraries, no complex build configurations. Just `#include "Mem.h"`.
- 🛡️ **Safe & Leak-Free (RAII):** Windows `HANDLE` lifecycle is bound to the class. If your program crashes or exits, the handle closes automatically.
- ⚡ **Modern C++20:** Leverages `<string_view>` for zero-allocation string comparisons and `constexpr` for compile-time template evaluations.
- 🔍 **Dynamic Scanning:** Instantly attach to running processes and locate the base addresses of loaded modules (e.g., `.exe` or `.dll`).

---

## ⚙️ Prerequisites

To compile and use this class, ensure your development environment meets the following requirements:

- **Operating System:** Windows
- **Standard:** C++20 or newer
- **Project Settings:** Your Visual Studio project must be set to **Multi-Byte Character Set**.
  > *To change this in Visual Studio: Right-click your Project -> Properties -> Advanced (or General) -> Character Set -> **Use Multi-Byte Character Set**.*

---

## 🚀 Installation

Because this is a header-only library, installation is trivial:

1. Download or clone this repository.
2. Drag and drop `Mem.h` into your C++ project folder.
3. Include it in your source file: `#include "Mem.h"`

---

## 💻 Quick Start Guide

Here is a complete, working example of how to attach to a process, find a module, and manipulate its memory.

```cpp
#include <iostream>
#include "Mem.h"

int main() {
    // 1. Attach to the target process
    Mem mem("game.exe");

    // Verify attachment
    if (mem.getProcessID() == 0) {
        std::cerr << "[!] Could not find the process. Make sure it is running.\n";
        return 1;
    }
    std::cout << "[+] Attached to Process ID: " << mem.getProcessID() << "\n";

    // 2. Locate a specific module (e.g., the main game client)
    std::uintptr_t clientBase = mem.getModuleBase("client.dll");
    if (clientBase == 0) {
        std::cerr << "[!] Could not find client.dll!\n";
        return 1;
    }
    std::cout << "[+] client.dll Base Address: 0x" << std::hex << clientBase << std::dec << "\n";

    // 3. Read Memory (Assuming health is a 4-byte integer at base + 0x1A2B)
    std::uintptr_t healthAddress = clientBase + 0x1A2B;
    int currentHealth = mem.read<int>(healthAddress);
    std::cout << "[*] Current Health: " << currentHealth << "\n";

    // 4. Write Memory (Overwrite health to 999)
    mem.write<int>(healthAddress, 999);
    std::cout << "[+] Health successfully updated to 999!\n";

    return 0;
} 
// The 'mem' object goes out of scope here. 
// The destructor fires, and CloseHandle() is called automatically!

```



## 📖 API Reference

### `Mem(const std::string_view processName)`

**Constructor.** Takes the executable name (e.g., `"notepad.exe"`). Takes a snapshot of all system processes and securely opens a handle to the target using RAII.

### `~Mem()`

**Destructor.** Automatically cleans up and safely closes the `HANDLE` to the process when the object is destroyed.

### `std::uintptr_t getModuleBase(const std::string_view moduleName)`

Scans the attached process for a specific module (like `"client.dll"` or `"engine.dll"`) and returns its base memory address. Returns `0` if not found.

### `template<typename T> constexpr const T read(const std::uintptr_t& address)`

Reads a value of type `T` from the target process at the given memory address.

```cpp
float playerX = mem.read<float>(0xDEADBEEF);

```

### `template<typename T> constexpr void write(const std::uintptr_t& address, const T& value)`

Writes a value of type `T` to the target process at the given memory address.

```cpp
mem.write<int>(0xDEADBEEF, 1337);

```

### `std::uintptr_t getProcessID()`

Returns the Process ID (PID) of the attached process. Useful for verifying if the constructor successfully found the process (returns `0` on failure).

---

## 🛡️ Best Practices

Out of the box, this class requests `PROCESS_ALL_ACCESS`. While excellent for tutorials, debugging, and offline applications, it is considered highly aggressive.

If you are developing software intended to run alongside anti-cheat systems (like EAC, BattlEye, or Vanguard) or aggressive antivirus heuristics, `PROCESS_ALL_ACCESS` will likely trigger a flag. In production, change the `OpenProcess` flags inside `Mem.h` to request only what you need:

```cpp
// Change from this:
processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

// To this:
processHandle = ::OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processID);

```

---

## ⚠️ Disclaimer

This repository is strictly for **educational purposes only**. The code provided is meant to demonstrate Windows API interaction, RAII concepts in C++, and memory management. Do not use this code to violate the Terms of Service of any software.

