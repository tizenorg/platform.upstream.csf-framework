#!/bin/bash

#echo EmbkcJFK7q > /proc/$$/attr/current
killall TPCSSerDaemon
killall tpcsserdaemontest
killall TWPSerDaemon
killall twpserdaemontest

echo "copying test plug-in ..."
#export LD_LIBRARY_PATH=$PWD
cp -f /tmp/tsc_test/libscclient.so /usr/lib/
cp -f /tmp/tsc_test/libscserver.so /usr/lib/
cp -f /tmp/tsc_test/TPCSSerDaemon /usr/bin/
cp -f /tmp/tsc_test/tpcsserdaemontest /usr/bin/

echo "setup environment ..."
#mkdir testcontents-1
#export TWP_CONTENT_PATH=testcontents-1

echo "Do following to run the test cases ..."
echo "cd /usr/bin/"
echo "./tpcsserdaemontest"


#echo "cleanup test contents ..."
#rm -rf testcontents-1

