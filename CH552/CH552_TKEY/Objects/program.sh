#!/bin/bash

# Find first matching device
DEVICE=$(ls /dev/ch37x* 2>/dev/null | head -n 1)

# Check if found
if [ -z "$DEVICE" ]; then
    echo "No CH37x device found in /dev"
    exit 1
fi

echo "Using device: $DEVICE"

shopt -s nullglob
HEX_FILES=(*.hex)

if [ ${#HEX_FILES[@]} -eq 0 ]; then
    echo "No .hex file found"
    exit 1
elif [ ${#HEX_FILES[@]} -gt 1 ]; then
    echo "Multiple .hex files found:"
    printf '%s\n' "${HEX_FILES[@]}"
    exit 1
fi

HEX_FILE="${HEX_FILES[0]}"
echo "Using hex file: $HEX_FILE"

# Run programmer
wchisptool -p "$DEVICE" -c ~/CH552_WCHISP.INI -o program -f "$HEX_FILE"
