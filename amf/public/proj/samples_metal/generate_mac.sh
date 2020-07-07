#/bin/sh

clear

rm -rf ./build;
mkdir build;
cd build;

cmake .. -DOpenCL_INCLUDE_DIR="../../../../../../CL_Headers" -DAMF_CORE_STATIC=1 -DMETAL_SUPPORT=1

cd ..