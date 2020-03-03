#/bin/sh

clear

rm -rf ./build
mkdir build
cd build

cmake .. -DOpenCL_INCLUDE_DIR="../../../../../../CL_Headers"

cd ..