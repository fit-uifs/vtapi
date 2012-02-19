
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "vtapi_libpq.h"

/* cube type handlers */
int cube_put (PGtypeArgs *args) {

    PGcube *cube = va_arg(args->ap, PGcube *);
    unsigned int * buf = NULL;
    char * out = NULL;
    int len = 0, xcnt = 0, dim = 0, varlen = 0;

    if (!args || !cube) return 0;

    /* expand buffer enough */
    xcnt = cube->dim * 2;
    len = (2 * sizeof(int)) + (xcnt * sizeof(double));
    if (args->put.expandBuffer(args, len) == -1) return -1;

    /* put header - varlena and dimensions count */
    out = args->put.out;
    varlen = htonl((sizeof(int) * 2) + (xcnt * sizeof(double)));
    memcpy(out, &varlen, sizeof(int));
    out += sizeof(int);
    dim = htonl(cube->dim);
    memcpy(out, &dim, sizeof(int));
    out += sizeof(int);

    /* put coordinates */
    for (int i = 0; i < xcnt; i++) {
        pq_swap8(out, &cube->x[i], 1);
        out += sizeof(double);
    }
    return len;
}
int cube_get (PGtypeArgs *args) {
    /* get received value and its length */
    char *val = PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
    int len = PQgetlength(args->get.result, args->get.tup_num, args->get.field_num);
    int xcnt = (len - (2 * sizeof(int))) / sizeof(double);;  /* coordinates count */
    double *xp; /* pointer to coordinates */

    PGcube *cube = va_arg(args->ap, PGcube *);

    if (len == 0 || xcnt == 0) {
        cube->dim = 0;
        cube->x = NULL;
        return 0;
    }
    
    pq_swap4(&cube->dim, val + sizeof(int), 1); /* number of dimensions */

    /* allocate and extract coordinates */
    xp = (double *) (val + 2 * sizeof(int));
    cube->x = (double *) PQresultAlloc((PGresult *) args->get.result, xcnt * sizeof(double));
    if (!cube->x) return 0;
    for (int i = 0; i < xcnt; i++) {
        pq_swap8(&cube->x[i], xp++, 1);
    }   
    return 0;
}

int geometry_put (PGtypeArgs *args) {
    //TODO
    return 0;
}

int geometry_get (PGtypeArgs *args) {
    /* get received value and its length */
    uchar *val = (uchar *)PQgetvalue(args->get.result, args->get.tup_num, args->get.field_num);
    int len = PQgetlength(args->get.result, args->get.tup_num, args->get.field_num);
    /* all kinds of geometries we will need */
    GEOSGeometry ** ggeom = va_arg(args->ap, GEOSGeometry **);
    PG_LWGEOM * geom;
    LWGEOM * lwgeom;

    if (len == 0) {
        *ggeom = NULL;
        return 0;
    }
    
    /* parse EWKB input into PG_LWGEOM type */
    geom = pglwgeom_from_ewkb(val,0,len);
    /* convert to LWGEOM */
    lwgeom = lwgeom_deserialize(SERIALIZED_FORM(geom));
    /* and create desired GEOSGeometry type */
    *ggeom = LWGEOM2GEOS(lwgeom);

    /* free memory */
    lwgeom_free(lwgeom);
    lwfree(geom);

    return 0;
}


/***************** PostGIS stuff *****************/
GEOSGeometry *
LWGEOM2GEOS(LWGEOM *lwgeom)
{
	GEOSCoordSeq sq;
	GEOSGeom g, shell, *geoms;
	/*
	LWGEOM *tmp;
	*/
	unsigned int ngeoms, i;
	int type = 0;
	int geostype;
#if POSTGIS_DEBUG_LEVEL >= 4
	char *wkt;
#endif

/*
	POSTGIS_DEBUGF(4, "LWGEOM2GEOS got a %s", lwgeom_typename(type));
*/

	if (has_arc(lwgeom))
	{
/*
		POSTGIS_DEBUG(3, "LWGEOM2GEOS_c: arced geometry found.");
*/

		lwerror("Exception in LWGEOM2GEOS: curved geometry not supported.");
		/*
		tmp = lwgeom;
		lwgeom = lwgeom_segmentize(tmp, 32);
		POSTGIS_DEBUGF(3, "LWGEOM2GEOM_c: was %p, is %p", tmp, lwgeom);
		*/
	}
	type = TYPE_GETTYPE(lwgeom->type);
	switch (type)
	{
		LWPOINT *lwp;
		LWPOLY *lwpoly;
		LWLINE *lwl;
		LWCOLLECTION *lwc;

	case POINTTYPE:
		lwp = (LWPOINT *)lwgeom;
		sq = ptarray_to_GEOSCoordSeq(lwp->point);
		g = GEOSGeom_createPoint(sq);
		if ( ! g ) lwerror("Exception in LWGEOM2GEOS");
		break;
	case LINETYPE:
		lwl = (LWLINE *)lwgeom;
		sq = ptarray_to_GEOSCoordSeq(lwl->points);
		g = GEOSGeom_createLineString(sq);
		if ( ! g ) lwerror("Exception in LWGEOM2GEOS");
		break;

	case POLYGONTYPE:
		lwpoly = (LWPOLY *)lwgeom;
		sq = ptarray_to_GEOSCoordSeq(lwpoly->rings[0]);
		shell = GEOSGeom_createLinearRing(sq);
		if ( ! shell ) return NULL;
		/*lwerror("LWGEOM2GEOS: exception during polygon shell conversion"); */
		ngeoms = lwpoly->nrings-1;
		geoms = (GEOSGeometry **)malloc(sizeof(GEOSGeom)*ngeoms);
		for (i=1; i<lwpoly->nrings; ++i)
		{
			sq = ptarray_to_GEOSCoordSeq(lwpoly->rings[i]);
			geoms[i-1] = GEOSGeom_createLinearRing(sq);
			if ( ! geoms[i-1] ) return NULL;
			/*lwerror("LWGEOM2GEOS: exception during polygon hole conversion"); */
		}
		g = GEOSGeom_createPolygon(shell, geoms, ngeoms);
		if ( ! g ) return NULL;
		free(geoms);
		break;
	case MULTIPOINTTYPE:
	case MULTILINETYPE:
	case MULTIPOLYGONTYPE:
	case COLLECTIONTYPE:
		if ( type == MULTIPOINTTYPE )
			geostype = GEOS_MULTIPOINT;
		else if ( type == MULTILINETYPE )
			geostype = GEOS_MULTILINESTRING;
		else if ( type == MULTIPOLYGONTYPE )
			geostype = GEOS_MULTIPOLYGON;
		else
			geostype = GEOS_GEOMETRYCOLLECTION;

		lwc = (LWCOLLECTION *)lwgeom;
		ngeoms = lwc->ngeoms;
		geoms = (GEOSGeometry **)malloc(sizeof(GEOSGeom)*ngeoms);

		for (i=0; i<ngeoms; ++i)
		{
			geoms[i] = LWGEOM2GEOS(lwc->geoms[i]);
			if ( ! geoms[i] ) return NULL;
		}
		g = GEOSGeom_createCollection(geostype, geoms, ngeoms);
		if ( ! g ) return NULL;
		free(geoms);
		break;

	default:
		lwerror("Unknown geometry type: %d", type);

		return NULL;
	}

	GEOSSetSRID(g, lwgeom->SRID);

#if POSTGIS_DEBUG_LEVEL >= 4
	wkt = GEOSGeomToWKT(g);
	POSTGIS_DEBUGF(4, "LWGEOM2GEOS: GEOSGeom: %s", wkt);
	/*
	if(tmp != NULL) lwgeom_release(tmp);
	*/
	free(wkt);
#endif

	return g;
}

