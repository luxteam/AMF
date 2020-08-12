SETLOCAL

RMDIR /S /Q vs2017
MKDIR vs2017
CD vs2017

cmake .. -G "Visual Studio 15 2017" -Dgtest_force_shared_crt=ON -A x64 -DOpenCL_INCLUDE_DIR="C:\Program Files (x86)\OCL_SDK_Light" -DAMF_CORE_STATIC="true"