@echo OFF
ninja -f ./build/Release/buildx64.ninja -v
clang -O3 ./bin/Release/x64/obj/*.o -o ./bin/Release/x64/6502EMU.exe