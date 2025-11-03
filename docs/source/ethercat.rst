EtherCAT and CiA-402
====================

The EtherCAT master implements a SOEM-style stack with Distributed Clocks
support. Sync0 is generated at 8 kHz for mode A or at 1–2 kHz for mode B where
the local interpolator reshapes the trajectory to 8 kHz internally.

PDO layout matches the typical CiA-402 CSP mapping:

* RxPDO: 0x6040 (Control word), 0x607A (Target position), 0x6060 (Modes),
  0x60B0 (Torque feedforward).
* TxPDO: 0x6041 (Status word), 0x6064 (Actual position), 0x606C (Actual
  velocity), 0x6077 (Torque), 0x6061 (Mode display).

SDO helpers expose scaling, limits, and homing constants. EMCY frames feed a
ring buffer mirrored into diagnostics counters. The CiA-402 state machine in
``/cia402`` drives operation enable transitions, quick stop, and fault reset
logic and accepts jerk limited setpoints from the planner at 125 µs intervals.
