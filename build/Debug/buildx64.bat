@echo OFF
ninja -f ./build/Debug/buildx64.ninja -v
clang -g ./bin/Debug/x64/obj/*.o -o ./bin/Debug/x64/6502EMU.exe