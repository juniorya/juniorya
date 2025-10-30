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
- Optional brachistochrone solver that projects cycloidal time-optimal arcs into
  the delta robot workspace
- EtherCAT master compatible with SOEM DC/Sync0 and CiA-402 CSP/CST/CSV profiles
  including XML import for KEBA drive descriptors
- Calibration subsystem with persistent storage (CRC-protected KV store)
- OPC UA server backed by an OS abstraction layer with auxiliary-process
  isolation
- Host-side graphical simulator featuring 2D/3D trajectory visualisation and a
  static graphical configurator for planner/robot parameters
- Desktop IDE that ingests ``.cncp`` projects, runs TinyML-assisted planning and
  exports OBJ/PPM/JSON artefacts, Structured Text builds and debug traces for
  offline validation
- Comprehensive G-code interpreter supporting modal state (G20/G21, G90/G91),
  feeds/spindle, arcs (G2/G3) and Bézier/NURBS splines (G5/G5.2/G5.3)
- Structured Text IDE pipeline with automatic C translation, syntax-highlighting
  metadata and keyword completions
- TinyML optimiser configurator with profile import/export hooks and runtime
  clamp controls for velocity scaling
- Synthetic machine-vision pipeline for alignment feedback exposed via OPC UA
  and leveraged by the simulator/IDE workflows

## Building

### Host / Simulator (default)

```bash
./configure --target-os=host --enable-brachisto
make
make check
./build/bin/cnc_simulator
```

The default configuration enables Bézier/NURBS G-code support, OPC UA services
and documentation generation. Pass `--enable-brachisto` to activate the
brachistochrone generator; omit the flag if you prefer the classic spline-only
planner. Use `make docs` if you wish to produce the HTML/PDF set after the
initial build.

### QEMU MIPS64

```bash
./configure --target-os=qemu --cc="clang --target=mips64el-linux-gnuabi64"
make
make qemu-selftest
```

The `qemu-selftest` target compiles the synthetic firmware sample and executes
it with `qemu-mips64el` when the emulator and cross compiler are available.

### QNX / VxWorks / Baget

Cross-compiling follows the same flow: select the desired RTOS and compiler
when invoking `configure`, then call `make`. For example, to target QNX using a
vendor-supported toolchain:

```bash
./configure --target-os=qnx --cc=qcc --extra-cflags='-Vgcc_nto64'
make
```

Set `--endian=be` when preparing a Baget big-endian image and adjust
`--extra-*` flags to point at your BSP-provided headers and libraries. The
`make install` stage honours `PREFIX` and the conventional `DESTDIR`.

## Documentation

Doxygen and Sphinx are wired through the Makefile. After configuring, run

```bash
make docs
```

HTML artefacts appear in `build/docs/html`, while PDF manuals are collected in
`build/docs/pdf`. Use `make docs_api` if you only need the raw API reference.

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
./build/bin/cnc_simulator
```

## IDE

The IDE command line front-end is emitted alongside the simulator binaries when
building for host or QEMU targets. It converts ``.cncp`` projects into sampled
trajectories and produces accompanying visual artefacts.

```bash
./build/bin/cnc_ide tests/data/demo_project.cncp ide_output
```

The generated directory contains a JSON snapshot of the configuration UI, OBJ
and PPM trajectory renders plus a text report with timing and limit details. The
IDE also generates a vision configuration summary, verifies XML drive
descriptors referenced from the project file, emits ``*_st.c`` and
``*_st_metadata.json`` artefacts for Structured Text programs and writes a CSV
debug trace with joint angles and Cartesian velocities. Add `TINYML_PROFILE` to
your project file to load optimiser gains on the desktop before deploying them
to the controller.

## Machine Vision and Drive Import

The auxiliary process hosts the OPC UA server and the synthetic machine-vision
pipeline. During development you can override the default camera/workspace
settings in the exported configurator JSON, or provide a dedicated `.cncp`
project via the IDE tooling. To exercise KEBA-compatible drive configuration,
export the device description XML from your commissioning environment and point
the runtime to it:

```bash
export KEBA_ECAT_XML=/path/to/your/keba_drive.xml
./build/bin/cnc_firmware
```

The parser extracts vendor/product identifiers and publishes the selected drive
name through the EtherCAT diagnostics log. Alignment error estimates computed by
the vision module are propagated to the OPC UA address space for client
consumption.

## Licensing

The project is provided for evaluation purposes. Consult the documentation for
third-party components such as `open62541` before redistribution.
