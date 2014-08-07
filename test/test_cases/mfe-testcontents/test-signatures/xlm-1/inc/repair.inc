;macros to support repair.vs library


;//////////////////////////////////////////////////////////
;Name
;	CheckRepairFlag
;
;Description
;	Checks the status of the Repair flag.  If the repair flag is set, control will jump to the provided label
;
;Call
;   Label of Repair code
;   
;Return
;	(The macro will exit if the rrapir flag is unset)
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////

#define CheckRepairFlag(p) \
	test      FL, UVE_FL_CLEAN \
	JNZ       p \
	exit


   
;//////////////////////////////////////////////////////////
;Name
;	KillScannedFile
;
;Description
;	Terminates process of scanned file.  Macro calls KillProcess with scanned file path as argument.
;
;Call
;
;Return
;	r0 = Error code. 0 for success, otherwise error code.
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////

#define KillScannedFile() \
    mov SCAN_FILE_PATH, r0 \
    calli KillProcess

;//////////////////////////////////////////////////////////
;Name
;	DeleteDirectory
;
;Description
;	Convenience macro that calls DeleteFile
;
;Call
;	r0 = Full file pathname
;	r1 = Recursion flag. Non-zero value for recursive deletion of the specified directory. 0 for no recursion.
;
;Return
;	r0 = Error code. 0 for success, otherwise error code.
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////
      
#define DeleteDirectory() \
    calli DeleteFile
	
	
;//////////////////////////////////////////////////////////
;Name
;	DeleteScannedFile
;
;Description
;	Macro relases file handle, loads path and then deletes the scanned file 
;
;Call
;
;Return
;	r0 = Error code. 0 for success, otherwise error code.
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////
	
#define DeleteScannedFile() \
    swi 25 \
    mov 0, r1 \
    mov SCAN_FILE_PATH, r0 \
    calli DeleteFile
    
    
;//////////////////////////////////////////////////////////
;Name
;	RepairSuccessful
;
;Description
;	Macro clears Virus Found flag 
;
;Call
;
;Return
;
;Flags Modified
;   X
;//////////////////////////////////////////////////////////
    
#define RepairSuccessful() \
    and FL, ~UVE_FL_VIRUSFOUND, FL

