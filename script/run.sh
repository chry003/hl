echo "[Compiling]: Removing old binary..."
rm ./hl

if [ -f "./main.asm" ]; then
    rm ./main.asm
    rm ./main.o
    rm ./main
fi

echo "[Compiling]: Compiling source code..."
gcc src/*.c -Isrc/include -o hl && ./hl ./example/main.hl

if [ -f "./main.asm" ]; then
    echo "[Compiling]: Generating object file from assembly..."
    nasm -f elf ./main.asm -o ./main.o

    echo "[Compiling]: Linking..."
    gcc -no-pie -m32 ./main.o -o ./main

    echo "[Compiling]: Runing and status code..."
    ./main
    echo $?

    cat ./main.asm
fi
