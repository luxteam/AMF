SETLOCAL

RMDIR /S /Q vs2019
MKDIR vs2019
CD vs2019

cmake .. -G "Visual Studio 16 2019" -A x64 -DOpenCL_INCLUDE_DIR="C:\Program Files (x86)\OCL_SDK_Light" -DAMF_CORE_STATIC="true"