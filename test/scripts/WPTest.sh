#!/bin/bash

echo "copying test plug-in ..."
export LD_LIBRARY_PATH=$PWD
mkdir -p /opt/usr/share/sec_plugin
cp -f libwpengine.so /opt/usr/share/sec_plugin

mkdir -p /opt/usr/apps/EmbkcJFK7q/data/database
cp -f .tcs.cfg /opt/usr/apps/EmbkcJFK7q/data/

echo "setup environment ..."
mkdir testcontents-1
export TWP_CONTENT_PATH=testcontents-1

echo "start to run test cases ..."
./twptest
echo "end of test cases ..."

echo "cleanup test contents ..."
rm -rf testcontents-1

