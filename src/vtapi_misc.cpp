/**
 * @file
 * @brief   Miscellaneous support stuff, mostly C
 *
 * @author   Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref Licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

/*
#include <stdio.h>
#include <string.h>
*/

#include <common/vtapi_global.h>
#include <common/vtapi_misc.h>



void endian_swap2(void *outp, void *inp)
{
    static unsigned short n = 0x0001;

    unsigned short *in = (unsigned short *) inp;
    unsigned short *out = (unsigned short *) outp;

    if (*(char *)&n == 0x01) {
        *out = ((((*in) >> 8) & 0xff) | (((*in) & 0xff) << 8));
    }
    else
        *out = *in;
}

void endian_swap4(void *outp, void *inp)
{
    static unsigned short n = 0x0001;

    unsigned int *in = (unsigned int *) inp;
    unsigned int *out = (unsigned int *) outp;

    if (*(char *)&n == 0x01) {
        *out = ((((*in) & 0xff000000) >> 24) | (((*in) & 0x00ff0000) >>  8) |
                (((*in) & 0x0000ff00) <<  8) | (((*in) & 0x000000ff) << 24));
    }
    else
        *out = *in;
}

void endian_swap8(void *outp, void *inp)
{
    static unsigned short n = 0x0001;

    unsigned long *in = (unsigned long *) inp;
    unsigned long *out = (unsigned long *) outp;

    if (*(char *)&n == 0x01) {
        *out = ((((*in) & 0xff00000000000000ull) >> 56)
              | (((*in) & 0x00ff000000000000ull) >> 40)
              | (((*in) & 0x0000ff0000000000ull) >> 24)
              | (((*in) & 0x000000ff00000000ull) >> 8)
              | (((*in) & 0x00000000ff000000ull) << 8)
              | (((*in) & 0x0000000000ff0000ull) << 24)
              | (((*in) & 0x000000000000ff00ull) << 40)
              | (((*in) & 0x00000000000000ffull) << 56));
    }
    else {
        *out = *in;
    }
}

#if HAVE_POSTGIS
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


/*
 * This is in liblwgeom/lwutil.c right now
void lwgeom_init_allocators() {
    lwgeom_install_default_allocators();
}
*/

void geos_notice (const char * fmt, ...) {
      char *msg;
      va_list ap;
      va_start (ap, fmt);

      if (!vsprintf (msg, fmt, ap))
      {
            va_end (ap);
            return;
      }
      printf("%s\n", msg);
      va_end(ap);
      free(msg);
}

#endif
