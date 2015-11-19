#!/bin/bash

. ./scripts/PrepareForEmul.sh
#export TCS_CFG="debug"

echo -e "\nbuilding libsecfw.so..."
pushd ../framework
make clean;
make

echo -e "\nbuilding security channel CLIENT..."
make -f Makefile_channel_client clean 
make -f Makefile_channel_client

echo -e "\nbuilding security channel SERVER..."
make -f Makefile_channel_server clean
make -f Makefile_channel_server

echo -e "\nbuilding TWPSerDaemon..."
make -f Makefile_TWPSerDaemon clean
make -f Makefile_TWPSerDaemon
popd

echo -e "\nbuilding libwpengine.so..."
pushd ../plugin
make clean
make
popd

echo -e "\nbuilding test cases ..."
pushd test_cases/twpserdaemon
make clean
make
popd

echo -e "\ncleanup test files on emulator ..."
sdb -e root on
sdb -e shell rm -rf /tmp/twpserdaemon_test

echo -e "\ncopying test files to emulator ..."
mkdir -p twpserdaemon_test/
cp ../framework/lib/libscclient.so twpserdaemon_test/
cp ../framework/lib/libscserver.so twpserdaemon_test/
cp ../framework/lib/libsecfw.so twpserdaemon_test/
cp scripts/TWPSerDaemonTest.sh twpserdaemon_test/Test.sh
cp ../framework/bin/TWPSerDaemon twpserdaemon_test/
cp test_cases/twpserdaemon/bin/twpserdaemontest twpserdaemon_test/
cp test_cases/mfe-testcontents/.tcs.cfg twpserdaemon_test/

cp ../plugin/plugin_i386_release/libwpengine.so twpserdaemon_test/

sdb -e push twpserdaemon_test /tmp/twpserdaemon_test

# push Test.sh to /usr/bin
sdb -e push scripts/TWPSerDaemonTest.sh /usr/bin/Test.sh

echo -e "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "preparation is done, please login to the emulator and perform following instructions"
echo "sdb -e shell"
echo "cd /usr/bin/"
echo "chmod +x Test.sh (optional)"
echo "./Test.sh"