Diagnostics
===========

Diagnostics counters aggregate EtherCAT cycle slips, DDS delivery latency,
and OPC UA method invocation errors. The ``$SELFTEST`` command executes a
repeatable trajectory (circle, square, spline lemniscate) and reports maximum
position deviation together with missed Sync0 events.

Log output is routed through the ``utils/log`` module, which timestamps entries
with the monotonic clock and records them both to stdout (for pc-linux) and to
a ring buffer inspectable through OPC UA and DDS telemetry.
