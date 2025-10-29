#!/usr/bin/env bash
set -euo pipefail

if ! command -v clang >/dev/null 2>&1; then
    echo "Skipping QEMU self-test: clang not available"
    exit 0
fi

if ! command -v qemu-mips64el >/dev/null 2>&1; then
    echo "Skipping QEMU self-test: qemu-mips64el not available"
    exit 0
fi

build_dir=$(mktemp -d)
trap 'rm -rf "${build_dir}"' EXIT

project_root="${PROJECT_SOURCE_DIR:-${PWD}}"

if ! clang --target=mips64el-linux-gnuabi64 -O2 -I"${project_root}" \
    "${project_root}/tests/qemu_selftest.c" -o "${build_dir}/qemu_selftest"; then
    echo "Skipping QEMU self-test: cross compilation failed"
    exit 0
fi

qemu-mips64el "${build_dir}/qemu_selftest"
