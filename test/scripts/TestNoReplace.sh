#!/bin/bash

echo EmbkcJFK7q > /proc/$$/attr/current

echo "preparing test contents ..."
rm -rf testcontents-1
cp -rf testcontents testcontents-1

echo "setup environment ..."
# export XM_HOME=testcontents-1
export TCS_CONTENT_PATH=testcontents-1
export TCS_ENABLE_SCANREPAIR="yes"
export TCS_SCAN_TYPE="1"
export LD_LIBRARY_PATH=.

echo "backup malware signatures"
mkdir testcontents-1/tmp
mv -f /opt/usr/apps/EmbkcJFK7q/data/database/cfg testcontents-1/tmp/
mv -f /opt/usr/apps/EmbkcJFK7q/data/database/xlm testcontents-1/tmp/
mv -f /opt/usr/apps/EmbkcJFK7q/data/database/sdb testcontents-1/tmp/
cp -rf testcontents-1/cfg /opt/usr/apps/EmbkcJFK7q/data/database/
cp -rf testcontents-1/xlm /opt/usr/apps/EmbkcJFK7q/data/database/
cp -rf testcontents-1/sdb /opt/usr/apps/EmbkcJFK7q/data/database/

echo "start to run test cases ..."
./tcstest
echo "end of test cases ..."

echo "restoring malware signatures"
rm -rf /opt/usr/apps/EmbkcJFK7q/data/database/cfg
rm -rf /opt/usr/apps/EmbkcJFK7q/data/database/xlm
rm -rf /opt/usr/apps/EmbkcJFK7q/data/database/sdb
cp -rf testcontents-1/tmp/cfg /opt/usr/apps/EmbkcJFK7q/data/database/
cp -rf testcontents-1/tmp/xlm /opt/usr/apps/EmbkcJFK7q/data/database/
cp -rf testcontents-1/tmp/sdb /opt/usr/apps/EmbkcJFK7q/data/database/

echo "cleanup test contents ..."
rm -rf testcontents-1

