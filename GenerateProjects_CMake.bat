@echo off
:: 关闭命令回显

cd %~dp0
:: 切换到脚本所在的目录

mkdir Build
cd Build

::cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release

::cmake --build . --config Debug
::cmake --build . --config Release

PAUSE