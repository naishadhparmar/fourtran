
# How to build

```
git clone https://github.com/JakobEngel/dso.git
cd dso
sudo apt-get install libsuitesparse-dev libeigen3-dev libboost-all-dev
sudo apt-get install zlib1g-dev
cd thirdparty
tar -zxvf libzip-1.1.1.tar.gz
cd libzip-1.1.1/
./configure
make
sudo make install
sudo cp lib/zipconf.h /usr/local/include/zipconf.h
```
