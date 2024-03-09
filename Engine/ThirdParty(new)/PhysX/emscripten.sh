cd build
cmake -DCMAKE_TOOLCHAIN_FILE=/Users/diharaw/emsdk/emscripten/1.37.9/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..

make