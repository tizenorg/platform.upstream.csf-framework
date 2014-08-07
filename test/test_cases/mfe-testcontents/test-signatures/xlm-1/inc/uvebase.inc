#if !defined(UVEBASE_INC)
#define UVEBASE_INC


#define UVE_FL_VIRUSFOUND (1 << 0)
#define UVE_FL_STOP (1 << 1)
#define UVE_FL_OPOK (1 << 2)
#define UVE_FL_Z (1 << 3)
#define UVE_FL_C (1 << 4)
#define UVE_FL_N (1 << 5)
#define UVE_FL_V (1 << 6)
#define UVE_FL_CLEAN (1 << 7)

#define K_BIT (1 << (1 + 8))

; uVirtran SWI definition
#define UVE_SWI_DUMP_CTX       1
#define UVE_SWI_PRINT_REG      2
#define UVE_SWI_PRINT_STR      3
#define UVE_SWI_MEMCPY        10
#define UVE_SWI_FS_DELETE     11
#define UVE_SWI_FS_MOVE       12
#define UVE_SWI_PROCESS_KILL  13
#define UVE_SWI_REGKEY_DELETE 14
#define UVE_SWI_EXEC_BIN      15
#define UVE_SWI_URL_GET       16
#define UVE_SWI_VFILE_PATH    17
#define UVE_SWI_MEMCHR        18
#define UVE_SWI_MEMCMP        19
#define UVE_SWI_STRCHR        20
#define UVE_SWI_STRLEN        21
#define UVE_SWI_STRNCPY       22
#define UVE_SWI_STRNCMP       23
#define UVE_SWI_STRNCASECMP   24
#define UVE_SWI_CLOSECURFILE  25
#define UVE_SWI_OSVER         26
#define UVE_SWI_HDBMATCH      27
#define UVE_SWI_CONT_EXEC_BIN 28
#define UVE_SWI_STRRCHR       29
#define UVE_SWI_FILESTAT      30
#define UVE_SWI_READFILE      31
#define UVE_SWI_OPENDIR       32
#define UVE_SWI_CLOSEDIR      33
#define UVE_SWI_READDIR       34
#define UVE_SWI_ENGVER        35
#define UVE_SWI_FOPEN         36
#define UVE_SWI_FCLOSE        37
#define UVE_SWI_FSEEK         38
#define UVE_SWI_FREAD         39
#define UVE_SWI_FWRITE        40
#define UVE_SWI_FTRUNCATE     41
#define UVE_SWI_XSHA1         42
#define UVE_SWI_MEMSET        43
#define UVE_SWI_RXMATCH       44
#define UVE_SWI_MEMRCHR       45
#define UVE_SWI_SETVARIANT    46
#define UVE_SWI_STRSTR        47
#define UVE_SWI_SETPROP       48
#define UVE_SWI_GETPROP       49
#define UVE_SWI_GETDTYPE      50
#define UVE_SWI_TMPFILE       51
#define UVE_SWI_GETENV        52

; legacy SWI definition
#define SWI_DUMP_CTX    1
#define SWI_PRINT_REG   2
#define SWI_PRINT_STR   3
#define SWI_RM          11
#define SWI_MV          12
#define SWI_KILL        13
#define SWI_RMKEY       14
#define SWI_EXEC        15
#define SWI_URLGET      16
#define SWI_VFILE_PATH  17
#define SWI_MEMCMP      19
#define SWI_STR_LEN     21
#define SWI_STR_CPY     22
#define SWI_STR_CMP     23
#define SWI_OSVER       26
#define SWI_FILESTAT    30
#define SWI_ENGVER      35
#define SWI_FOPEN       36
#define SWI_FCLOSE      37
#define SWI_FSEEK       38
#define SWI_FREAD       39
#define SWI_FWRITE      40
#define SWI_FTRUNCATE   41
#define SWI_SETVARIANT  46
#define SWI_TMPFILE     51
#define SWI_GETENV      52

