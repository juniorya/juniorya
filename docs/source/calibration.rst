Calibration
===========

Calibration sequences run under the ``calib`` module and expose a dedicated
command interface (``$CAL`` family). The supported routines are:

``$CAL HOME``
    Drives the axes towards reference switches and records encoder zero points.
``$CAL ZERO <axis>``
    Applies manual zero-offset adjustments.
``$CAL GEO``
    Executes a least-squares fit on fixture points to refine geometric
    parameters.
``$CAL SCALE <axis> <value>``
    Stores per-axis scale corrections when encoders exhibit systematic error.

Successful sequences persist results inside the storage subsystem, which uses a
copy-on-write key-value store with CRC protection.
