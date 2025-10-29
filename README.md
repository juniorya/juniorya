# Delta CNC Controller for MIPS64

This repository hosts a portable CNC firmware targeting delta parallel robots.
It supports fixed-point motion planning, EtherCAT CiA-402 drives, OPC UA
telemetry and a desktop simulator. The codebase is written in ISO C11 and is
organised for deterministic execution on QNX 7.x, VxWorks 7, OSРВ «Багет», as
well as host and QEMU-based development environments.

## Features

- 1 kHz real-time motion loop with optional 2 kHz mode
- Fixed-point Q16.16 kinematics and jerk-limited look-ahead planner
- Spline engine for Bézier, B-spline, quintic and rational cubic (NURBS-lite)
  trajectories with Shin–McKay additive time-scaling
- EtherCAT master compatible with SOEM DC/Sync0 and CiA-402 CSP/CST/CSV profiles
- Calibration subsystem with persistent storage (CRC-protected KV store)
- OPC UA server backed by an OS abstraction layer
- Host-side graphical simulator that renders end-effector trajectories and
  drive states without physical hardware

## Building

### Host / Simulator (default)

```bash
cmake -S . -B build -DTARGET_OS=host
cmake --build build
ctest --test-dir build
./build/simulator/delta_simulator --trajectory data/demo_gcode.ngc
```

### QEMU MIPS64

```bash
cmake -S . -B build-qemu -DTARGET_OS=qemu -DCMAKE_TOOLCHAIN_FILE=toolchains/qemu-mips64.cmake
cmake --build build-qemu
cmake --build build-qemu --target qemu_run
```

### QNX / VxWorks / Baget

The `toolchains/` directory contains example toolchain files. Select the target
via the `TARGET_OS` cache entry. For example, building for QNX:

```bash
cmake -S . -B build-qnx -DTARGET_OS=qnx -DCMAKE_TOOLCHAIN_FILE=toolchains/qnx.cmake
cmake --build build-qnx
```

Refer to the documentation under `docs/` for deployment instructions.

## Documentation

Documentation is generated with Doxygen and Sphinx. The combined HTML/PDF set
is produced by the `docs` target, while the raw API reference is available
through `docs_api`.

```bash
cmake --build build --target docs
```

Generated files are placed in `build/docs`.

## Tests and Self-Diagnostics

- Unit tests cover kinematics, spline geometry, planner limits and storage
  integrity.
- Integration tests exercise the EtherCAT state machine, OPC UA model and the
  simulated motion loop.
- `$SELFTEST` demonstrates the canonical acceptance trajectory (homing, circle,
  square and figure-eight) and records tracking metrics.

## Simulator

The simulator renders the delta robot workspace to a simple image sequence and
CSV log. It uses the production kinematics, planner and motion modules, making
it suitable for validating trajectories without hardware.

```bash
./build/simulator/delta_simulator --headless --output ./sim_output
```

## Licensing

The project is provided for evaluation purposes. Consult the documentation for
third-party components such as `open62541` before redistribution.
