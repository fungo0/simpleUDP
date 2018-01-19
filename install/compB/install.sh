#!/bin/sh
sudo apt-get update  # To get the latest package lists
sudo apt-get install build-essential -y
sudo apt-get install gcc -y
sudo apt-get install libssl-dev -y
sudo apt-get install gdb -y
sudo apt-get install valgrind
#https://www.wolfssl.com/docs/wolfssl-manual/ch2/
cd /vagrant/lib/compB/wolfssl-3.12.2
./configure --enable-dtls --enable-debug --enable-pwdbased --enable-ecc
sudo make install
# check if installed successfully
./testsuite/testsuite.test
#http://www.jianshu.com/p/4ef363a60668
if ! grep -q -F '/usr/local/lib' /etc/ld.so.conf; then
	sudo sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf'
    sudo ldconfig
fi;

#sudo bash /vagrant/install/compB/install.sh
