rm -r build
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
make -j6