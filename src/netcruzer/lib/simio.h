#if !defined(NZSYS_DISABLE_STDIO_WRITE)

#ifndef _FD_INVALID
#define _FD_INVALID (-1)
#endif

#if !defined(__SIMIO__)
typedef enum
{
    SIM_OPEN,
    SIM_CLOSE,
    SIM_READ,
    SIM_WRITE,
    SIM_LSEEK,
    SIM_UNLINK,
    SIM_RENAME
}
    SIMMETHOD;
typedef struct tagSIMOPEN
{
    const char *    name;
    int        access;
    int        mode;
    int        handle;
}
    SIMOPEN;
typedef struct tagSIMCLOSE
{
    int        handle;
}
    SIMCLOSE;
typedef struct tagSIMREAD
{
    int        handle;
    const void *    buffer;
    unsigned int    len;
}
    SIMREAD;
typedef struct tagSIMWRITE
{
    int        handle;
    const void *    buffer;
    unsigned int    len;
}
    SIMWRITE;
typedef struct tagSIMLSEEK
{
    int        handle;
    long        offset;
    int        origin;
}
    SIMLSEEK;
typedef struct tagSIMUNLINK
{
    const char *    filename;
    int        rc;
}
    SIMUNLINK;
typedef struct tagSIMRENAME
{
    const char *    oldname;
    const char *    newname;
    int        rc;
}
    SIMRENAME;
typedef struct tagSIMIO
{
    SIMMETHOD    method;
    union
    {
    SIMOPEN        open;
    SIMCLOSE    close;
    SIMREAD        read;
    SIMWRITE    write;
    SIMLSEEK    lseek;
    SIMUNLINK    unlink;
    SIMRENAME    rename;
    }
    u;
}
    SIMIO, *PSIMIO;
#endif

#endif  //#if !defined(NZSYS_DISABLE_STDIO_WRITE)