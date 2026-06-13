#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "usage: $0 input.bin" >&2
    exit 2
fi

input="$1"
output="${input}.txt"

if [ ! -f "$input" ]; then
    echo "input file not found: $input" >&2
    exit 1
fi

xxd -p -c 256 "$input" | tr -d '\n' | tr '[:lower:]' '[:upper:]' > "$output"

echo "wrote $output"
