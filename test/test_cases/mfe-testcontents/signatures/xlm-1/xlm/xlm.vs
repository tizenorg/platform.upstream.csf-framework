library 1, "1"
#include "uvebase.inc"
#include "repair.inc"

export RepairInit, 0
export RepairExit, 1
export DeleteFile, 2
export KillProcess, 3
export GetFilePath, 4
export GetFileName, 5
export CleanScannedFile, 6
export CleanRelatedFile, 7
export BulkProcess, 8
export BulkFindAndExit, 9
export GetDexStringRange, 10

;//////////////////////////////////////////////////////////
;Name
;   RepairInit
;
;Description
;    Clean up memory allocated by RepairInit
;Call
;    r0, 1 means detection is for code section of an executable, 0 otherwise.
;Return
;   r0 = 0 if succesful,
;        1 if clean flag is clear
;        2 if unable to get scanned file path
;        3 if within archive
;         
;Flags Modified
;
;//////////////////////////////////////////////////////////

RepairInit:
namespace RepairInit
;    DEBUG_IMM(" DEBUG: RepairInit")

    xor SCAN_FILE_PATH, SCAN_FILE_PATH, SCAN_FILE_PATH
    calln GetFilePath
    cmp r0, 0
    JE $GetFileError

    calln IsWithinArchive
    test FL, UVE_FL_OPOK
    JNZ $WithinArchiveError     ; pipe character found, return error

    ; remount /system folder (if needed)
    mov 1, r0
    mov SCAN_FILE_PATH, r1
    calln RemountSystem
    cmp r0, 0
    JNZ $RemountSystemError

$CleanExit:
    mov 0, r0
    jmp $Quit

$GetFileError:
    mov 2, r0
    jmp $Quit

$WithinArchiveError:
    mov 3, r0

$RemountSystemError:
    mov 4, r0

$Quit:    
    ret
namespace

;//////////////////////////////////////////////////////////
;Name
;   RepairExit
;
;Description
;    Clean up memory allocated by RepairInit
;Call
;
;Return
;   
;Flags Modified
;
;//////////////////////////////////////////////////////////

RepairExit:
namespace RepairExit
;    DEBUG_IMM(" DEBUG: RepairExit")

    ; remount /system folder (if needed)
    mov 0, r0
    mov SCAN_FILE_PATH, r1
    calln RemountSystem

    ; release memory allocatd for scanned file path
    cmp SCAN_FILE_PATH, 0
    JE $Quit
    
    mrel SCAN_FILE_PATH
$Quit:
    ret
namespace

;//////////////////////////////////////////////////////////
;Name
;	DeleteFile
;
;Description
;	Deletes a single file, a specified directory and optionally all its files and subdirectories. 
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

DeleteFile:
namespace DeleteFile
    swi SWI_RM
    ret
namespace


;//////////////////////////////////////////////////////////
;     INTERNAL functions
;//////////////////////////////////////////////////////////




;//////////////////////////////////////////////////////////
;Name
;	GetFilePath
;
;Description
;	Retrieves file path of the scanned file.
;   (INTERNAL - called by RepairInit)
;
;Call
;    r0, 1 means detection is for code section of an executable, 0 otherwise.
;
;Return
;	r0 = Length of the file path stored, including th terminator 0 if error.
;   SCAN_FILE_PATH = Buffer address where the file path is to be stored. 
;   SCAN_FILE_PATH_LENGTH = r0. 
;
;Flags Modified
;
;//////////////////////////////////////////////////////////

GetFilePath:
namespace GetFilePath
    mov r0, r2
    mov FILE_PATH_LENGTH, r1
    mget r1, SCAN_FILE_PATH

    strb 0, SCAN_FILE_PATH
    
    mov SCAN_FILE_PATH, r0 			
    swi SWI_VFILE_PATH ;r0=pointer to buffer,r1=size of buffer
    mov r0, SCAN_FILE_PATH_LENGTH
    
    ; If this is a detection for code section of an executable file
    cmp r2, CODE_SECTION
    JNE $Quit

    ; To get the position of "|" in file path of virtual file of code section
    lea $Code_Section_Tail, r0
    swi UVE_SWI_STRLEN
    mov r0, r3
    mov -1, r2
    add SCAN_FILE_PATH, SCAN_FILE_PATH_LENGTH, r1
    sub r1, r0, r1
    sub r1, 1, r1
    
    ; To compare if the file name is "|.text", virtual file name must be this, this is defined by MCS engine 3.*
    mov r1, r0
    lea $Code_Section_Tail, r0
    swi SWI_STR_CMP
    cmp r0, 0
    JE $Remove_Section_Tail
    xor r0, r0 ,r0
    jmp $Quit
    
    ; To get rid of the "|.text" in the file path, the actual file to be delete should be without this tail.
$Remove_Section_Tail:
    strb 0, r1   
    sub SCAN_FILE_PATH_LENGTH, r3, SCAN_FILE_PATH_LENGTH
    mov SCAN_FILE_PATH_LENGTH, r0

$Quit:
    ret

$Code_Section_Tail:
    bdata ("|.text", 0)
namespace


;//////////////////////////////////////////////////////////
;Name
;	IsWithinArchive
;
;Description
;	Checks if the scanned file is in an archive.  
;	(INTERNAL - called by RepairInit)
;
;Call
;	r0 = Full pathname of the scanned file
;
;Return
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////

IsWithinArchive:
namespace IsWithinArchive
    mov SCAN_FILE_PATH, r0
    mov SCAN_FILE_PATH_LENGTH, r1
    xregxfind r0, r1, --, (/\\|/)
    ret
namespace

;//////////////////////////////////////////////////////////
;Name
;	KillProcess
;
;Description
;	Terminates process.
;
;Call
;	r0 = Path of the binary corresponding to the process to be terminated.
;   [Symbian] If the first character (drive letter) of the path name is '?', then the action is applied to every drive on the device.
;
;Return
;	r0 = Error code. 0 for success, otherwise error code.
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////

KillProcess:
#define R_TMP r10
namespace KillProcess
    push R_TMP

    ; Save FILE_PATH pointer.
    mov r0, R_TMP

    ; Kill Process is only for ELF file.
    calln IsElf
    cmp r0, 1
    JNE $Exit

    mov R_TMP, r0
;    DEBUG_IMM("KillProcess: UVE_SWI_PROCESS_KILL")
;    DEBUG_STR(r0)
    swi UVE_SWI_PROCESS_KILL

$Exit:
    pop R_TMP
    ret
namespace
#undef R_TMP


