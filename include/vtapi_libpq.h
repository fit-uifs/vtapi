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

//#include "postgis/liblwgeom.h"
#include "postgresql/libpqtypes.h"

/* cube struct */
typedef struct
{
    int vl_len;
    unsigned int dim; /* number of cube dimensions */
    double *x;  /* coordinates */
} PGcube;

typedef struct
{
	unsigned char endian; /* 1 byte - endian indicator */
	unsigned char gtype; /* 1 byte - geom type, 3 bytes - flags */
        unsigned int size; /* size of array */
        //double *x; /* coordinates */
	//char flags; /* HasZ, HasM, HasBBox, IsGeodetic */
	//char *data;
} ewkb_t;

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

#ifdef	__cplusplus
}
#endif

#endif	/* VTAPI_LIBPQ_H */

