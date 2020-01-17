/* Code generated by cmd/cgo; DO NOT EDIT. */

/* package _/root/desktop/perf_tuning_results/gojvmti/src */


#line 1 "cgo-builtin-export-prolog"

#include <stddef.h> /* for ptrdiff_t below */

#ifndef GO_CGO_EXPORT_PROLOGUE_H
#define GO_CGO_EXPORT_PROLOGUE_H

#ifndef GO_CGO_GOSTRING_TYPEDEF
typedef struct { const char *p; ptrdiff_t n; } _GoString_;
#endif

#endif

/* Start of preamble from import "C" comments.  */


#line 3 "agent.go"
#include <stdio.h>
#include <stdlib.h>
#include <jvmti.h>

#line 1 "cgo-generated-wrapper"

#line 3 "config.go"
#include <stdio.h>
#include <stdlib.h>
#include <jvmti.h>
#include "api.h"

#line 1 "cgo-generated-wrapper"



/* End of preamble from import "C" comments.  */


/* Start of boilerplate cgo prologue.  */
#line 1 "cgo-gcc-export-header-prolog"

#ifndef GO_CGO_PROLOGUE_H
#define GO_CGO_PROLOGUE_H

typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;
typedef GoInt64 GoInt;
typedef GoUint64 GoUint;
typedef __SIZE_TYPE__ GoUintptr;
typedef float GoFloat32;
typedef double GoFloat64;
typedef float _Complex GoComplex64;
typedef double _Complex GoComplex128;

/*
  static assertion to make sure the file is being used on architecture
  at least with matching size of GoInt.
*/
typedef char _check_for_64_bit_pointer_matching_GoInt[sizeof(void*)==64/8 ? 1:-1];

#ifndef GO_CGO_GOSTRING_TYPEDEF
typedef _GoString_ GoString;
#endif
typedef void *GoMap;
typedef void *GoChan;
typedef struct { void *t; void *v; } GoInterface;
typedef struct { void *data; GoInt len; GoInt cap; } GoSlice;

#endif

/* End of boilerplate cgo prologue.  */

#ifdef __cplusplus
extern "C" {
#endif


extern void gCacheMapCount();

extern void gCacheMapClean();

extern void gCacheObject(char* p0, char* p1, int p2);

extern void gOptions(jvmtiEnv* p0, char* p1);

extern GoInt goAtoi(char* p0);

extern char* gStringAdd(char* p0, char* p1, char* p2);

extern void gLog(char* p0);

#ifdef __cplusplus
}
#endif