;//////////////////////////////////////////////////////////
;Name
;	GetFileName
;
;Description
;	Get file name, extertion included.
;
;Call
;	r0 = Memory for the full file path.
;        r1 = Full file path length, the ending NULL included
;
;Return
;	r0 = Error code. 0 for success, otherwise error code.
;        r1 = The address of the file name.
;        r2 = Length of the file name, ending NULL is included.
;Flags Modified
;	
;//////////////////////////////////////////////////////////
GetFileName:
namespace GetFileName
    cmp r0, 0
    JE $FilePathAddressError
    mov r0, r3
    
    cmp r1, 0
    JE $FilePathLenghtError
    
    ; Get the address of the last character of the file path
    add r0, r1, r1
    ; sub 2 to the address of last charcter of the full file path.
    sub r1, 2, r1
    JBS $FilePathLenghtError
    
    mov 1, r0
$GetFirstSlash:
    ldrb r1, r2
    cmp r2, 5ch
    JE  $FileNameFound
    inc r0, r0
    dec r1, r1
    cmp r1, r3
    JNE $GetFirstSlash

$FileNameNotFound:
    mov 1, r0
    ret
$FilePathAddressError:
    mov 2, r0
    ret
$FilePathLenghtError:
    mov 3, r0
    ret
$FileNameFound:
    inc r1, r1
    mov r0, r2
    xor r0, r0, r0
    ret
namespace

;//////////////////////////////////////////////////////////
;Name
;	CleanScannedFile
;
;Description
;	Kill process of current scanned file and remove it.
;
;Call
;
;Return
;	r0 = Error code. 0 for success, otherwise error code.
;
;Flags Modified
;	UVE_FL_VIRUSFOUND, if clean successfully, it will be unset
;//////////////////////////////////////////////////////////
CleanScannedFile:
namespace CleanScannedFile
    calln RepairInit
    
    cmp r0, 0
    JNE $Clean_Quit  
    
    mov SCAN_FILE_PATH, r0
    calln KillProcess
    
;    mov SCAN_FILE_PATH, r0
    swi UVE_SWI_CLOSECURFILE
    mov 0, r1
    mov SCAN_FILE_PATH, r0
    calln DeleteFile
    
    cmp r0, 0
    JNE $Clean_Quit
    
    and FL, ~UVE_FL_VIRUSFOUND, FL
$Clean_Quit:
    calln RepairExit
    ret
namespace

;//////////////////////////////////////////////////////////
;Name
;    CleanRelatedFile
;
;Description
;    Kill process of current related file and remove it.
;
;Call
;    r0 = Memory for removal target information:
;
;    -----------------------------------------------
;    Ofs Siz Content
;    -----------------------------------------------
;     0   4 file size
;     4   4 CRC Check - Offset
;     8   4 CRC Check - Size
;    12   4 CRC Check - CRC value
;    16   4 Binary pattern Check - Offset
;    20   4 Binary pattern Check - Size
;    24   x Binary pattern Check - Binary data
;  24+x   y file path, zero terminated
;    -----------------------------------------------
;
;Return
;    r0 = Error code. 0 for success, otherwise error code.
;
;Flags Modified
;    no FL modification
;//////////////////////////////////////////////////////////
CleanRelatedFile:
#define R_IN_FILE_SIZE     r10
#define R_IN_CRC_OFFSET    r11
#define R_IN_CRC_SIZE      r12
#define R_IN_CRC_VALUE     r13
#define R_IN_BINARY_OFFSET r14
#define R_IN_BINARY_SIZE   r15
#define R_IN_BINARY_DATA   r16
#define R_IN_FILE_PATH     r17
#define R_STAT_SIZE        r18
#define R_STAT_ATTR        r19
#define R_FH               r20
#define R_CRC_BUF          r21
#define R_BINARY_BUF       r22
#define R_STAT_CRC         r23
namespace CleanRelatedFile
    push R_IN_FILE_SIZE
    push R_IN_CRC_OFFSET
    push R_IN_CRC_SIZE
    push R_IN_CRC_VALUE
    push R_IN_BINARY_OFFSET
    push R_IN_BINARY_SIZE
    push R_IN_BINARY_DATA
    push R_IN_FILE_PATH
    push R_STAT_SIZE
    push R_STAT_ATTR
    push R_FH
    push R_CRC_BUF
    push R_BINARY_BUF
    push R_STAT_CRC
    push FL

    ; Initialize.
    mov 0, R_CRC_BUF
    mov 0, R_BINARY_BUF
    mov 0, R_FH

    ; Get params.
;    DEBUG_IMM("Get params")
    ldrw r0, R_IN_FILE_SIZE
    ldrw r0[4], R_IN_CRC_OFFSET
    ldrw r0[8], R_IN_CRC_SIZE
    ldrw r0[12], R_IN_CRC_VALUE
    ldrw r0[16], R_IN_BINARY_OFFSET
    ldrw r0[20], R_IN_BINARY_SIZE
    add r0, 24, R_IN_BINARY_DATA
    add R_IN_BINARY_DATA, R_IN_BINARY_SIZE, R_IN_FILE_PATH
;    DEBUG_REG(R_IN_FILE_SIZE)
;    DEBUG_REG(R_IN_CRC_OFFSET)
;    DEBUG_REG(R_IN_CRC_SIZE)
;    DEBUG_REG(R_IN_CRC_VALUE)
;    DEBUG_REG(R_IN_BINARY_OFFSET)
;    DEBUG_REG(R_IN_BINARY_SIZE)
;    DEBUG_REG(R_IN_BINARY_DATA)
;    DEBUG_STR(R_IN_FILE_PATH)

    ; Get Target File Stat
;    DEBUG_IMM("Get Target File Stat")
    mov R_IN_FILE_PATH, r0
    CHK_ERR_SWI_FLR0(SWI_FILESTAT, $Quit)
    mov r2, R_STAT_ATTR
    dd2i x0, R_STAT_SIZE
;    DEBUG_REG(R_STAT_ATTR)
;    DEBUG_REG(R_STAT_SIZE)

    ; Check File Attribute - Path is a file
;    DEBUG_IMM("Check File Attribute - Path is a file")
    cmp R_STAT_ATTR, 0
    JNE $Quit

    ; Check File Size.
;    DEBUG_IMM("Check File Size")
    cmp R_STAT_SIZE, R_IN_FILE_SIZE
    JNE $Quit

    ; Read File Data
;    DEBUG_IMM("Read File Data - fopen")
    mov R_IN_FILE_PATH, r0
    mov F_FOPEN_READ, r1
    CHK_ERR_SWI_FLR0(SWI_FOPEN, $Quit)
    mov r1, R_FH

;    DEBUG_IMM("Read File Data - fseek for CRC")
    mov R_FH, r0
    mov F_FSEEK_SET, r1
    du2d R_IN_CRC_OFFSET, x0
    CHK_ERR_SWI_FLR0(SWI_FSEEK, $Quit)

;    DEBUG_IMM("Check CRC Check Size")
    cmp R_IN_CRC_SIZE, 0
    JZ $SKIP_CRC_CHECK

;    DEBUG_IMM("Read File Data - allocate memory for CRC")
    mget R_IN_CRC_SIZE, R_CRC_BUF

;    DEBUG_IMM("Read File Data - fread for CRC")
    mov R_FH, r0
    mov R_CRC_BUF, r1
    mov R_IN_CRC_SIZE, r2
    CHK_ERR_SWI_FLR0(SWI_FREAD, $Quit)