#define MCS_VTYPE_MALWARE    1
#define MCS_VTYPE_SPAM       2
#define MCS_VTYPE_PUP        3
#define MCS_VTYPE_PHISHING   4
#define MCS_VTYPE_VIRUS      5
#define MCS_VTYPE_TROJAN     6
#define MCS_VTYPE_EXPLOIT    7
#define MCS_VTYPE_SUSPICIOUS 8

#define F_FOPEN_READ   (1 << 0)
#define F_FOPEN_WRITE  (1 << 1)
#define F_FOPEN_CREATE (1 << 2)
#define F_FOPEN_TMP    (1 << 3)
#define F_FOPEN_EXCL   (1 << 4)

#define F_FSEEK_SET    0
#define F_FSEEK_CUR    1
#define F_FSEEK_END    2

#define UVE_QSRC_CASEIGNORE (1 << 0)
#define UVE_QSRC_SETOFFSET (1 << 1)

#define DUMP_CTX() \
    swi SWI_DUMP_CTX

#define PRINT_REG() \
    swi SWI_PRINT_REG

#define PRINT_STR() \
    swi SWI_PRINT_STR

#define DEBUG_REG(r) \
    push r0 \
    mov r, r0 \
    swi SWI_PRINT_REG \
    pop r0

#define DEBUG_STR(r) \
    push r0 \
    mov r, r0 \
    swi SWI_PRINT_STR \
    pop r0

#define DEBUG_IMM(s) \
    namespace @ \
        push r0 \
        lea $Label, r0 \
        swi SWI_PRINT_STR \
        jmp $Exit \
    $Label: \
        bdata(s, 0) \
    $Exit: \
        pop r0 \
    namespace

#define SYS_KILL(p) \
    lea p, r0 \
    swi SWI_KILL

#define SYS_MV(o, n) \
    lea o, r0 \
    lea n, r1 \
    swi SWI_MV

#define SYS_RM(p, r) \
    lea p, r0 \
    mov r, r1 \
    swi SWI_RM

#define SYS_RMKEY(p, r) \
    lea p, r0 \
    mov r, r1 \
    swi SWI_RMKEY

#define SYS_EXEC(p, w) \
    lea p, r0 \
    mov w, r1 \
    swi SWI_EXEC

#define SYS_URLGET(u, f) \
    lea u, r0 \
    lea f, r1 \
    swi SWI_URLGET

#define SYS_MREL_BUF(r) \
    namespace @ \
        cmp r, 0 \
        JZ $Exit \
        mrel r \
    $Exit: \
        mov 0, r \
    namespace

#define SYS_FCLOSE_FH(r) \
    namespace @ \
        cmp r, 0 \
        JZ $Exit \
        mov r, r0 \
        swi SWI_FCLOSE \
        mov 0, r \
    $Exit: \
    namespace

#define PCHECK pcheck -c,
#define IPCHECK pcheck -i,
#define CHECK check -c,
#define ICHECK check -i,

#define JE jcond -z,
#define JZ jcond -z,
#define JNE jcond -!z,
#define JNZ jcond -!z,

#define JAES jcond -!n,
#define JAS jcond -!nz,
#define JBS jcond -n,
#define JBES jcond -nz,

#define JAEU jcond -!c,
#define JAU jcond -!cz,
#define JBU jcond -c,
#define JBEU jcond -cz,

;; Stuff added by Andy.  Unofficial.

#define SCLASS_USER          1
#define SCLASS_TERMINAL      2
#define SCLASS_USER_TERMINAL 3

#define BLEVEL_WARN             1
#define BLEVEL_ASK              2
#define BLEVEL_REJECT_STORE     3
#define BLEVEL_REJECT_ASK_STORE 4
#define BLEVEL_REJECT_NOSTORE   5

#define DTYPE_HTML        1
#define DTYPE_URL         2
#define DTYPE_EMAIL       3
#define DTYPE_PHONE       4
#define DTYPE_JAVA_APPLET 5
#define DTYPE_JAVA_SCRIPT 6
#define DTYPE_TEXT        7

#define DTYPE_ALL 0,1,2,3,4,5,6,7

;; delete error checking
#define UVE_ERR_FS_DELETE 1000

