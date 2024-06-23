cd `dirname $0`
cd ..
cmake -S . -B build
cmake --build build 
cmake --install build