#!/bin/bash

. ./scripts/PrepareForEmul.sh

#export TCS_CFG="debug"

echo -e "\nbuilding framework ..."
pushd ../framework
make clean; make
popd

echo -e "\nbuilding test cases ..."
pushd test_cases
make clean -f WPMakefile; make -f WPMakefile
popd

echo -e "\nbuilding MFE plug-in ..."
pushd ../plugin
make clean; make
popd

echo -e "\ncleanup test files on emulator ..."
sdb -e root on
sdb -e shell rm -rf /root/twp_test

echo -e "\ncopying test files to emulator ..."
mkdir -p twp_test/
cp ../plugin/plugin_i386_release/libwpengine.so twp_test/
cp scripts/WPTest.sh twp_test/Test.sh
cp ../framework/lib/libsecfw.so twp_test/
cp test_cases/bin/twptest twp_test/
cp test_cases/mfe-testcontents/.tcs.cfg twp_test/
sdb -e push twp_test /root/twp_test

rm -rf twp_test

echo -e "\ncleanup binaries .."
pushd ../framework
make clean
popd

pushd test_cases
make distclean -f WPMakefile
popd

pushd ../plugin
make distclean
popd

echo -e "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "preparation is done, please login to your emulator and perform following instructions"
echo "sdb -e root on"
echo "sdb -e shell"
echo "cd /root/twp_test"
echo "chmod +x Test.sh (optional)"
echo "./Test.sh"


