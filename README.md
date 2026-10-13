# PECD — Lab 01: Getting Started with ESP32

Code examples and presentation for the first PECD laboratory session at FRI
(Faculty of Computer and Information Science, University of Ljubljana).
The two-hour lab introduces ESP32 development with ESP-IDF, FreeRTOS tasks,
GPIO interrupts, and sleep modes.

## Repository contents

| File | Description |
| --- | --- |
| [Lab_01_ESP32.pptx](Lab_01_ESP32.pptx) | Lab presentation: board overview, pinout, development workflow, four exercises, and completion checklist. |
| [code/01_system_info.c](code/01_system_info.c) | Logs the ESP-IDF version, chip model and revision, CPU core count, wireless features, flash size, and heap statistics; prints a heartbeat every two seconds. |
| [code/02_dual_core_tasks.c](code/02_dual_core_tasks.c) | Runs two FreeRTOS tasks pinned to cores 0 and 1, with periods of 700 ms and 1100 ms. |
| [code/03_button_interrupt.c](code/03_button_interrupt.c) | Handles the BOOT button on GPIO0 using a falling-edge interrupt, a task notification, and a 200 ms debounce interval. |
| [code/04_sleep_modes.c](code/04_sleep_modes.c) | Demonstrates two seconds of Light-sleep and five seconds of Deep-sleep, including an RTC-memory boot counter. |

## Requirements

- A dual-core ESP32 development board. The presentation uses a 30-pin board;
  the button example assumes that BOOT is connected to GPIO0.
- A USB data cable and access to the board's serial port.
- Visual Studio Code with the Espressif ESP-IDF extension installed and
  configured with ESP-IDF and its development tools.
- An ESP-IDF version that provides the APIs used in these examples, including
  `esp_sleep_get_wakeup_causes()` for the sleep exercise.

## Running the examples

The `code/` directory contains standalone source files, not complete ESP-IDF
projects. Each file defines its own `app_main()`, so run one example at a time.

1. In VS Code, use **ESP-IDF: New Project** from the Command Palette to create
   a basic project from the `hello_world` template, then open that project.
2. Replace the project's `main/main.c` with the desired example. Ensure that
   `main/CMakeLists.txt` registers that filename as its component source.
3. Use **ESP-IDF: Set Espressif Device Target** to select `esp32`, and
   **ESP-IDF: Select Port to Use** to select the board's serial port.
4. Use **ESP-IDF: Build Your Project**, **ESP-IDF: Flash Your Project**, and
   **ESP-IDF: Monitor Your Device** from the Command Palette or the extension's
   toolbar. Select UART flashing when prompted. The extension also provides
   **ESP-IDF: Build, Flash and Start a Monitor on Your Device**.
5. Observe the serial output, then replace `main/main.c` with the next example
   and repeat the build, flash, and monitor steps in VS Code.

All lab builds use the VS Code ESP-IDF extension. The terminal commands shown
in the presentation are equivalents for reference.

See the [official ESP-IDF extension documentation](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/index.html)
for installation and configuration.

The presentation labels exercises as `code/01_system_info`, etc.; the source
files in this repository use the corresponding `.c` filenames listed above.

## On-board buttons

| Button | Connection | Use in this lab |
| --- | --- | --- |
| BOOT | GPIO0; pressing connects it to ground | Exercise 3 reads this button with an internal pull-up and a falling-edge interrupt. Released is high; pressed is low. |
| EN / RESET | Chip enable/reset (EN), not a GPIO | Restarts the ESP32; cannot be read as a normal application button. |

The code correctly selects `GPIO_NUM_0` for BOOT. GPIO0 need not appear on the
board's header for the on-board button to work. Do not substitute EN as the
interrupt input. Holding BOOT during reset selects download mode; release it
before starting the application.

This matches the standard ESP32 development-board wiring documented in
[Espressif's ESP32-DevKitC guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html).
For a different board revision, confirm its button wiring against its schematic.

## Lab checkpoints

1. **System information:** show a successful flash, recognizable chip information,
   and the periodic heartbeat. Explain the roles of flash and RAM.
2. **Tasks on both cores:** show interleaved output from cores 0 and 1. Explain
   why the tasks call `vTaskDelay()`.
3. **Button interrupt:** produce five deliberate BOOT-button events. Explain
   why the interrupt handler only notifies a task, while the task debounces and
   logs the event. Briefly press BOOT; do not hold it while resetting the board,
   because GPIO0 low selects download mode.
4. **Sleep modes:** show execution resuming after Light-sleep and `app_main()`
   restarting after Deep-sleep. Compare ordinary RAM with the RTC boot counter.

By the end of the lab, be ready to explain the relationship between ESP-IDF and
FreeRTOS, task scheduling, interrupt-to-task communication, and the state retained
in each sleep mode.
