# RISC-V 32I Processor Simulator

![C++](https://img.shields.io/badge/Language-C++17-blue)
![Qt](https://img.shields.io/badge/Framework-Qt_6-green)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey)

## 📖 Overview

**RISC-V Simulator** is a graphical desktop application developed to simulate the **RV32I (RISC-V 32-bit Integer)** Instruction Set Architecture. 

Developed as part of my Bachelor's Thesis at the **West University of Timișoara**, this tool allows users to write Assembly code, compile it, and execute it step-by-step while visualizing the internal state of the processor (Registers, Program Counter, and Memory) in real-time.

It serves as an educational tool for understanding computer architecture, the Fetch-Decode-Execute cycle, and low-level memory management.

---

## 📸 Screenshots

### Main Interface & Code Editor
<img width="1917" height="1029" alt="image" src="https://github.com/user-attachments/assets/35415f29-fb39-4424-8d84-256080dd4649" />

*The main window featuring the Assembly editor, register bank view, and memory map.*

### Execution Visualization

https://github.com/user-attachments/assets/61972d1a-19b7-4c1b-8666-0adb38b50659

*Real-time updates of the Program Counter, Registers and Memory during instruction execution.*

## Instructions Page

<img width="1920" height="1032" alt="Screenshot 2025-12-09 215033" src="https://github.com/user-attachments/assets/71107a8d-6fd7-4d9e-baca-cb08c1dee489" />

*Users can verify the correct syntax and behavior of any RV32I instruction without leaving the application.*

---

## ✨ Key Features

* **Full RV32I Implementation:** Supports the standard integer instruction set of the RISC-V architecture.
* **Integrated Code Editor:**
    * Executed line highlighting
    * **Dedicated Instructions Reference:** A built-in guide explaining every instruction's syntax and behavior.
* **Advanced Execution Control:**
    * **Run / Step:** Execute every instruction with a 2 second delay or instruction-by-instruction.
    * **Smart Stop Button:** Single-click to **Pause** execution, Double-click to **Stop** (resets PC and clears instruction highlight).
    * **System Reset:** Instantly clears Registers, Data Memory (RAM), Console output, and halts any active execution.
* **Real-Time Visualization:**
    * **I/O Console:** Displays output from the simulated program.
    * **Register Bank:** View 32 GPRs in Decimal.
    * **Memory Inspector:** Monitor changes dynamically (view Address, Decimal, Hexadecimal, and ASCII values).

 ## 🎮 Controls & UI Logic

| Control | Action | Description |
| :--- | :--- | :--- |
| ▶️ **Run** | Execute Program | Runs the assembly code continuously until completion. |
| ⏭️ **Step** | Next Instruction | Executes a single instruction and advances the Program Counter. |
| ⏸️ **Stop (1 Click)** | Pause | Pauses the running execution, keeping the current state active. |
| ⏹️ **Stop (2 Clicks)**| Terminate | Stops execution, resets PC to 0, and clears the current line highlight. |
| 🔄 **Reset** | Hard Reset | Wipes all Registers, RAM, and Console logs; stops execution immediately. |

---

## 🛠️ Tech Stack

* **Core Logic:** C++ (RV32I Implementation, Memory Management, Simulation Engine)
* **User Interface:** Qt Quick (QML) - for a modern, fluid, and responsive GUI.
* **Interoperability:** C++ models integrated with QML views (signal/slot mechanism).
* **Build System:** CMake

---

## 🚀 Getting Started

### Prerequisites
* C++ Compiler (GCC, Clang, or MSVC)
* Qt 6.5 or newer installed
* CMake 3.16+

### Installation & Build

1.  **Clone the repository**
    ```bash
    git clone https://github.com/atamas19/Riscv-Sym.git
    cd Riscv-Sym
    ```

2.  **Build using Qt Creator (Recommended for Windows)**
    * Open the **root** `CMakeLists.txt` in Qt Creator.
    * Configure the project. It is highly recommended to select the **Release** configuration to ensure the simulator runs at maximum speed.
    * Press **Build & Run** (Ctrl+R).

3.  **Build using Command Line (CMake)**
    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build . -j
    ./riscv-viz
    ```

---

## 💡 Usage Example

1.  Launch the application.
2.  In the editor panel, write a simple RISC-V program (e.g., adding two numbers):
    ```assembly
    addi x1, x0, 10   ; Load 10 into register x1
    sw x1, 0(x0)      ; Stores the value of x1 into memory at address 0
    lw x2, 0(x0)      ; Loads into x2 the value found in memory at address 0
    ```
3.  Click **"Run"** / **"Step"**.
4.  Watch `x1` and `x2` update in the "Registers" panel and memory being written/read in the "RAM Memory" panel.

---

## 🧩 Architecture Details

The simulator mimics a single-cycle processor architecture.
* **Instruction Memory:** Stores the assembled machine code.
* **Data Memory:** Simulates RAM.
* **Control Unit:** Decodes instructions (R-type, I-type, S-type, B-type, U-type, J-type).
* **ALU:** Performs arithmetic and logic operations.

---

## 👤 Author

**Andrei Tamas**
* GitHub: [@atamas19](https://github.com/atamas19)
* LinkedIn: [Andrei Tamas](https://www.linkedin.com/in/andrei-tamas-405837255/)