;    DEBUG_IMM("Read File Data - check fread size for CRC")
    cmp r1, R_IN_CRC_SIZE
    JNE $Quit

    ; Calculate CRC check sum.
;    DEBUG_IMM("Calculate CRC check sum")
    mov 0, r0
    mcsum r0, R_CRC_BUF, R_IN_CRC_SIZE, R_STAT_CRC
;    DEBUG_REG(R_STAT_CRC)

    ; Check CRC value.
;    DEBUG_IMM("Check CRC value")
    cmp R_STAT_CRC, R_IN_CRC_VALUE
    JNE $Quit

$SKIP_CRC_CHECK:

;    DEBUG_IMM("Check Binary Check Size")
    cmp R_IN_BINARY_SIZE, 0
    JZ $SKIP_BINARY_CHECK

;    DEBUG_IMM("Read File Data - fseek for Binary")
    mov R_FH, r0
    mov F_FSEEK_SET, r1
    du2d R_IN_BINARY_OFFSET, x0
    CHK_ERR_SWI_FLR0(SWI_FSEEK, $Quit)

;    DEBUG_IMM("Read File Data - allocate memory for Binary")
    mget R_IN_BINARY_SIZE, R_BINARY_BUF

;    DEBUG_IMM("Read File Data - fread for Binary")
    mov R_FH, r0
    mov R_BINARY_BUF, r1
    mov R_IN_BINARY_SIZE, r2
    CHK_ERR_SWI_FLR0(SWI_FREAD, $Quit)

;    DEBUG_IMM("Read File Data - check fread size for Binary")
    cmp r1, R_IN_BINARY_SIZE
    JNE $Quit

    ; Memcmp binary data.
    mov R_BINARY_BUF, r0
    mov R_IN_BINARY_DATA, r1
    mov R_IN_BINARY_SIZE, r2
    CHK_ERR_SWI_FLR0(SWI_MEMCMP, $Quit)

$SKIP_BINARY_CHECK:

;    DEBUG_IMM("Read File Data - fclose")
    mov R_FH, r0
    CHK_ERR_SWI_FLR0(SWI_FCLOSE, $Quit)

    ; comment out for kill process issue for apk.
    ; Kill Process
;    DEBUG_IMM("Kill Process")
;    mov R_IN_FILE_PATH, r0
;    CHK_ERR_SWI_FL(SWI_KILL, $Quit)

    ; Remount system folder with RW.
;    DEBUG_IMM("Remount system folder with RW")
    mov 1, r0
    mov R_IN_FILE_PATH, r1
    calln RemountSystem

    ; Delete File
;    DEBUG_IMM("Delete File")
    mov R_IN_FILE_PATH, r0
    CHK_ERR_SWI_FLR0(SWI_RM, $Quit)

    ; Remount system folder with RO.
;    DEBUG_IMM("Remount system folder with RO")
    mov 0, r0
    mov R_IN_FILE_PATH, r1
    calln RemountSystem

$Quit:
    SYS_FCLOSE_FH(R_FH)
    SYS_MREL_BUF(R_CRC_BUF)
    SYS_MREL_BUF(R_BINARY_BUF)

;    DEBUG_IMM("Error code:")
;    DEBUG_REG(r0)

    pop FL
    pop R_STAT_CRC
    pop R_BINARY_BUF
    pop R_CRC_BUF
    pop R_FH
    pop R_STAT_ATTR
    pop R_STAT_SIZE
    pop R_IN_FILE_PATH
    pop R_IN_BINARY_DATA
    pop R_IN_BINARY_SIZE
    pop R_IN_BINARY_OFFSET
    pop R_IN_CRC_VALUE
    pop R_IN_CRC_SIZE
    pop R_IN_CRC_OFFSET
    pop R_IN_FILE_SIZE
    ret
namespace
#undef R_IN_FILE_SIZE
#undef R_IN_CRC_OFFSET
#undef R_IN_CRC_SIZE
#undef R_IN_CRC_VALUE
#undef R_IN_BINARY_OFFSET
#undef R_IN_BINARY_SIZE
#undef R_IN_BINARY_DATA
#undef R_IN_FILE_PATH
#undef R_STAT_SIZE
#undef R_STAT_ATTR
#undef R_FH
#undef R_CRC_BUF
#undef R_BINARY_BUF
#undef R_STAT_CRC


;//////////////////////////////////////////////////////////
;Name
;	IsLinux
;
;Description
;	Checks if the scan environment is on Linux.  
;	(INTERNAL - called by RemountSystem)
;
;Call
;
;Return
;	r0 = 0 for Not Linux, 1 for Linux
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////

IsLinux:
#define OSVER_UNIX_LINUX    (((4) << 16) | (1))
namespace IsLinux
    CHK_ERR_SWI_FL(SWI_OSVER, $NotLinux)
    cmp r0, OSVER_UNIX_LINUX
    JNZ $NotLinux

    mov 1, r0
;    DEBUG_IMM(" DEBUG: IsLinux: Yes")
    jmp $Exit

$NotLinux:
;    DEBUG_IMM(" DEBUG: IsLinux: No")
    mov 0, r0

$Exit:
    ret
namespace
#undef OSVER_UNIX_LINUX

;//////////////////////////////////////////////////////////
;Name
;	IsAndroid
;
;Description
;	Checks if the scan environment is on Android.  
;	(INTERNAL - called by RemountSystem)
;   SWI_GETENV is supported from MCS 3.1.25
;
;Call
;
;Return
;	r0 = 0 for Not Android, 1 for Android, 2 for Unsupported
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////

IsAndroid:
#define FLATFORM_DATAMEM_LEN    (16)
#define SUPPORTED_ENGVER        (3001)
#define R_FLATFORM_DATAMEM      r10
namespace IsAndroid
    push R_FLATFORM_DATAMEM
    mov 0, R_FLATFORM_DATAMEM

    ; Check EngineVersion (MCS: 3.1 or later, UVE: 3.1 or later)
    CHK_ERR_SWI_FL(SWI_ENGVER, $Unsupported)
;    DEBUG_REG(r0)
;    DEBUG_REG(r1)

;    DEBUG_IMM("Check Engine Version")
    cmp r0, SUPPORTED_ENGVER
    JBU $Unsupported
    cmp r1, SUPPORTED_ENGVER
    JBU $Unsupported

    ; Alloc memory
    mov FLATFORM_DATAMEM_LEN, R_FLATFORM_DATAMEM
    mget R_FLATFORM_DATAMEM, R_FLATFORM_DATAMEM

    ; Get Enviromnet information
;    DEBUG_IMM("Get Enviromnet information")
    lea $PLATFORM_TYPE, r0
    mov R_FLATFORM_DATAMEM, r1
    mov FLATFORM_DATAMEM_LEN, r2

    CHK_ERR_SWI_FL(SWI_GETENV, $NotAndroid)
