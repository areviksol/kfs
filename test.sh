#!/bin/bash
# test.sh - Simple test script for KFS kernel

set -e

echo "=== KFS Kernel Build and Test ==="
echo ""

# Clean and build
echo "1. Cleaning previous build..."
make clean

echo ""
echo "2. Building kernel..."
make

echo ""
echo "3. Verifying kernel binary..."
if [ -f kfs.bin ]; then
    echo "✓ kfs.bin created successfully"
    ls -lh kfs.bin
else
    echo "✗ kfs.bin not found"
    exit 1
fi

echo ""
echo "4. Checking for kernel strings..."
if strings kfs.bin | grep -q "Welcome to KFS"; then
    echo "✓ Welcome message found in kernel"
else
    echo "✗ Welcome message not found"
    exit 1
fi

echo ""
echo "5. Verifying multiboot header..."
if hexdump -C kfs.bin | grep -q "02 b0 ad 1b"; then
    echo "✓ Multiboot header present"
else
    echo "✗ Multiboot header missing"
    exit 1
fi

echo ""
echo "=== All tests passed! ==="
echo ""
echo "To run the kernel in QEMU, use:"
echo "  make run"
