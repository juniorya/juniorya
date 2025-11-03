DDS Middleware
==============

The DDS module offers a lightweight pub/sub stack written in C. Two backends
are provided:

``minidds``
    Self-contained serializer and discovery for constrained environments such as
    QEMU and Baget. Multicast discovery is emulated through a static peer list.

``cyclonedds``
    Optional integration with Eclipse Cyclone DDS when available on Linux.

Topics are defined as C structures accompanied by CDR descriptors. The
following topics are available:

* ``cnc.state``
* ``cnc.axis``
* ``cnc.delta``
* ``cnc.telemetry``
* ``cnc.command``
* ``cnc.calib``

Reliability (BestEffort or Reliable) and history depth are configured via QoS
structures. The OPC UA bridge subscribes to telemetry and republishes it through
the UA address space.