GEOSCoordSeq
ptarray_to_GEOSCoordSeq(POINTARRAY *pa)
{
	unsigned int dims = 2;
	unsigned int size, i;
	POINT3DZ p;
	GEOSCoordSeq sq;

	if ( TYPE_HASZ(pa->dims) ) dims = 3;
	size = pa->npoints;

	sq = GEOSCoordSeq_create(size, dims);
	if ( ! sq ) lwerror("Error creating GEOS Coordinate Sequence");

	for (i=0; i<size; i++)
	{
		getPoint3dz_p(pa, i, &p);

/*
		POSTGIS_DEBUGF(4, "Point: %g,%g,%g", p.x, p.y, p.z);
*/

#if POSTGIS_GEOS_VERSION < 33
    /* Make sure we don't pass any infinite values down into GEOS */
    /* GEOS 3.3+ is supposed to  handle this stuff OK */
    if ( isinf(p.x) || isinf(p.y) || (dims == 3 && isinf(p.z)) )
      lwerror("Infinite coordinate value found in geometry.");
#endif

		GEOSCoordSeq_setX(sq, i, p.x);
		GEOSCoordSeq_setY(sq, i, p.y);
		if ( dims == 3 ) GEOSCoordSeq_setZ(sq, i, p.z);
	}
	return sq;
}

PG_LWGEOM *
pglwgeom_from_ewkb(uchar *ewkb, int flags, size_t ewkblen)
{
	PG_LWGEOM *ret;
	LWGEOM_PARSER_RESULT lwg_parser_result;
	char *hexewkb;
	size_t hexewkblen = ewkblen*2;
	int i, result;

	hexewkb = (char *)lwalloc(hexewkblen+1);
	for (i=0; i<ewkblen; i++)
	{
		deparse_hex(ewkb[i], &hexewkb[i*2]);
	}
	hexewkb[hexewkblen] = '\0';

	result = serialized_lwgeom_from_ewkt(&lwg_parser_result, hexewkb, flags);
        
/*
	if (result)
		PG_PARSER_ERROR(lwg_parser_result);
*/
        /* this was changed from palloc to malloc */
        ret = (PG_LWGEOM *)malloc(lwg_parser_result.size + VARHDRSZ);
	SET_VARSIZE(ret, lwg_parser_result.size + VARHDRSZ);
	memcpy(VARDATA(ret), lwg_parser_result.serialized_lwgeom, lwg_parser_result.size);

	lwfree(hexewkb);

	return ret;
}

void pq_swap4(void *outp, void *inp, int tonet)
{
    static int n = 1;

    unsigned int *in = (unsigned int *) inp;
    unsigned int *out = (unsigned int *) outp;

    if (*(char *)&n == 1)
        *out = tonet ? htonl(*in) : ntohl(*in);
    else
        *out = *in;

}

void pq_swap8(void *outp, void *inp, int tonet)
{
    static int n = 1;

    unsigned int *in = (unsigned int *) inp;
    unsigned int *out = (unsigned int *) outp;

    if (*(char *)&n == 1) {
        out[0] = (unsigned int) (tonet ? htonl(in[1]) : ntohl(in[1]));
        out[1] = (unsigned int) (tonet ? htonl(in[0]) : ntohl(in[0]));
    }
    else {
        out[0] = in[0];
        out[1] = in[1];
    }
}

void lwgeom_init_allocators() {
    lwgeom_install_default_allocators();
}
