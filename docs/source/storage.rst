Storage
=======

Non-volatile configuration is stored in ``/storage`` using a compact key-value
format. Records are written using a copy-on-write double buffer to guarantee
atomicity even on power failure. Every entry contains:

* Key identifier
* Payload length
* CRC-16 checksum

On Baget the storage backend targets dedicated NVRAM, while Linux/QEMU store
records in a binary file inside the working directory.