;    DEBUG_REG(r0)
    cmp r0, 0
    JNZ $Unsupported

    lea $PLATFORM_ANDROID, r0
    mov R_FLATFORM_DATAMEM, r1
    mov 7, r2

;    DEBUG_STR(r1)
    CHK_ERR_SWI_FL(SWI_STR_CMP, $NotAndroid)
    cmp r0, 0
    JNZ $NotAndroid

    mov 1, r0
;    DEBUG_IMM(" DEBUG: IsAndroid: Yes")
    jmp $Exit

$NotAndroid:
    mov 0, r0
;    DEBUG_IMM(" DEBUG: IsAndroid: No")
    jmp $Exit

$Unsupported:
;    DEBUG_IMM(" DEBUG: IsAndroid: Unsupported")
    mov 2, r0

$Exit:
    SYS_MREL_BUF(R_FLATFORM_DATAMEM)

    pop R_FLATFORM_DATAMEM
    ret

$PLATFORM_TYPE:
    bdata("PLATFORM_TYPE", 0)

$PLATFORM_ANDROID:
    bdata("android", 0)
namespace
#undef FLATFORM_DATAMEM
#undef SUPPORTED_ENGVER
#undef R_FLATFORM_DATAMEM

;//////////////////////////////////////////////////////////
;Name
;	IsWithinSystemFolder
;
;Description
;	Checks if the scanned file is in a system folder.  
;	(INTERNAL - called by RemountSystem)
;
;Call
;	r0 = target file path, zero terminated.
;
;Return
;	r0 = 0 for not within system folder, 1 for within system folder
;
;Flags Modified
;	
;//////////////////////////////////////////////////////////

IsWithinSystemFolder:
namespace IsWithinSystemFolder
    lea $SYSTEM_FOLDER_PATH, r1
    mov 8, r2

    CHK_ERR_SWI_FL(SWI_STR_CMP, $NotWithinSystemFolder)
    cmp r0, 0
    JNZ $NotWithinSystemFolder

    mov 1, r0
;    DEBUG_IMM(" DEBUG: IsWithinSystemFolder: Yes")
    jmp $Exit

$NotWithinSystemFolder:
;    DEBUG_IMM(" DEBUG: IsWithinSystemFolder: No")
    mov 0, r0

$Exit:
    ret

$SYSTEM_FOLDER_PATH:
    bdata("/system/", 0)
namespace

;//////////////////////////////////////////////////////////
;Name
;	RemountSystem
;
;Description
;	Remount System partition
;	(INTERNAL - called by RepairInit, RepairExit)
;
;Call 
;	r0 = 0 for readonly mode, 1 for readwrite mode
;	r1 = target file path, zero teminated

;Return
;	r0 = Error code. 0 for success or no need to remount, otherwise error code.
;
;Flags Modified
;	None
;//////////////////////////////////////////////////////////

RemountSystem:
#define R_REMOUNT_MODE    r10
#define R_TARGET_FILE     r11
namespace RemountSystem
    push R_REMOUNT_MODE
    push R_TARGET_FILE
    mov r0, R_REMOUNT_MODE
    mov r1, R_TARGET_FILE

    calln IsLinux
    cmp r0, 0
    JE $Exit

    calln IsAndroid
    cmp r0, 0
    JE $Exit

    mov R_TARGET_FILE, r0
    calln IsWithinSystemFolder
    cmp r0, 0
    JE $Exit

    calln IsRoot
    cmp r0, 0
    JE $Exit

    cmp R_REMOUNT_MODE, 0
    JNZ $Remount_RW

    ; remount in ro
    lea $Mount_CommandLine_RO, r0
;    DEBUG_IMM(" DEBUG: RemountSystem: Read-Only")
;    DEBUG_STR(r0)
    mov 5, r1
    CHK_ERR_SWI_FL(SWI_EXEC, $Exit)

    jmp $Exit

$Remount_RW:
    ; remount in rw
    lea $Mount_CommandLine_RW, r0
;    DEBUG_IMM(" DEBUG: RemountSystem: Read-Write")
;    DEBUG_STR(r0)
    mov 5, r1
    CHK_ERR_SWI_FL(SWI_EXEC, $Exit)

$Exit:
    pop R_TARGET_FILE
    pop R_REMOUNT_MODE
    ret

$Mount_CommandLine_RW:
    bdata("/system/bin/mount -o rw,remount /dev/null /system", 0)

$Mount_CommandLine_RO:
    bdata("/system/bin/mount -o ro,remount /dev/null /system", 0)
namespace
#undef R_REMOUNT_MODE
#undef R_TARGET_FILE

;//////////////////////////////////////////////////////////
;Name
;   StrCat
;
;Description
;   Concatenate 2 strings
;
;Call 
;   r0 = Address of first string (\0 terminalted)
;   r1 = Address of second string (\0 terminalted)
;
;Return
;   r0 = Address of concatenated string (\0 terminalted)
;   r1 = Length of concreated string (including '\0')
;
;Flags Modified
;   None
;//////////////////////////////////////////////////////////

StrCat:
#define R_STR1 r10
#define R_STR2 r11
#define R_STR3 r12
#define R_STR1_LEN r13
#define R_STR2_LEN r14
#define R_STR3_LEN r15
namespace StrCat
    push R_STR1
    push R_STR2
    push R_STR3
    push R_STR1_LEN
    push R_STR2_LEN
    push R_STR3_LEN

    mov r0, R_STR1
    mov r1, R_STR2
    mov 0, R_STR3

    ; Get length of first string without '\0' terminated
    mov R_STR1, r0
    CHK_ERR_SWI_FL(SWI_STR_LEN, $Error)
    mov r0,  R_STR1_LEN

    ; Get length of second string with '\0' terminated
    mov R_STR2, r0
    CHK_ERR_SWI_FL(SWI_STR_LEN, $Error)
    mov r0,  R_STR2_LEN
    inc R_STR2_LEN, R_STR2_LEN

    ; Allocate memory
    add R_STR1_LEN, R_STR2_LEN, R_STR3_LEN
    mget R_STR3_LEN, R_STR3
    strb 0, R_STR3

    ; Copy first string.
    mov R_STR3, r0
    mov R_STR1, r1
    mov -1, r2
    CHK_ERR_SWI_FL(SWI_STR_CPY, $Error)

    ; Copy second string with '\0'
    add R_STR3, R_STR1_LEN, r0
    mov R_STR2, r1
    mov -1, r2
    CHK_ERR_SWI_FL(SWI_STR_CPY, $Error)

    ; Set result
    mov R_STR3, r0
    mov R_STR3_LEN, r1
    jmp $Exit

$Error:
    SYS_MREL_BUF(R_STR3)
    mov 0, r0
    mov 0, r1

$Exit:
    pop R_STR3_LEN
    pop R_STR2_LEN
    pop R_STR1_LEN
    pop R_STR3
    pop R_STR2
    pop R_STR1
    ret
