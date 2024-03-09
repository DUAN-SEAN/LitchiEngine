@echo off
:: 关闭命令回显

cd %~dp0
:: 切换到脚本所在的目录

mkdir Build
cd Build
cmake ..

:: cmake --build . --config Release

PAUSE