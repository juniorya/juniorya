OS Abstraction Layer
====================

The OSAL shields the firmware from platform differences while keeping the
real-time control path allocation free. The main primitives include:

* ``osal_thread`` and ``osal_process`` for isolating the motion loop from
  auxiliary services.
* ``osal_timer`` providing a periodic 125 µs interrupt compatible callback that
  calls into the motion interpolator.
* ``osal_clock_monotonic_ns`` returning nanosecond timestamps used for EtherCAT
  synchronisation and DDS timestamps.
* ``osal_socket_udp``/``osal_socket_tcp`` wrappers with low-latency options and
  consistent error reporting.

For Baget the implementation binds the real-time process to the highest
priority core, while Linux/QEMU builds rely on ``pthread`` affinity and POSIX
interval timers.
