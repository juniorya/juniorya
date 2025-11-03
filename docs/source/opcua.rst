OPC UA Server
=============

The OPC UA server exposes the CNC status using a model compatible with typical
SCADA tooling. Core nodes include:

* ``Objects/CNC/State`` — overall machine state and active errors.
* ``Objects/CNC/Axis[A|B|C]`` — target position, actual position, velocity, and
  following error per axis.
* ``Objects/CNC/Delta`` — Cartesian pose and workspace limits.
* ``Objects/CNC/Calib`` — calibration offsets and scale factors.

Methods ``Start``, ``Hold``, ``Reset``, ``Home``, ``SaveParams``, and
``LoadParams`` map directly to commands inside the core FSM. When DDS support is
enabled, the OPC UA ↔ DDS bridge inside ``/tools`` keeps both transports in sync.
