// ****************************************************************************
/*!
Module:     Data definitions.

Platform:   Independent

\file       typeDef.h

\brief      Data type definitions header.
            For Microsoft type, see this link: 
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751%28v=vs.85%29.aspx

\author     Frédéric Parent
\since      Nov 5, 2012

\copyright (c) 2012-2013 LeddarTech Inc. All rights reserved.
*/
// ***************************************************************************

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__


//*****************************************************************************
//*************** Header Includes *********************************************
//*****************************************************************************

// Standard C libraries include files
#include <stddef.h>         // to get size_t, NULL, wchar_t
#include <stdint.h>
#if __STDC_VERSION__ >= 199901L     // Support of C99 compiler version.
    #include <stdbool.h>    // to get bool, true, false
#endif


//*****************************************************************************
//************** Constants, Macros and Data Type Definition *******************
//*****************************************************************************

//******************************
// Short name standard types 
//******************************
/*
typedef unsigned long long  U64;
typedef unsigned long       U32;
typedef unsigned short      U16;
typedef unsigned char       U8;
typedef long long           S64;
typedef long                S32;
typedef short               S16;
typedef char                S8;
typedef float               F32;
typedef double              F64;
*/
//*******************************************
// Some standard Microsoft style data types
//******************************************
#ifndef _WINDOWS_
#define CONST               const
typedef unsigned long long  ULONGLONG,  *PULONGLONG;
typedef unsigned long       DWORD,      *PDWORD;
typedef unsigned long       ULONG,      *PULONG;
typedef unsigned long       ULONG_PTR,  *PULONG_PTR;
typedef ULONG_PTR           SIZE_T,     *PSIZE_T;
typedef unsigned int        UINT,       *PUINT;
typedef unsigned short      WORD,       *PWORD;
typedef unsigned short      USHORT,     *PUSHORT;
typedef wchar_t             WCHAR,      *PWCHAR;
typedef unsigned char       BYTE,       *PBYTE;
typedef unsigned char       UCHAR,      *PUCHAR;
typedef BYTE                BOOLEAN,    *PBOOLEAN;
typedef long long           LONGLONG,   *PLONGLONG;
typedef long                LONG,       *PLONG;
typedef int                 INT,        *PINT;
typedef short               SHORT,      *PSHORT;
typedef char                CHAR,       *PCHAR;
typedef float               FLOAT,      *PFLOAT;
typedef void                VOID,       *PVOID;
typedef PVOID               HANDLE,     *PHANDLE; 
#endif // _WINDOWS_

//******************************
// LeddarTech types 
//******************************
typedef unsigned long long  LtUInt64;
typedef unsigned int        LtUInt32;
typedef unsigned short      LtUInt16;
typedef unsigned char       LtUInt8;
typedef long long           LtInt64;
typedef int                 LtInt32, LtFixedPoint;
typedef short               LtInt16;
typedef char                LtInt8;
typedef float               LtFloat32;
typedef double              LtFloat64;


//******************************
// LabWindows
//******************************
//typedef unsigned char       uchar;
typedef unsigned int        uint;

//******************************
// Boolean definitions.
//******************************
#if __bool_true_false_are_defined   // Normally found in <stdbool.h>

    // Boolean type definition
    #ifndef BOOL
        #define BOOL                    bool 
    #endif

    // Boolean states definitions
    #ifndef TRUE
        #define TRUE                    true
        #define FALSE                   false
    #endif

#else   // __bool_true_false_are_defined

    // Boolean type definition
    #ifndef __cplusplus
        #ifndef bool
            #define bool                unsigned char
        #endif
    #endif

    #ifndef BOOL
        #ifdef __cplusplus
            #define BOOL                bool
        #else
            #define BOOL                unsigned char
        #endif
    #endif

    // Boolean states definitions
    #ifndef TRUE
        #ifdef __cplusplus
            #define TRUE                true
            #define FALSE               false
        #else
            #define TRUE                ((BOOL) 1)
            #define FALSE               ((BOOL) 0)
        #endif
    #endif
#endif  // __bool_true_false_are_defined


//******************************
// Null definition.
//******************************
#ifndef NULL
    #define NULL                        0L
#endif


#endif  /* __TYPEDEF_H__ */
