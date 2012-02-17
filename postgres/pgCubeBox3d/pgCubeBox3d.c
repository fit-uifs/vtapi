/* 
 * File:   pgCubeBox3d.c
 * Author: Volf Tomas, ivolf@fit.vutbr.cz
 *
 * Created on 14 February 2012
 */


// #define _DEBUG // For debug uncomment this line

#include "postgres.h"           // general Postgres declarations
#include "fmgr.h"               // Postgres function manager and function-call interface

#include "include/liblwgeom.h"  // Geometry representations
#include "include/cubedata.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;		// identification
#endif


/*
 * The macro PG_ARGISNULL(n) allows a function to test whether each input is null. (Of course, 
 * doing this is only necessary in functions not declared "strict".) 
 * As with the PG_GETARG_xxx() macros, the input arguments are counted beginning at zero.
 * PG_GETARG_xxx_COPY() guarantees to return a copy of the specified argument that is safe for writing into.
 *
 * Never modify the contents of a pass-by-reference input value. If you do so you are likely to corrupt 
 * on-disk data, since the pointer you are given might point directly into a disk buffer. 
 * The sole exception to this rule is explained in Section 34.10 (User-Defined Aggregates). 
 * 
 * When allocating memory, use the PostgreSQL functions palloc and pfree instead of the corresponding 
 * C library functions malloc and free. The memory allocated by palloc will be freed automatically 
 * at the end of each transaction, preventing memory leaks. 
 * Always zero the bytes of your structures using memset. Without this, it's difficult to support 
 * hash indexes or hash joins, as you must pick out only the significant bits of your data structure 
 * to compute a hash. Even if you initialize all fields of your structure, there might be alignment padding 
 * (holes in the structure) that contain garbage values.
 *
 */


PG_FUNCTION_INFO_V1(box3d2cube);
/**
 * Convert a geometry box3d representation to the cube representation
 * @param box3d representation od the geometry
 * @return cube representation of a box3d
 */
Datum box3d2cube(PG_FUNCTION_ARGS)
{
    BOX3D* box3d = (BOX3D*) PG_GETARG_POINTER(0);
    NDBOX* result;

    int dim = 0;
    if (box3d != NULL) {
        dim = 3;
    }

    int size = offsetof(NDBOX, x[0]) + sizeof(double) * 2 * dim;
    result = (NDBOX*) palloc0(size);
    SET_VARSIZE(result, size);

    result->dim = dim;

    if (dim) {
        result->x[0] = box3d->xmin;
        result->x[0 + dim] = box3d->xmax;
        result->x[1] = box3d->ymin;
        result->x[1 + dim] = box3d->ymax;
        result->x[2] = box3d->zmin;
        result->x[2 + dim] = box3d->zmax;
    }

    PG_RETURN_NDBOX(result);
}



PG_FUNCTION_INFO_V1(cube2box3d);
/**
 * Convert a cube representation to the box3d representation
 * @param cube representation
 * @return box3d representation
 */
Datum cube2box3d(PG_FUNCTION_ARGS)
{
    NDBOX* cube = (NDBOX*) PG_GETARG_POINTER(0);
    BOX3D* result = (BOX3D *) palloc(sizeof(BOX3D));
    SET_VARSIZE(result, sizeof(BOX3D));

    result->xmin = result->xmax = result->ymin = result->ymax = result->zmin = result->zmax = 0;

    switch (cube->dim) {
      case 3:
          result->zmin = cube->x[2];
          result->zmax = cube->x[2 + cube->dim];
      case 2:
          result->ymin = cube->x[1];
          result->ymax = cube->x[1 + cube->dim];
      case 1:
          result->xmin = cube->x[0];
          result->xmax = cube->x[0 + cube->dim];
        break;
      default:
          ereport(ERROR, (errcode(ERRCODE_DATA_EXCEPTION),
                       errmsg("Unexpected number of cube dimensions.")));
        break;
    }

    PG_RETURN_POINTER(result);
}


