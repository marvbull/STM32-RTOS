# ⚙️ STM32-RTOS-ContextSwitch

## 📖 Project Overview

This project demonstrates a **custom real-time kernel** on an **STM32 microcontroller**, with a focus on implementing **manual context switching**.  
The project was initially generated using **STM32CubeMX** to set up the hardware abstraction (clocks, timers, SysTick), then extended with **custom kernel logic** and **bare-metal control structures**.

---

## 🧠 Key Features

- Manually implemented **context switching** on ARM Cortex-M  
- Lightweight **thread management** without external RTOS libraries  
- Use of **SysTick** to trigger task switching  
- **Register-level** control with direct manipulation of stacks and processor state  
- Integration of startup code and exception overrides (e.g. `PendSV_Handler`)  
- Minimal footprint – designed for educational and experimental use


## 📂 Repository Structure

- `src/core/src/kernel.c` / `inc/kernel.h` – Core scheduler and task switching logic  
- `src/core/src/asmDumps.s` – Assembly routines for saving/restoring context  
- `doc/` – Proof pictures of more than one running Thread


## ⚙️ Technologies Used

- **STM32F501 Microcontroller**  also worked on **STM32U575 Microcontroller**
- **STM32CubeMX** – Project and peripheral initialization  
- **Bare-metal C** – Kernel logic and memory management  
- **ARM Cortex-M Assembly** – Manual context saving/restoring  
- **SysTick Timer** – Periodic context switch trigger  
- **STM32CubeIDE / Keil / GCC** – Flexible toolchain support


## 🚀 How It Works

1. STM32CubeMX initializes the project (clock tree, SysTick, memory layout, vector table).  
2. Tasks are manually defined with separate stacks.  
3. On each tick, `SysTick_Handler` triggers the `PendSV_Handler` for a context switch.  
4. `asmDumps.s` saves registers (e.g., R4–R11) and restores the next task's context.  
5. The system returns to thread mode using **PSP** (Process Stack Pointer).

The project builds a minimal RTOS-like system with **manual scheduling**, without relying on any third-party RTOS.


## 🛠️ Getting Started

### Requirements

- STM32 board (e.g. STM32F103, STM32F407)  
- STM32CubeIDE or alternative ARM GCC toolchain  
- ST-Link programmer/debugger  
- Optional: UART or GPIO for task status output

### Steps

1. Open or regenerate the `.ioc` file with **STM32CubeMX**  
2. Import project into STM32CubeIDE (or build using Makefile/GCC)  
3. Modify `kernel.c` to define your own threads  
4. Build and flash to target board  
5. Use debugger or GPIO/UART output to verify task switching

## ✅ Project Status

✅ **Functional Prototype**  
The system performs reliable context switches between multiple threads.  
All switching is implemented using custom logic – no RTOS frameworks involved.

Great base for deeper exploration of embedded RTOS internals.

## 📄 License

This project is licensed under the **MIT License** – see the [LICENSE](./LICENSE) file for details.
