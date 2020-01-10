@echo off
mkdir build-gcc
cd build-gcc
cmake .. -G Ninja && ^
cmake --build . --target test-main && ^
.\test-main.exe
if errorlevel 1 goto error
cd ..

mkdir build-ninja-msvc
cd build-ninja-msvc
cmake .. -G Ninja -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe && ^
cmake --build . --target test-main && ^
.\test-main.exe
if errorlevel 1 goto error
cd ..

mkdir build-msvc
cd build-msvc
cmake .. && ^
cmake --build . --target test-main && ^
.\Debug\test-main.exe
if errorlevel 1 goto error
cd ..

:error
:exit