namespace
#undef R_STR1
#undef R_STR2
#undef R_STR3
#undef R_STR1_LEN
#undef R_STR2_LEN
#undef R_STR3_LEN

;//////////////////////////////////////////////////////////
;Name
;   IsRoot
;
;Description
;   Check if root privilege or not
;	(INTERNAL - called by RemountSystem)
;
;Call
;
;Return
;	r0 = 0 for Not Root, 1 for Root
;
;Flags Modified
;	None
;//////////////////////////////////////////////////////////

IsRoot:
#define R_SHELLSCRIPT_PATH r10
#define R_SHELLOUTPUT_PATH r11
#define R_SHELLSCRIPT_PATH_LEN r12
#define R_SHELLOUTPUT_PATH_LEN r13
#define R_SHELLCOMMAND_SH r14
#define R_SHELLCOMMAND_ID r15
#define R_SHELLCOMMAND_CHMOD r16
#define R_SHELLCOMMAND_SH_LEN r17
#define R_SHELLCOMMAND_ID_LEN r18
#define R_SHELLCOMMAND_CHMOD_LEN r19
#define R_FILEHANDLE r20
#define R_BUF r21
#define R_BUF_LEN r22
#define BUF_SIZE (256)
namespace IsRoot
    push FL
    push R_SHELLSCRIPT_PATH
    push R_SHELLOUTPUT_PATH
    push R_SHELLSCRIPT_PATH_LEN
    push R_SHELLOUTPUT_PATH_LEN
    push R_SHELLCOMMAND_SH
    push R_SHELLCOMMAND_ID
    push R_SHELLCOMMAND_CHMOD
    push R_SHELLCOMMAND_SH_LEN
    push R_SHELLCOMMAND_ID_LEN
    push R_SHELLCOMMAND_CHMOD_LEN
    push R_FILEHANDLE
    push R_BUF
    push R_BUF_LEN

    ; Clear mget/mrel register.
    mov 0, R_SHELLSCRIPT_PATH
    mov 0, R_SHELLOUTPUT_PATH
    mov 0, R_SHELLCOMMAND_SH
    mov 0, R_SHELLCOMMAND_ID
    mov 0, R_SHELLCOMMAND_CHMOD
    mov 0, R_BUF

    ; Get shell script filename by temporary filename.
    mov 0, r0
    CHK_ERR_SWI_FL(SWI_TMPFILE, $NotRoot)
    mov r0, R_SHELLSCRIPT_PATH

    CHK_ERR_SWI_FL(SWI_STR_LEN, $NotRoot)
    mov r0, R_SHELLSCRIPT_PATH_LEN
    inc R_SHELLSCRIPT_PATH_LEN, R_SHELLSCRIPT_PATH_LEN

    ; Get redirect output filename by temporary filename.
    mov 0, r0
    CHK_ERR_SWI_FL(SWI_TMPFILE, $NotRoot)
    mov r0, R_SHELLOUTPUT_PATH

    CHK_ERR_SWI_FL(SWI_STR_LEN, $NotRoot)
    mov r0, R_SHELLOUTPUT_PATH_LEN
    inc R_SHELLOUTPUT_PATH_LEN, R_SHELLOUTPUT_PATH_LEN

    ; Make command strings "/system/bin/id > filename2"
    lea $CommandLine_id, r0
    mov R_SHELLOUTPUT_PATH, r1
    calln StrCat
    cmp r0, 0
    JZ $NotRoot
    mov r0, R_SHELLCOMMAND_ID
    mov r1, R_SHELLCOMMAND_ID_LEN

    ; Make command string "/system/bin/sh filename1"
    lea $CommandLine_sh, r0
    mov R_SHELLSCRIPT_PATH, r1
    calln StrCat
    cmp r0, 0
    JZ $NotRoot
    mov r0, R_SHELLCOMMAND_SH
    mov r1, R_SHELLCOMMAND_SH_LEN

    ; Make command string "/system/bin/chmod 600 filename1"
    lea $CommandLine_chmod, r0
    mov R_SHELLSCRIPT_PATH, r1
    calln StrCat
    cmp r0, 0
    JZ $NotRoot
    mov r0, R_SHELLCOMMAND_CHMOD
    mov r1, R_SHELLCOMMAND_CHMOD_LEN

    ; Create shell script file with 0 byte.
    mov R_SHELLSCRIPT_PATH, r0
    mov (F_FOPEN_CREATE|F_FOPEN_WRITE), r1
    CHK_ERR_SWI_FLR0(SWI_FOPEN, $NotRoot)
    mov r1, R_FILEHANDLE

    mov R_FILEHANDLE, r0
    CHK_ERR_SWI_FLR0(SWI_FCLOSE, $NotRoot)
    mov 0, R_FILEHANDLE

    mov R_SHELLSCRIPT_PATH, r0
    CHK_ERR_SWI_FLR0(SWI_FILESTAT, $NotRoot)
    cmp r2, 0
    JNZ $NotRoot

    ; Change permission "-rw------- : 600".
    mov R_SHELLCOMMAND_CHMOD, r0
    mov 5, r1
    CHK_ERR_SWI_FL(SWI_EXEC, $NotRoot)
;    DEBUG_STR(R_SHELLCOMMAND_CHMOD)

    ; Write shell script in filename1
    mov R_SHELLSCRIPT_PATH, r0
    mov F_FOPEN_WRITE, r1
    CHK_ERR_SWI_FLR0(SWI_FOPEN, $NotRoot)
    mov r1, R_FILEHANDLE

    mov R_FILEHANDLE, r0
    mov R_SHELLCOMMAND_ID, r1
    mov R_SHELLCOMMAND_ID_LEN, r2
    CHK_ERR_SWI_FLR0(SWI_FWRITE, $NotRoot)

    mov R_FILEHANDLE, r0
    du2d R_SHELLCOMMAND_ID_LEN, x0
    CHK_ERR_SWI_FLR0(SWI_FTRUNCATE, $NotRoot)

    mov R_FILEHANDLE, r0
    CHK_ERR_SWI_FLR0(SWI_FCLOSE, $NotRoot)
    mov 0, R_FILEHANDLE

    ; Run shell script file
    mov R_SHELLCOMMAND_SH, r0
    mov 5, r1
    CHK_ERR_SWI_FL(SWI_EXEC, $NotRoot)
