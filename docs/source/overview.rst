Overview
========

This manual describes the portable Delta CNC platform designed for Komdiv-64
controllers and their QEMU emulation. The firmware targets a deterministic
8 kHz control loop, synchronised through EtherCAT DC Sync0, and exposes both
OPC UA and DDS interfaces for supervisory control.

The architecture layers the following components:

* **OSAL** — portable operating system abstraction that provides deterministic
  timers and network primitives without relying on dynamic allocation inside
  the real-time loop.
* **Planner** — jerk-limited S-curve motion planner with spline support that
  accepts G-code commands and produces position setpoints for the CiA-402
  axes.
* **Kinematics** — fixed-point delta robot inverse and forward kinematics with
  singularity detection.
* **Communications** — EtherCAT master, OPC UA server, and DDS middleware for
  machine-to-machine connectivity.

Every subsystem is documented with Doxygen comments in the source tree, and
this Sphinx guide provides mathematical background, configuration steps, and
integration walkthroughs.
