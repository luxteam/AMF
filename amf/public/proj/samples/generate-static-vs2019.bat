SETLOCAL

RMDIR /S /Q vs2017
MKDIR vs2017
CD vs2017

cmake .. -G "Visual Studio 16 2019" -A x64 -DOpenCL_INCLUDE_DIR="C:\Program Files (x86)\OCL_SDK_Light" -DAMF_CORE_STATIC="true"