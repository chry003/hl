echo "[Compiling]: Compiling source code..."
gcc src/*.c -Isrc/include -o hl && ./hl ./example/main.hl

status_code=$?
echo -e "[CODE_HL]: $status_code"

if [[ $status_code -eq 0 ]]; then
    ./main
    status_code=$?
    echo -e "[CODE_ASM]: $status_code"
fi
