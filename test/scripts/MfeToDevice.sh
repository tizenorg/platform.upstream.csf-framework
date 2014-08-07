#!/bin/bash

export TCS_CFG="release"
. ./scripts/PrepareForDevice.sh

echo -e "\nbuilding framework ..."
pushd ../framework
make clean; make
popd

echo -e "\nbuilding test cases ..."
pushd test_cases
make clean; make
popd

echo -e "\nbuilding MFE plug-in ..."
pushd ../plugin
make clean; make
popd

echo -e "\ncleanup test files on device ..."
sdb -d root on
sdb -d shell rm -rf /root/tcs_test

echo -e "\ncopying test files to device ..."
mkdir -p tcs_test
cp ../plugin/lib/libengine.so tcs_test
cp ../framework/lib/libsecfw.so tcs_test
cp scripts/Test.sh tcs_test
cp scripts/TestNoReplace.sh tcs_test
cp scripts/TestWithReplace.sh tcs_test
cp test_cases/bin/tcstest tcs_test
cp -rf test_cases/mfe-testcontents/samples tcs_test/testcontents
cp test_cases/mfe-testcontents/.tcs.cfg tcs_test
pushd tcs_test/testcontents
tar xvf tcs-testfile-x.mbytes.tar
popd
cp -rf test_cases/mfe-testcontents/mcs.fs/* tcs_test/testcontents
pushd tcs_test
find . -name .svn -print | xargs /bin/rm -rf
popd
sdb -d push tcs_test /root/tcs_test
sdb -d push test_cases/mfe-testcontents/.tcs.cfg /opt/usr/share/sec_plugin/
rm -rf tcs_test

echo -e "\ncleanup binaries .."
pushd ../framework
make distclean
popd

pushd test_cases
make distclean
popd

pushd ../plugin
make distclean
popd

echo -e "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@"
echo "preparation is done, please login to your device and perform following instructions"
echo "sdb -d root on"
echo "sdb -d shell"
echo "cd /root/tcs_test"
echo "chmod +x Test.sh (optional)"
echo "./Test.sh"

# sdb -d shell "cd /root/tcs_test ; export; ./Test.sh"

