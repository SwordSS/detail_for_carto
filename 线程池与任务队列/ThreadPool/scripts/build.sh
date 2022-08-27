#/bin/sh
cd ..
mkdir -p build
cd build
cmake .. && make -j8