;    DEBUG_STR(R_SHELLCOMMAND_SH)

    ; Check existance of output file
    mov R_SHELLOUTPUT_PATH, r0
    CHK_ERR_SWI_FLR0(SWI_FILESTAT, $NotRoot)
    cmp r2, 0
    JNZ $NotRoot

    ; Allocate memory
    mov BUF_SIZE, R_BUF_LEN
    mget R_BUF_LEN, R_BUF

    ; Read output file.
    mov R_SHELLOUTPUT_PATH, r0
    mov F_FOPEN_READ, r1
    CHK_ERR_SWI_FLR0(SWI_FOPEN, $NotRoot)
    mov r1, R_FILEHANDLE

    mov R_FILEHANDLE, r0
    mov R_BUF, r1
    mov R_BUF_LEN, r2
    CHK_ERR_SWI_FLR0(SWI_FREAD, $NotRoot)

    mov R_FILEHANDLE, r0
    CHK_ERR_SWI_FLR0(SWI_FCLOSE, $NotRoot)

    ; Check Root Privilege.
    ; If Root, "uid=0(root)" and "gid=0(root)" are included in the result.
    xregxfind R_BUF, R_BUF_LEN, --, (/uid=0\\\(root\\\)/)
    test FL, UVE_FL_VIRUSFOUND
    JZ $NotRoot

    and FL, ~UVE_FL_VIRUSFOUND, FL
    xregxfind R_BUF, R_BUF_LEN, --, (/gid=0\\\(root\\\)/)
    test FL, UVE_FL_VIRUSFOUND
    JZ $NotRoot

    and FL, ~UVE_FL_VIRUSFOUND, FL
;    DEBUG_IMM(" DEBUG: IsRoot: Yes")
    mov 1, r0
    jmp $Exit

$NotRoot:
;    DEBUG_IMM(" DEBUG: IsRoot: No")
    mov 0, r0

$Exit:
    ; Remove temporary files
    push r0
    SYS_RM_BUF(R_SHELLSCRIPT_PATH, 0)
    SYS_RM_BUF(R_SHELLOUTPUT_PATH, 0)
    pop r0

    ; Release alloc memory
    SYS_MREL_BUF(R_SHELLSCRIPT_PATH)
    SYS_MREL_BUF(R_SHELLOUTPUT_PATH)
    SYS_MREL_BUF(R_SHELLCOMMAND_SH)
    SYS_MREL_BUF(R_SHELLCOMMAND_ID)
    SYS_MREL_BUF(R_SHELLCOMMAND_CHMOD)
    SYS_MREL_BUF(R_BUF)

    pop R_BUF_LEN
    pop R_BUF
    pop R_FILEHANDLE
    pop R_SHELLCOMMAND_CHMOD_LEN
    pop R_SHELLCOMMAND_ID_LEN
    pop R_SHELLCOMMAND_SH_LEN
    pop R_SHELLCOMMAND_CHMOD
    pop R_SHELLCOMMAND_ID
    pop R_SHELLCOMMAND_SH
    pop R_SHELLOUTPUT_PATH_LEN
    pop R_SHELLSCRIPT_PATH_LEN
    pop R_SHELLOUTPUT_PATH
    pop R_SHELLSCRIPT_PATH
    pop FL
    ret

$CommandLine_id:
    bdata("/system/bin/id > ", 0)

$CommandLine_sh:
    bdata("/system/bin/sh ", 0)

$CommandLine_chmod:
    bdata("/system/bin/chmod 600 ", 0)
namespace
#undef R_SHELLSCRIPT_PATH
#undef R_SHELLOUTPUT_PATH
#undef R_SHELLSCRIPT_PATH_LEN
#undef R_SHELLOUTPUT_PATH_LEN
#undef R_SHELLCOMMAND_SH
#undef R_SHELLCOMMAND_ID
#undef R_SHELLCOMMAND_CHMOD
#undef R_SHELLCOMMAND_SH_LEN
#undef R_SHELLCOMMAND_ID_LEN
#undef R_SHELLCOMMAND_CHMOD_LEN
#undef R_FILEHANDLE
#undef R_BUF
#undef R_BUF_LEN
#undef BUFFER_SIZE


;//////////////////////////////////////////////////////////
;Name
;    BulkProcess
;
;Description
;    commonalize instructions for Android Bulk signaturese
;
;Call
;    r0 = Memory for removal target information:
;
;    -----------------------------------------------
;    Ofs Siz Content
;    -----------------------------------------------
;     0   4 file size
;     4   4 1st CRC Check - Offset
;     8   4 1st CRC Check - Size
;    12   4 1st CRC Check - CRC value
;    16   4 2nd CRC Check - Offset
;    20   4 2nd CRC Check - Size
;    24   4 2nd CRC Check - CRC value
;    28   x variant name, zero terminated
;    -----------------------------------------------
;
;Return
;    none
;
;Flags Modified
;    FL (UVE_FL_VIRUSFOUND)
;//////////////////////////////////////////////////////////
BulkProcess:
#define R_IN_FILE_SIZE         r10
#define R_IN_1ST_CRC_OFFSET    r11
#define R_IN_1ST_CRC_SIZE      r12
#define R_IN_1ST_CRC_VALUE     r13
#define R_IN_2ND_CRC_OFFSET    r14
#define R_IN_2ND_CRC_SIZE      r15
#define R_IN_2ND_CRC_VALUE     r16
#define R_IN_VARIANT_STR       r17
#define R_1ST_BUF              r18
#define R_2ND_BUF              r19
#define R_1ST_CALC_CRC         r20
#define R_2ND_CALC_CRC         r21
namespace BulkProcess
    push R_IN_FILE_SIZE
    push R_IN_1ST_CRC_OFFSET
    push R_IN_1ST_CRC_SIZE
    push R_IN_1ST_CRC_VALUE
    push R_IN_2ND_CRC_OFFSET
    push R_IN_2ND_CRC_SIZE
    push R_IN_2ND_CRC_VALUE
    push R_IN_VARIANT_STR
    push R_1ST_BUF
    push R_2ND_BUF
    push R_1ST_CALC_CRC
    push R_2ND_CALC_CRC

    ; Initialize.
    mov 0, R_1ST_BUF
    mov 0, R_2ND_BUF

    ; Get params.
;    DEBUG_IMM("Get params")
    ldrw r0, R_IN_FILE_SIZE
    ldrw r0[4], R_IN_1ST_CRC_OFFSET
    ldrw r0[8], R_IN_1ST_CRC_SIZE
    ldrw r0[12], R_IN_1ST_CRC_VALUE
    ldrw r0[16], R_IN_2ND_CRC_OFFSET
    ldrw r0[20], R_IN_2ND_CRC_SIZE
    ldrw r0[24], R_IN_2ND_CRC_VALUE
    add r0, 28, R_IN_VARIANT_STR
;    DEBUG_REG(R_IN_FILE_SIZE)
;    DEBUG_REG(R_IN_1ST_CRC_OFFSET)
;    DEBUG_REG(R_IN_1ST_CRC_SIZE)
;    DEBUG_REG(R_IN_1ST_CRC_VALUE)
;    DEBUG_REG(R_IN_2ND_CRC_OFFSET)
;    DEBUG_REG(R_IN_2ND_CRC_SIZE)
;    DEBUG_REG(R_IN_2ND_CRC_VALUE)
;    DEBUG_STR(R_IN_VARIANT_STR)

    ; Check Data Size
;    DEBUG_IMM("Check Data Size")
;    DEBUG_REG(DS)
    cmp DS, R_IN_FILE_SIZE
    JNE $Quit

    ; Check 1st CRC Size.
