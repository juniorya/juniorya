Platforms
=========

The firmware builds with CMake using the ``TARGET_OS`` cache entry:

``pc-linux``
    Native build that uses POSIX threads and sockets. Provides mock EtherCAT
    slaves and loopback DDS for host-side testing.

``qemu-mips64``
    Cross build targeting Komdiv-64 through QEMU. Relies on the same POSIX
    layer but restricts optional dependencies to keep the footprint minimal.

``baget-komdiv64``
    Deployment build for Baget RTOS. The OSAL implementation maps threads and
    timers to the RTOS primitives, while EtherCAT and DDS rely on hardware
    specific drivers.

Select the target with:

.. code-block:: bash

   cmake -S . -B build -DTARGET_OS=baget-komdiv64

The Sync0 frequency is controlled by ``-DSYNC0_RATE_HZ`` and defaults to
8 kHz. Lower frequencies (2 kHz or 1 kHz) trigger the local interpolator inside
``motion`` to up-sample setpoints without heavy computations in the ISR.
