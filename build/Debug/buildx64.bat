@echo OFF
ninja -f ./build/Debug/buildx64.ninja -v
clang -g %WLIBPATH64% %WLIBVALUES% -L C:/Libraries/_MyLibraries/nGLIB3/libs/x64 -l GLIB.lib ./bin/Debug/x64/obj/*.o -o ./bin/Debug/x64/output.exe