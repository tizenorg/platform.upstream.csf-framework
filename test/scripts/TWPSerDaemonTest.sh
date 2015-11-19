#!/bin/bash

#echo EmbkcJFK7q > /proc/$$/attr/current

killall TWPSerDaemon
killall twpserdaemontest
killall TPCSSerDaemon
killall tpcsserdaemontest

echo "copying test plug-in ..."
#export LD_LIBRARY_PATH=$PWD
cp -f /tmp/twpserdaemon_test/libscclient.so /usr/lib/
cp -f /tmp/twpserdaemon_test/libscserver.so /usr/lib/
cp -f /tmp/twpserdaemon_test/libsecfw.so /usr/lib/
cp -f /tmp/twpserdaemon_test/TWPSerDaemon /usr/bin/
cp -f /tmp/twpserdaemon_test/twpserdaemontest /usr/bin/

mkdir -p /opt/usr/share/sec_plugin
cp -f /tmp/twpserdaemon_test/libwpengine.so /opt/usr/share/sec_plugin/
mkdir -p /opt/usr/apps/EmbkcJFK7q/data/database
cp -f /tmp/twpserdaemon_test/.tcs.cfg /opt/usr/apps/EmbkcJFK7q/data/
echo "setup environment ..."
#mkdir testcontents-1
#export TWP_CONTENT_PATH=testcontents-1

echo "Do following to run the test cases ..."
echo "cd /usr/bin/"
echo "./twpserdaemontest"
#echo "end of test cases ..."

#echo "cleanup test contents ..."
#rm -rf testcontents-1

