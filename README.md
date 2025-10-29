# Delta CNC Controller for MIPS64

This repository hosts a portable CNC firmware targeting delta parallel robots.
It supports fixed-point motion planning, EtherCAT CiA-402 drives, OPC UA
telemetry and a desktop simulator. The codebase is written in ISO C11 and is
organised for deterministic execution on QNX 7.x, VxWorks 7, OSРВ «Багет», as
well as host and QEMU-based development environments.

## Features

- 1 kHz real-time motion loop with optional 2 kHz mode
- Fixed-point Q16.16 kinematics and jerk-limited look-ahead planner with TinyML
  assisted velocity scaling
- Spline engine for Bézier, B-spline, quintic and rational cubic (NURBS-lite)
  trajectories with Shin–McKay additive time-scaling
- EtherCAT master compatible with SOEM DC/Sync0 and CiA-402 CSP/CST/CSV profiles
  including XML import for KEBA drive descriptors
- Calibration subsystem with persistent storage (CRC-protected KV store)
- OPC UA server backed by an OS abstraction layer with auxiliary-process
  isolation
- Host-side graphical simulator featuring 2D/3D trajectory visualisation and a
  static graphical configurator for planner/robot parameters
- Desktop IDE that ingests ``.cncp`` projects, runs TinyML-assisted planning and
  exports OBJ/PPM/JSON artefacts for offline validation
- Synthetic machine-vision pipeline for alignment feedback exposed via OPC UA
  and leveraged by the simulator/IDE workflows

## Building

### Host / Simulator (default)

```bash
cmake -S . -B build -DTARGET_OS=host
cmake --build build
ctest --test-dir build
./build/simulator/delta_simulator
```

### QEMU MIPS64

```bash
cmake -S . -B build-qemu -DTARGET_OS=qemu -DCMAKE_TOOLCHAIN_FILE=toolchains/qemu-mips64.cmake
cmake --build build-qemu
cmake --build build-qemu --target qemu_run
cmake --build build-qemu --target qemu_smoke_test
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

The simulator renders the delta robot workspace to 2D and 3D artefacts (PPM and
OBJ) and produces a JSON configuration snapshot. It uses the production
kinematics, planner and motion modules, making it suitable for validating
trajectories without hardware. The configurator output can be reviewed as a
static UI mock-up while tuning planner and geometry parameters. Vision settings
are surfaced alongside planner and geometry data so that the same profile can be
reused during commissioning.

```bash
./build/simulator/delta_simulator
```

## IDE

The IDE command line front-end is emitted alongside the simulator binaries when
building for host or QEMU targets. It converts ``.cncp`` projects into sampled
trajectories and produces accompanying visual artefacts.

```bash
./build/ide/cnc_ide tests/data/demo_project.cncp ide_output
```

The generated directory contains a JSON snapshot of the configuration UI, OBJ
and PPM trajectory renders plus a text report with timing and limit details. The
IDE also generates a vision configuration summary and verifies XML drive
descriptors referenced from the project file.

## Machine Vision and Drive Import

The auxiliary process hosts the OPC UA server and the synthetic machine-vision
pipeline. During development you can override the default camera/workspace
settings in the exported configurator JSON, or provide a dedicated `.cncp`
project via the IDE tooling. To exercise KEBA-compatible drive configuration,
export the device description XML from your commissioning environment and point
the runtime to it:

```bash
export KEBA_ECAT_XML=/path/to/your/keba_drive.xml
./build/cnc_firmware
```

The parser extracts vendor/product identifiers and publishes the selected drive
name through the EtherCAT diagnostics log. Alignment error estimates computed by
the vision module are propagated to the OPC UA address space for client
consumption.

## Licensing

The project is provided for evaluation purposes. Consult the documentation for
third-party components such as `open62541` before redistribution.
