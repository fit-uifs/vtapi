/* 
 * File:   vtapi_libpq.h
 * Author: vojca
 *
 * Created on February 15, 2012, 2:19 PM
 */

#ifndef VTAPI_LIBPQ_H
#define	VTAPI_LIBPQ_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "vtapi_config.h"

#ifdef POSTGIS

#include <malloc.h>
#include <math.h>


/* postgres headers format */
typedef union
{
    struct                      /* Normal varlena (4-byte length) */
    {
    uint32      va_header;
    char        va_data[1];
    }           va_4byte;
    struct                      /* Compressed-in-line format */
    {
         uint32      va_header;
         uint32      va_rawsize; /* Original data size (excludes header) */
         char        va_data[1]; /* Compressed data */
    }           va_compressed;
} varattrib_4b;

/* some useful macros extracted from postgres.h */
#define SET_VARSIZE(PTR, len) \
(((varattrib_4b *) (PTR))->va_4byte.va_header = (len) & 0x3FFFFFFF)
#define VARDATA(PTR)            (((varattrib_4b *) (PTR))->va_4byte.va_data)
#define VARHDRSZ		((int32) sizeof(int32))
#define SERIALIZED_FORM(x) ((uchar *)VARDATA((x)))


/* custom cube struct */
typedef struct
{
    unsigned int dim; /* number of cube dimensions */
    double *x;  /* coordinates */
} PGcube;


/**
 * Cube type libpqtypes put handler
 * @param
 * @return
 */
int cube_put (PGtypeArgs *);
/**
 * Cube type libpqtypes get handler
 * @param
 * @return
 */
int cube_get (PGtypeArgs *);
/**
 * Geometry type (PostGIS) libpqtypes put handler
 * @param
 * @return
 */
int geometry_put (PGtypeArgs *);
/**
 * Geometry type (PostGIS) libpqtypes get handler
 * @param
 * @return
 */
int geometry_get (PGtypeArgs *);

/**
 * Endian swapper for 4-byte long types
 * @param out buffer
 * @param in buffer
 * @param Indicator of swapping type (0 - htonl, 1 - ntohl)
 */
void pq_swap4(void *, void *, int);
/**
 * Endian swapper for 8-byte long types
 * @param out buffer
 * @param in buffer
 * @param Indicator of swapping type (0 - htonl, 1 - ntohl)
 */
void pq_swap8(void *, void *, int);

/**
 * liblwgeom requires this to be implemented
 */
// void lwgeom_init_allocators();
// FIXME: tohle hlasi konflikt

/**
 * Extract serialized PG_LWGEOM geometry from EWKB binary
 * @param ewkb serialized input
 * @param flags
 * @param ewkblen length
 * @return
 */
PG_LWGEOM * pglwgeom_from_ewkb(uchar *ewkb, int flags, size_t ewkblen);
/**
 * Convert LWGEOM type geometry to GEOSGeometry format
 * @param g
 * @return
 */
GEOSGeometry * LWGEOM2GEOS(LWGEOM *g);
/**
 * Convert POINTARRAY of coordinates to GEOSCoordSeq format
 * @param
 * @return 
 */
GEOSCoordSeq ptarray_to_GEOSCoordSeq(POINTARRAY *);


#endif /* POSTGIS */

#ifdef	__cplusplus
}
#endif

#endif	/* VTAPI_LIBPQ_H */

