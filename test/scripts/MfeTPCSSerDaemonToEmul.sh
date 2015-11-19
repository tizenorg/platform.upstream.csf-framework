#!/bin/bash

. ./scripts/PrepareForEmul.sh
#export TCS_CFG="debug"

echo -e "\nbuilding security channel CLIENT..."
pushd ../framework
make -f Makefile_channel_client clean 
make -f Makefile_channel_client

echo -e "\nbuilding security channel SERVER..."
make -f Makefile_channel_server clean
make -f Makefile_channel_server

echo -e "\nbuilding TPCS Ser Daemon..."
make -f Makefile_TPCSSerDaemon clean
make -f Makefile_TPCSSerDaemon
popd

echo -e "\nbuilding test cases..."
pushd test_cases/tpcsserdaemon
make clean
make
popd

echo -e "\ncleanup test files on device ..."
sdb -e root on
sdb -e shell rm -rf /tmp/tsc_test

echo -e "\ncopying test files to device ..."
mkdir -p tsc_test/
cp ../framework/lib/libscclient.so tsc_test/
cp ../framework/lib/libscserver.so tsc_test/
cp scripts/TPCSSerDaemonTest.sh tsc_test/Test.sh
cp test_cases/tpcsserdaemon/bin/tpcsserdaemontest tsc_test/
cp ../framework/bin/TPCSSerDaemon tsc_test/TPCSSerDaemon

sdb -e push tsc_test /tmp/tsc_test
sdb -e push test_cases/q7097a278m /opt/usr/apps/q7097a278m
sdb -e push test_cases/u7097a278m /opt/usr/apps/u7097a278m
sdb -e push test_cases/n7097a278m /opt/usr/apps/n7097a278m
sdb -e push tsc_test/Test.sh /usr/bin/Test.sh

echo -e "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "preparation is done, please login to your device and perform following instructions"
echo "sdb -e shell"
echo "cd /usr/bin"
echo "chmod +x Test.sh (optional)"
echo "./Test.sh"
