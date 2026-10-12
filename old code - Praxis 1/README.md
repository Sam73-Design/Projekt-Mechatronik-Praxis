# LabyBot V8

The archived robot program is split into the following modules:

- `LabyBot_V8.cpp`: Arduino `setup()` and the main maze-solving `loop()`.
- `include/Config.h`: wiring, calibration, speed limits, PID settings, and direction masks.
- `RobotState`: shared run flags, sensor history, and timers.
- `Application`: hardware instances, button handling, line-tracker calibration, finish detection, and cooling.
- `ToFSensors`: initialization and readings for the three VL53L0X sensors.
- `Motor`: motor direction and PWM output.
- `PIDController`: wall-following feedback controller.
- `Drive`: movement control and motion queue.
- `StackLogic`: path simplification and saved solution.
- `Led`: status LED control.

Declarations are in `include/`; implementations are in `src/`. Include headers rather than `.cpp` files, and compile every `.cpp` in this folder and its `src/` directory together.

This archive is excluded by the repository's current PlatformIO source filter. A separate build must target these sources and provide Arduino, Wire, the VL53L0X library, and the original `Stack.h` library (including `concat`, `length`, `peek`, and `clear`). Do not also compile the active project's `src/main.cpp`, which defines its own `setup()` and `loop()`.

The refactor retains the original control logic and timing. The existing `crossingTime = millis` typo in solve mode was corrected to `crossingTime = millis()`.
