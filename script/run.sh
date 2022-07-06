echo "[Compiling]: Removing old binary..."

echo "[Compiling]: Compiling source code..."
gcc src/*.c -Isrc/include -o hl && ./hl ./example/main.hl

echo "[Compiling]: Generating object file from assembly..."
nasm -f elf ./main.asm -o ./main.o

echo "[Compiling]: Linking..."
gcc -no-pie -m32 ./main.o -o ./main

echo "[Compiling]: Runing and status code..."
./main
echo $?


mv main.* "./bin/"
mv main "./bin/"
mv hl "./bin/"
