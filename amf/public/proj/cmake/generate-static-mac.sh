#/bin/sh

clear

rm -rf ./mac-gnumake
mkdir mac-gnumake

cd mac-gnumake
cmake .. -DOpenCL_INCLUDE_DIR="../../../../../Thirdparty/OpenCL-Headers" -DAMF_CORE_SHARED=1
cd ..