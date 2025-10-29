# Delta CNC Controller

This repository contains a simplified but fully documented CNC controller stack
for a delta robot targeting MIPS64 platforms with QNX, VxWorks and Baget
profiles.

## Building

```
cmake -S . -B build -DTARGET_OS=host
cmake --build build
```

Run tests:

```
./build/tests_host
```

Generate documentation:

```
cmake --build build --target docs_api
```