; macro to check for failure to delete (when file exists)
; the virusfound flag will be set if errors occur
#define CHK_ERR_DEL(r, a) \
    cmp r, UVE_ERR_FS_DELETE \
    jcond -!z, a \
    or FL, UVE_FL_VIRUSFOUND, FL

; macro to check for failure to execute swi (when swi is not supported)
#define CHK_ERR_SWI_FL(s, a) \
    and FL, ~UVE_FL_OPOK, FL \
    swi s \
    test FL, UVE_FL_OPOK \
    JZ a

#define CHK_ERR_SWI_FLR0(s, a) \
    and FL, ~UVE_FL_OPOK, FL \
    swi s \
    test FL, UVE_FL_OPOK \
    JZ a \
    cmp r0, 0 \
    JNZ a

#define SELECT_BIG_2(ra, rb) \
    namespace @ \
        cmp ra, rb \
        JAU $NoChange \
        mov rb, ra \
    $NoChange: \
    namespace

#define SELECT_BIG_3(r, ra, rb) \
    namespace @ \
        mov ra, r \
        cmp ra, rb \
        JAU $NoChange \
        mov rb, r \
    $NoChange: \
    namespace

#define SELECT_SMALL_2(ra, rb) \
    namespace @ \
        cmp ra, rb \
        JBU $NoChange \
        mov rb, ra \
    $NoChange: \
    namespace

#define SELECT_SMALL_3(r, ra, rb) \
    namespace @ \
        mov ra, r \
        cmp ra, rb \
        JBU $NoChange \
        mov rb, r \
    $NoChange: \
    namespace

#define SET_ZERO(r) \
    xor r, r, r

#endif

; buffer versions of kill and rm
; register passed in contains address of filename
#define SYS_KILL_BUF(p) \
    mov p, r0 \
    swi SWI_KILL
    
#define SYS_RM_BUF(p, r) \
    mov p, r0 \
    mov r, r1 \
    swi SWI_RM

; get path name
;    
#define SYS_GTNAME(r) \
    mov r31, r0 \
    mov r, r1 \
    swi SWI_VFILE_PATH


;//////////////////////////////////////////////////////////
; These are variables holding scanned file pathname and size.
; They are used by the macros that affect the current scanned file.
;//////////////////////////////////////////////////////////


#define FILE_PATH_LENGTH 300
#define SCAN_FILE_PATH r30
#define SCAN_FILE_PATH_LENGTH r31
#define CODE_SECTION 1
#define NON_CODE_SECTION 0


#define MCS_SC_USER 1
#define MCS_SC_TERMINAL 2

#define MCS_BC_LEVEL0 0
#define MCS_BC_LEVEL1 1
#define MCS_BC_LEVEL2 2
#define MCS_BC_LEVEL3 3
#define MCS_BC_LEVEL4 4

#define MCS_DTYPE_UNKNOWN 0
#define MCS_DTYPE_HTML 1
#define MCS_DTYPE_URL 2
#define MCS_DTYPE_EMAIL 3
#define MCS_DTYPE_PHONE 4
#define MCS_DTYPE_JAVA 5
#define MCS_DTYPE_JAVAS 6
#define MCS_DTYPE_TEXT 7
#define MCS_DTYPE_ELF_TEXT 8
#define MCS_DTYPE_PE_TEXT 9
#define MCS_DTYPE_E32_TEXT 10
#define MCS_DTYPE_VBA 11
#define MCS_DTYPE_WM6MAC 12
#define MCS_DTYPE_PPT 13
#define MCS_DTYPE_DOC 14
#define MCS_DTYPE_VBAMAC 15
#define MCS_DTYPE_VBASRP 16

#define TCS_SC_USER MCS_SC_USER
#define TCS_SC_TERMINAL MCS_SC_TERMINAL


#define TCS_BC_LEVEL0 MCS_BC_LEVEL0
#define TCS_BC_LEVEL1 MCS_BC_LEVEL1
#define TCS_BC_LEVEL2 MCS_BC_LEVEL2
#define TCS_BC_LEVEL3 MCS_BC_LEVEL3
#define TCS_BC_LEVEL4 MCS_BC_LEVEL4

