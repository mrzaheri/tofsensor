@echo off

cd /D %1
python modules\tools\git_info\generate_header.py %1 %2

echo on
