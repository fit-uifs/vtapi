/* 
 * File:   cvAbbrevs.h
 * Author: chmelarp
 *
 * Created on 16. duben 2008, 15:36
 */

#ifndef _ABBREVS_H
#define	_ABBREVS_H

/**
 * General (C, C++) abbreviations
 *
 */

// #include <stdlib.h>
#include <math.h>


// take it easy(TM) (Java like)
#define null NULL
#ifdef	__cplusplus
#define String string
using namespace std;
#endif

// easy boolean(TM)
// use boolean in stead of bool, because of C - it doesn't have it :)
// easy something else
#ifdef	__cplusplus
#define boolean bool
#else
#define boolean int
#define false   0
#define true    1
#endif

#ifdef	__cplusplus
    // to be a byte (unsigned char)
    #define byte unsigned char
    // word (unsigned int 32b a nikdy jinak :)
    #define word unsigned int;
#else
    // to be a byte (unsigned char)
    #define byte uint8_t
    // word (unsigned int 32b a nikdy jinak :)
    #define word uint32_t;
#endif

// easy malloc(TM)
// use alloc and free instead of new and delete in case of structures
// because it doesn't initialize anything and C doesn't have it
#define alloc(type,n) (type *)malloc((n)*sizeof(type))
#define allinit(pointer, type, n) (pointer)=(type *)malloc((n)*sizeof(type)), memset((pointer), 0, (n)*sizeof(type)), (pointer)

// short strings length
#define MAXChars 255
#define MAXString 2048

// use pi
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// absolute value, signum
#define ABS(a) ((a) >= 0 ? (a) : (-(a)))
#define SIGN(a) ((a) > 0 ? 1 : -1)

// maximum, minimum
#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

// round (returns long integer)
#define ROUND(x) floor(0.5 + (x))
// swap two values
#define SWAP(a,b) swp_temp=(a);(a)=(b);(b)=swp_temp
// square x^2
#define SQR(x) (x)*(x)


#endif	/* _ABBREVS_H */