;    DEBUG_IMM("Check 1st CRC Size")
    cmp R_IN_1ST_CRC_SIZE, 0
    JZ $SKIP_1ST_CRC_CHECK

    ; Check 1st CRC Offset.
;    DEBUG_IMM("Check 1st CRC Offset")
    cmp DS, R_IN_1ST_CRC_OFFSET
    JBU $Quit

    ; Check 1st CRC - Allocate memory.
;    DEBUG_IMM("Check 1st CRC - Allocate memory")
    mget R_IN_1ST_CRC_SIZE, R_1ST_BUF

    ; Check 1st CRC - Read data
;    DEBUG_IMM("Check 1st CRC - Read data")
    mov R_IN_1ST_CRC_OFFSET, DP
    read R_IN_1ST_CRC_SIZE, R_1ST_BUF, r0
    cmp r0, R_IN_1ST_CRC_SIZE
    JNE $Quit

    ; Check 1st CRC - Calc CRC.
;    DEBUG_IMM("Check 1st CRC - Calc CRC")
    mov 0, r0
    mcsum r0, R_1ST_BUF, R_IN_1ST_CRC_SIZE, R_1ST_CALC_CRC
;    DEBUG_REG(R_1ST_CALC_CRC)

    ; Check 1st CRC - Compare CRC.
;    DEBUG_IMM("Check 1st CRC - Compare CRC")
    cmp R_1ST_CALC_CRC, R_IN_1ST_CRC_VALUE
    JNE $Quit

$SKIP_1ST_CRC_CHECK:

    ; Check 2nd CRC Size.
;    DEBUG_IMM("Check 2nd CRC Size")
    cmp R_IN_2ND_CRC_SIZE, 0
    JZ $SKIP_2ND_CRC_CHECK

    ; Check 2nd CRC Offset.
;    DEBUG_IMM("Check 2nd CRC Offset")
    cmp DS, R_IN_2ND_CRC_OFFSET
    JBU $Quit

    ; Check 2nd CRC - Allocate memory.
;    DEBUG_IMM("Check 2nd CRC - Allocate memory")
    mget R_IN_2ND_CRC_SIZE, R_2ND_BUF

    ; Check 2nd CRC - Read data
;    DEBUG_IMM("Check 2nd CRC - Read data")
    mov R_IN_2ND_CRC_OFFSET, DP
    read R_IN_2ND_CRC_SIZE, R_2ND_BUF, r0
    cmp r0, R_IN_2ND_CRC_SIZE
    JNE $Quit

    ; Check 2nd CRC - Calc CRC.
;    DEBUG_IMM("Check 2nd CRC - Calc CRC")
    mov 0, r0
    mcsum r0, R_2ND_BUF, R_IN_2ND_CRC_SIZE, R_2ND_CALC_CRC
;    DEBUG_REG(R_2ND_CALC_CRC)

    ; Check 2nd CRC - Compare CRC.
;    DEBUG_IMM("Check 2nd CRC - Compare CRC")
    cmp R_2ND_CALC_CRC, R_IN_2ND_CRC_VALUE
    JNE $Quit

$SKIP_2ND_CRC_CHECK:

    ; Add variant name.
;    DEBUG_IMM("Add variant name")
    mov R_IN_VARIANT_STR, r0
    CHK_ERR_SWI_FL(SWI_SETVARIANT, $Quit)

    ; Add VIRUSFOUND Flag.
;    DEBUG_REG(FL)
;    DEBUG_IMM("Add VIRUSFOUND Flag")
    or FL, UVE_FL_VIRUSFOUND, FL
;    DEBUG_REG(FL)

    ; Check Clean Option.
;    DEBUG_IMM("Check Clean Option")
    test FL, UVE_FL_CLEAN
    JZ $Quit

    ; CleanScannedFile.
;    DEBUG_IMM("Call CleanScannedFile")
    calln CleanScannedFile

$Quit:
    SYS_MREL_BUF(R_1ST_BUF)
    SYS_MREL_BUF(R_2ND_BUF)

;    DEBUG_IMM("Error code:")
;    DEBUG_REG(r0)

    pop R_2ND_CALC_CRC
    pop R_1ST_CALC_CRC
    pop R_2ND_BUF
    pop R_1ST_BUF
    pop R_IN_VARIANT_STR
    pop R_IN_2ND_CRC_VALUE
    pop R_IN_2ND_CRC_SIZE
    pop R_IN_2ND_CRC_OFFSET
    pop R_IN_1ST_CRC_VALUE
    pop R_IN_1ST_CRC_SIZE
    pop R_IN_1ST_CRC_OFFSET
    pop R_IN_FILE_SIZE
    ret
namespace
#undef R_IN_FILE_SIZE
#undef R_IN_1ST_CRC_OFFSET
#undef R_IN_1ST_CRC_SIZE
#undef R_IN_1ST_CRC_VALUE
#undef R_IN_2ND_CRC_OFFSET
#undef R_IN_2ND_CRC_SIZE
#undef R_IN_2ND_CRC_VALUE
#undef R_IN_VARIANT_STR
#undef R_1ST_BUF
#undef R_2ND_BUF
#undef R_1ST_CALC_CRC
#undef R_2ND_CALC_CRC


;//////////////////////////////////////////////////////////
;Name
;    BulkFindAndExit
;//////////////////////////////////////////////////////////
BulkFindAndExit:
namespace BulkFindAndExit

    ; Add VIRUSFOUND Flag.
    or FL, UVE_FL_VIRUSFOUND, FL

    ; Check Clean Option.
    test FL, UVE_FL_CLEAN
    JZ $Exit

    ; CleanScannedFile.
    calln CleanScannedFile

$Exit:
    exit
namespace


;//////////////////////////////////////////////////////////
;Name
;  IsElf
;
;Description
;  Check if Elf file or not
;
;Return
;  r0 = 1 is Elf, 0 is not Elf
;
;Flags Modified
;  None
;//////////////////////////////////////////////////////////
IsElf:
#define ELF_MAGIC_NUMBER_SIZE 4
#define ELF_MAGIC_NUMBER_STR 464c457fh
#define R_TMP r10
namespace IsElf
    push DP
    push R_TMP

    mov 0, DP

    ; check DS.
    cmp DS, ELF_MAGIC_NUMBER_SIZE
    JBS $No

    ; check magic number.
    inw.le R_TMP
;    DEBUG_REG(R_TMP)
    cmp R_TMP, ELF_MAGIC_NUMBER_STR
    JNE $No

$Yes:
;    DEBUG_IMM("IsElf: Yes")
    mov 1, r0
    jmp $Exit

$No:
;    DEBUG_IMM("IsElf: No")
    mov 0, r0

$Exit:
    pop R_TMP
    pop DP
    ret
namespace
#undef ELF_MAGIC_NUMBER_SIZE
#undef ELF_MAGIC_NUMBER_STR
#undef R_TMP


