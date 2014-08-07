#!/bin/bash

build()
{
	if [ $1 = "i386" ]
	then
	. ./scripts/PrepareForEmul.sh
	else
	. ./scripts/PrepareForDevice.sh
	fi
	
    	cd ../framework
	rm -rf lib
        rm -rf bin
        make
        make -f Makefile_channel_client
        make -f Makefile_channel_server
	make -f Makefile_TWPSerDaemon
        make -f Makefile_TPCSSerDaemon
        cd -
} 

run()
{
        cd ..
        
        #cleanup
        rm Result*
        rm finalResult*
        
        if [ $1 = "i386" ]
        then
	  sdb -e root on
	  sdb -e shell rm /tmp/Result1-Contentscreening_TCS_i386.txt
	  sdb -e shell rm /tmp/Result2-Webprotection_TWP_i386.txt
	  sdb -e shell rm /tmp/Result3-Plugincontrolservice_TPCS_i386.txt
	  sdb -e shell rm /tmp/Result4-Webprotetcioncontrolservice_TWPS_i386.txt
	  
	  #execute permission for scripts
	  chmod +x ./scripts/MfeToEmul.sh
	  chmod +x ./scripts/MfeWPToEmul.sh
	  chmod +x ./scripts/MfeTPCSSerDaemonToEmul.sh
	  chmod +x ./scripts/MfeTWPSerDaemonToEmul.sh
	  
	  #run scripts
	  echo "RUNNING TCS tests"
	  ./scripts/MfeToEmul.sh
	  sdb -e shell "cd /root/tcs_test; chmod a+x ./Test.sh; ./Test.sh>/tmp/Result1-Contentscreening_TCS_i386.txt" 
	  
	  echo "Running TWP tests"
	  ./scripts/MfeWPToEmul.sh
	  sdb -e shell "cd /root/twp_test; chmod a+x ./Test.sh; ./Test.sh>/tmp/Result2-Webprotection_TWP_i386.txt"
	  
	  echo "Running TPC tests"
	  ./scripts/MfeTPCSSerDaemonToEmul.sh
	  sdb -e shell "cd /usr/bin; chmod a+x ./Test.sh; ./Test.sh; ./tpcsserdaemontest>/tmp/Result3-Plugincontrolservice_TPCS_i386.txt"
	  
	  echo "Running TWPS tests"
	  ./scripts/MfeTWPSerDaemonToEmul.sh
	  sdb -e shell "cd /usr/bin; chmod a+x ./Test.sh; ./Test.sh; ./twpserdaemontest>/tmp/Result4-Webprotetcioncontrolservice_TWPS_i386.txt "
	  
	  #pull results
	  sdb -e pull /tmp/Result1-Contentscreening_TCS_i386.txt
	  sdb -e pull /tmp/Result2-Webprotection_TWP_i386.txt
	  sdb -e pull /tmp/Result3-Plugincontrolservice_TPCS_i386.txt
	  sdb -e pull /tmp/Result4-Webprotetcioncontrolservice_TWPS_i386.txt
	  
	  grep -i executed Result*>finalResult_i386.txt
	  
	  #determine pass/fail
	  TCS=`grep "0 failure" Result1-Contentscreening_TCS_i386.txt`
	  TWP=`grep "0 failure" Result2-Webprotection_TWP_i386.txt`
	  TPCS=`grep "0 failure" Result3-Plugincontrolservice_TPCS_i386.txt`
	  TWPS=`grep "0 failure" Result4-Webprotetcioncontrolservice_TWPS_i386.txt`
	  
	  if [ "$TCS" = "" -o "$TWP" = "" -o "$TPCS" = "" -o "$TWPS" = "" ]
	  then
	  echo "FAILED">>finalResult_i386.txt
	  else
	  echo "PASSED">>finalResult_i386.txt
	  fi
	else
	  sdb -d root on
	  sdb -d shell rm /tmp/Result1-Contentscreening_TCS_arm.txt
	  sdb -d shell rm /tmp/Result2-Webprotection_TWP_arm.txt
	  
	  #execute permission for scripts
	  chmod +x ./scripts/MfeToDevice.sh
	  chmod +x ./scripts/MfeWPToDevice.sh
	  
	  #run scripts
	  echo "RUNNING TCS tests"
	  ./scripts/MfeToDevice.sh
	  sdb -d shell "cd /root/tcs_test; chmod a+x ./Test.sh; ./Test.sh>/tmp/Result1-Contentscreening_TCS_arm.txt" 
	  
	  echo "Running TWP tests"
	  ./scripts/MfeWPToDevice.sh
	  sdb -d shell "cd /root/twp_test; chmod a+x ./Test.sh; ./Test.sh>/tmp/Result2-Webprotection_TWP_arm.txt"
	  
	  #pull results
	  sdb -d pull /tmp/Result1-Contentscreening_TCS_i386_arm.txt
	  sdb -d pull /tmp/Result2-Webprotection_TWP_i386_arm.txt
	  
	  grep -i executed Result*>finalResult_arm.txt
	  
	  #determine pass/fail
	  TCS=`grep "0 failure" Result1-Contentscreening_TCS_i386_arm.txt`
	  TWP=`grep "0 failure" Result2-Webprotection_TWP_i386_arm.txt`
	  
	  if [ "$TCS" = "" -o "$TWP" = "" ]
	  then
	  echo "FAILED">>finalResult_arm.txt
	  else
	  echo "PASSED">>finalResult_arm.txt
	  fi
	fi
}

type=$2;
if [ "$type" = "" ]
 then
  type="i386"
fi

if [ "$1" == "Build" ]
then
	build $type
fi

if [ "$1" == "Run" ]
then
	build $type
	run  $type
else
echo "Invalid Parameters"
fi

