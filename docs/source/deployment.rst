Deployment
==========

1. Configure the build for the target platform.
2. Generate documentation as needed with ``cmake --build build --target docs``.
3. Flash or copy the ``cnc_firmware`` binary to the controller.
4. Provide EtherCAT topology description (XML) and DDS profile files through the
   configuration directory.
5. Run ``$CAL HOME`` to establish initial offsets, then ``$SELFTEST`` to verify
   performance.

Baget deployments rely on the provided toolchain file that configures the
cross-compiler and runtime libraries. QEMU deployments use the same file but
execute under emulation for validation prior to hardware tests.
