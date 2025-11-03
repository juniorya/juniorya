# Delta CNC Platform

Delta CNC Platform is a portable firmware stack for Komdiv-64 based delta
robots. The project is written in ISO C11, targets an 8 kHz real-time loop, and
runs on Baget RTOS, QEMU Komdiv-64, and standard Linux hosts for simulation.

## Features

- OS abstraction layer with 125 µs periodic timers and affinity control.
- EtherCAT master with CiA-402 CSP support and configurable Sync0 frequency.
- Jerk-limited planner with spline support (Bezier3, B-spline3, quintic, and
  NURBS-lite).
- Delta kinematics in Q16.16 fixed-point arithmetic with Jacobian monitoring.
- Calibration sequences ($CAL family) and reliable storage with CRC protection.
- OPC UA server exposing machine state and OPC UA ↔ DDS bridge for telemetry.
- Lightweight DDS middleware with CDR serialisation and QoS management.
- Comprehensive documentation generated with Doxygen and Sphinx.

## Building

```bash
cmake -S . -B build -DTARGET_OS=pc-linux -DSYNC0_RATE_HZ=8000
cmake --build build
ctest --test-dir build
```

Enable optional components:

```bash
cmake -S . -B build -DENABLE_G5=ON -DENABLE_OPCUA=ON -DENABLE_DDS=ON
```

Generate documentation:

```bash
cmake --build build --target docs_api
cmake --build build --target docs
```

## QEMU Komdiv-64

```
cmake -S . -B build-qemu -DTARGET_OS=qemu-mips64 -DSYNC0_RATE_HZ=8000
cmake --build build-qemu
```

Run the emulator with the produced binary and the provided scripts inside
``/tools``.

## Baget Deployment

```
cmake -S . -B build-baget -DTARGET_OS=baget-komdiv64 -DCMAKE_TOOLCHAIN_FILE=toolchains/baget-komdiv64.cmake
cmake --build build-baget
```

Copy ``cnc_firmware`` to the Baget target and provide EtherCAT XML and DDS
profile files in the controller configuration directory.

## Self-test

Execute ``$SELFTEST`` via the console interface to run a circle, a square, and a
spline lemniscate. The results are published through OPC UA and DDS telemetry.
