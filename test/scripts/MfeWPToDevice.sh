#!/bin/bash

export TCS_CFG="release"
. ./scripts/PrepareForDevice.sh

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

echo -e "\ncleanup test files on device ..."
sdb -d root on
sdb -d shell rm -rf /root/twp_test

echo -e "\ncopying test files to device ..."
mkdir -p twp_test/
cp ../plugin/lib/libwpengine.so twp_test/
cp scripts/WPTest.sh twp_test/Test.sh
cp ../framework/lib/libsecfw.so twp_test/
cp test_cases/bin/twptest twp_test/
cp test_cases/mfe-testcontents/.tcs.cfg twp_test/
sdb -d push twp_test /root/twp_test

rm -rf twp_test

echo -e "\ncleanup binaries .."
pushd ../framework
make distclean
popd

pushd test_cases
make distclean -f WPMakefile
popd

pushd ../plugin
make distclean
popd

echo -e "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "preparation is done, please login to your device and perform following instructions"
echo "sdb -d root on"
echo "sdb -d shell"
echo "cd /root/twp_test"
echo "chmod +x Test.sh (optional)"
echo "./Test.sh"