;//////////////////////////////////////////////////////////
;Name
;  GetDexStringRange
;
;Description
;  Get string range from Dex file
;
;Call
; none
;
;Return
;  r0: result
;  r1: start position
;  r2: range byte
;
;Flags Modified
;  none
;//////////////////////////////////////////////////////////
GetDexStringRange:
#define DEX_STRING_IDS_SIZE 38h
#define MAX_IDS_SIZE 100000
#define R_STRING_IDS_SIZE r10
#define R_STRING_IDS_PTR r11
#define R_TMP r12
#define R_OUT_RET r13
#define R_OUT_SMALL r14
#define R_OUT_BIG r15
#define HEAP_IDS_SIZE 10000
#define HEAP_IDS_BYTE 40000
#define R_HEAP r16
#define R_HEAP_PTR r17
#define R_READ_IDS_NUM r18
#define R_READ_IDS_BYTE r19
#define R_STRING_PTR r20
#define R_OUT_RANGE r21
namespace GetDexStringRange
    push DP
    push R_STRING_IDS_SIZE
    push R_STRING_IDS_PTR
    push R_TMP
    push R_OUT_RET
    push R_OUT_SMALL
    push R_OUT_BIG
    push R_HEAP
    push R_HEAP_PTR
    push R_READ_IDS_NUM
    push R_READ_IDS_BYTE
    push R_STRING_PTR
    push R_OUT_RANGE

    ; --- Initialization --- 
    mov -1, R_OUT_RET
    mov DS, R_OUT_SMALL
    SET_ZERO(R_OUT_BIG)
    SET_ZERO(R_HEAP)

    ; --- Check Property --- 
    SET_ZERO(r0)
    lea $PROPERTY_NAME_DEX_STRING_POS, r1
    swi UVE_SWI_GETPROP
    test r0, r0
    JNZ $NotInProperty
    test r1, r1
    JZ $NotInProperty
    cmp r2, 8
    JZ $NotInProperty

    SET_ZERO(R_OUT_RET)
    ldrw r1, R_OUT_SMALL
    ldrw r1[4], R_OUT_RANGE
    mrel r1
    jmp $Exit

$NotInProperty:
    ; --- Read String offset from Dex header --- 
    mov DEX_STRING_IDS_SIZE, DP
    inw.le R_STRING_IDS_SIZE
    inw.le R_STRING_IDS_PTR

    ; if (R_STRING_IDS_SIZE == 0) then Exit
    test R_STRING_IDS_SIZE, R_STRING_IDS_SIZE
    JZ $Exit

    ; --- Maximum size for infinity roop --- 
    ; if (MAX_IDS_SIZE < R_STRING_IDS_SIZE) then R_STRING_IDS_SIZE = MAX_IDS_SIZE
    SELECT_SMALL_2(R_STRING_IDS_SIZE, MAX_IDS_SIZE)

    ; --- Allocate heap memory for IDS
    mov HEAP_IDS_BYTE, R_TMP
    mget R_TMP, R_HEAP
    test R_HEAP, R_HEAP
    JZ $Exit

$HeadOfHeapRoop:
    ; if (R_STRING_IDS_SIZE == 0) then Exit
    test R_STRING_IDS_SIZE, R_STRING_IDS_SIZE
    JZ $Success

    ; R_READ_IDS_NUM <= select small one which R_STRING_IDS_SIZE or HEAP_IDS_SIZE
    SELECT_SMALL_3(R_READ_IDS_NUM, R_STRING_IDS_SIZE, HEAP_IDS_SIZE)

    ; Calculate IDS byte size
    shl R_READ_IDS_NUM, 2, R_READ_IDS_BYTE

    ; --- read String IDS into heap memory --- 
    mov R_STRING_IDS_PTR, DP
    read R_READ_IDS_BYTE, R_HEAP, R_TMP

    ; if (R_READ_IDS_BYTE != R_TMP) then Exit
    cmp R_READ_IDS_BYTE, R_TMP
    JNE $Exit

    ; --- for next heap --- 
    sub R_STRING_IDS_SIZE, R_READ_IDS_NUM, R_STRING_IDS_SIZE
    add R_STRING_IDS_PTR, R_READ_IDS_BYTE, R_STRING_IDS_PTR

    mov R_HEAP, R_HEAP_PTR

$HeadOfStringSearch:
    ; if (R_READ_IDS_NUM == 0) then Finish
    test R_READ_IDS_NUM, R_READ_IDS_NUM
    JZ $HeadOfHeapRoop

    ; --- Get string offset --- 
    ldrw R_HEAP_PTR, R_STRING_PTR

    ; --- Get String Offset --- 
    SELECT_SMALL_2(R_OUT_SMALL, R_STRING_PTR)
    SELECT_BIG_2(R_OUT_BIG, R_STRING_PTR)

    ; --- for next string --- 
    add R_HEAP_PTR, 4, R_HEAP_PTR
    dec R_READ_IDS_NUM, R_READ_IDS_NUM

    jmp $HeadOfStringSearch

$Success:
    SET_ZERO(R_OUT_RET)


    ; --- Calculate range size --- 
    SELECT_SMALL_2(R_OUT_BIG, DS)
    sub R_OUT_BIG, R_OUT_SMALL, R_OUT_RANGE

    ; --- Set Property --- 
    SET_ZERO(r0)
    lea $PROPERTY_NAME_DEX_STRING_POS, r1
    lea $PROPERTY_NAME_DEX_STRING_DATA, r2
    strw R_OUT_SMALL, r2
    strw R_OUT_RANGE, r2[4]
    mov 8, r3
    swi UVE_SWI_SETPROP

$Exit:
    mov R_OUT_RET, r0
    mov R_OUT_SMALL, r1
    mov R_OUT_RANGE, r2

    pop R_OUT_RANGE
    pop R_STRING_PTR
    pop R_READ_IDS_BYTE
    pop R_READ_IDS_NUM
    pop R_HEAP_PTR
    pop R_HEAP
    pop R_OUT_BIG
    pop R_OUT_SMALL
    pop R_OUT_RET
    pop R_TMP
    pop R_STRING_IDS_PTR
    pop R_STRING_IDS_SIZE
    pop DP
    ret
$PROPERTY_NAME_DEX_STRING_POS:
    bdata("DEX_STRING_POS", 0)
$PROPERTY_NAME_DEX_STRING_DATA:
    wdata(0, 0)
namespace
#undef DEX_STRING_IDS_SIZE
#undef MAX_IDS_SIZE
#undef R_STRING_IDS_SIZE
#undef R_STRING_IDS_PTR
#undef R_TMP
#undef R_OUT_RET
#undef R_OUT_SMALL
#undef R_OUT_BIG
#undef HEAP_IDS_SIZE
#undef HEAP_IDS_BYTE
#undef R_HEAP
#undef R_HEAP_PTR
#undef R_READ_IDS_NUM
#undef R_READ_IDS_BYTE
#undef R_STRING_PTR
#undef R_OUT_RANGE
