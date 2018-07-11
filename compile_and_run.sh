rm -rf build/

mkdir build

qmake -qt5

make -j8 

mv moc* build

mv *.o build

cd build/

LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../libs/qhttp-master/xbin/" C_INCLUDE_PATH="$C_INCLUDE_PATH:../libs/lemon-1.3.1/lemon" ../GeoSmartSim