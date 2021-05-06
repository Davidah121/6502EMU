@echo OFF
ninja -f ./build/Release/buildx64.ninja -v
clang -O3 %WLIBPATH64% %WLIBVALUES% -L C:/Libraries/_MyLibraries/nGLIB3/libs/x64 -l GLIB.lib ./bin/Release/x64/obj/*.o -o ./bin/Release/x64/6502EMU.exe