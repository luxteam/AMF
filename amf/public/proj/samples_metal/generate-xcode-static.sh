#/bin/sh
clear

rm -rf ./build
mkdir build
cd build

cmake -G "Xcode" .. -DOpenCL_INCLUDE_DIR="../../../../../Thirdparty/OpenCL-Headers" -DAMF_CORE_STATIC="true" -DMETAL_SUPPORT=1

cd ..