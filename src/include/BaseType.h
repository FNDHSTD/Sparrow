#ifndef __BASETYPE_H__
#define __BASETYPE_H__

typedef unsigned long long U64;
typedef unsigned int       U32;
typedef unsigned short     U16;
typedef unsigned char      U8;
typedef long long          I64;
typedef int                I32;
typedef short              I16;
typedef signed char        I8;
typedef char               C8;
typedef unsigned short     C16;

typedef U64 PHYSICAL_ADDRESS;

typedef U64 STATUS_CODE;

typedef void VOID;

typedef enum {
    False,
    True,
} BOOL;

#define IN
#define OUT

#endif
