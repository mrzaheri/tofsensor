@echo off
cd /D %1
del git_describe.h
python modules\tools\git_info\rename_files.py %*
echo on