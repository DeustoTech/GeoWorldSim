echo "============================================"
echo "Installing QT"
echo "============================================"

sudo apt-get install -y qt5-default
sudo apt-get install -y qt5-qmake
sudo apt-get install -y qtbase5-dev-tools
sudo apt-get install -y qtchooser

echo "============================================"
echo "Installing QT Libraries"
echo "============================================"

sudo apt-get install -y libqt5websockets5-dev
sudo apt-get install -y libqt5sql5
sudo apt-get install -y libqt5sql5-psql
sudo apt-get install -y libkf5unitconversion-dev
sudo apt-get install -y libqt5qml5
sudo apt-get install -y qtmultimedia5-dev

echo "============================================"
echo "Installing Other libraries"
echo "============================================"

sudo apt-get install -y unzip
sudo apt-get install -y libpq-dev
sudo apt-get install -y libgeos-dev libgeos++-dev
sudo apt-get install -y libproj-dev
sudo apt-get install -y libgdal-dev
sudo apt-get install -y libsvm-dev
sudo apt-get install -y grass-dev
sudo apt-get install -y libfuzzylite-dev
sudo apt-get install -y libeigen3-dev
sudo apt-get install -y libshp-dev
sudo apt-get install -y libfann-dev
sudo apt-get install -y cmake

echo "============================================"
echo "Downloading and Installing Lemon graph library"
echo "============================================"

mkdir libs
cd libs
wget http://lemon.cs.elte.hu/pub/sources/lemon-1.3.1.zip
unzip lemon-1.3.1.zip 
cd lemon-1.3.1
mkdir build
cd build
cmake .. && make 
cd ../../..

echo "============================================"
echo "Downloading and Installing Qhttp library"
echo "============================================"

mkdir libs
cd libs
wget https://github.com/azadkuh/qhttp/archive/master.zip
mv master.zip qhttp-master.zip
unzip qhttp-master.zip
cd qhttp-master
./update-dependencies.sh
qmake -qt5 && make
cd ../..

echo "============================================"
echo "Installing pug renderer"
echo "============================================"

sudo apt-get install -y nodejs
sudo apt-get install -y nodejs-legacy
sudo apt-get install -y npm
sudo npm install pug-cli -g

echo "============================================"
echo "Installing R"
echo "============================================"

sudo apt-get install r-base