/* 
 * File:   pgsiftorder.c
 * Author: chmelarp
 *
 * Created on 16 April 2008, 15:36 CET
 * Updated on  7 April 2009, 10:31 CET
 * 
 * See the INFO.txt for reference!
 * 
 * TODO: Optimize c_rating_cosine into c_rating_cosine_XY(e1, w1, norm1, e2, w2, norm2) -- add the morm that wont be counted or using below:
 * TODO: Create c_rating_normalize_vect to perform the mormalization required for above
 */

// debugging? uncomment this...
// #define _DEBUG

// according to the config.log (above 8.3.3 already defined)
// #define PG_VERSION_NUM 80303

#include <math.h>
#include "postgres.h"           // general Postgres declarations
#include "fmgr.h"               // Postgres function manager and function-call interface
#include "utils/typcache.h"     // for Type cache definitions
#include "access/tupmacs.h"     // Tuple macros used by both index tuples and heap tuples

#include "utils/array.h"        // Declarations for Postgres arrays.
// #include "contrib/intarray/_int.h"
// #include "executor/executor.h"  // for GetAttributeByName()

// identification... its me :)
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
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



PG_FUNCTION_INFO_V1(c_rating_cosine_norm);
/*
 * Counts cosine rating of two vectors using the pre-counted norm (recomended).
 * // @param norm1 float = 1
 * // @param norm2 float = 1
 * @param elements1 int4[]
 * @param weights1 float[]
 * @param elements2 int4[]
 * @param weights2 float[]
 */
Datum 
c_rating_cosine_norm(PG_FUNCTION_ARGS) {
    float rating = 0;
    
    // Feel free to implement the fastest version when documents and queries have the unit L2 norm (1):
    // r(dq, dd) = |dq.dd|
    ereport(WARNING, (errcode(ERRCODE_UNDEFINED_FUNCTION),
    errmsg("function 'rating_cosine_norm' not implemented")));
    
    PG_RETURN_FLOAT4(0);
}


PG_FUNCTION_INFO_V1(c_rating_cosine);
/*
 * Counts cosine rating of two vectors.
 * @param elements1 int4[]
 * @param weights1 float[]
 * @param elements2 int4[]
 * @param weights2 float[]
 */
Datum 
c_rating_cosine(PG_FUNCTION_ARGS) {
    ArrayType*  vector1 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(0)));
    ArrayType*  weight1 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(1)));

    ArrayType*  vector2 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(2)));
    ArrayType*  weight2 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(3)));

    int32       length1 = ArrayGetNItems(ARR_NDIM(vector1), ARR_DIMS(vector1));   // array lengths
    int32       length2 = ArrayGetNItems(ARR_NDIM(vector2), ARR_DIMS(vector2));
    
    // check length of weights - for SIGSEGV :)
    if ( ( length1 > ArrayGetNItems(ARR_NDIM(weight1), ARR_DIMS(weight1) )) || ( length2 > ArrayGetNItems(ARR_NDIM(weight2), ARR_DIMS(weight2) )) ) {
        ereport(ERROR, (errcode(ERRCODE_CARDINALITY_VIOLATION),
                       errmsg("weight arrays must be of the same size as key arrays")));
    }

    int32*      ptr1 = (int32*) ARR_DATA_PTR(vector1);         // array data pointers
    float4*     ptrw1 = (float4*) ARR_DATA_PTR(weight1);
    int32*      ptr2 = (int32*) ARR_DATA_PTR(vector2);
    float4*     ptrw2 = (float4*) ARR_DATA_PTR(weight2);    
    int32       pos1 = 0;           // array position
    int32       pos2 = 0;
    float4      norm1 = 0;          // norms for the normalization
    float4      norm2 = 0;
    float4      rating = 0;         // result

    #ifdef _DEBUG
        ereport(NOTICE, (111111, errmsg("c_rating_cosine length1: %d length2: %d \r\n", length1, length2)));
    #endif
        
    //                  |dq.dd|
    //    r(dq, dd) = -----------
    //                 |dq|x|dd|
    //
    // go through the two vectors
    while (pos1 < length1 && pos2 < length2) {    

        #ifdef _DEBUG
            ereport(NOTICE, (111112, errmsg("c_rating_cosine pos1: %d (%d, %f) pos2: %d (%d, %f) \r\n", pos1, ptr1[pos1], ptrw1[pos1], pos2, ptr2[pos2], ptrw2[pos2])));
        #endif        
            
        // this is done for the first time and when values equals	
        if (ptr1[pos1] == ptr2[pos2]) {
            rating += (ptrw1[pos1] * ptrw2[pos2]);

            norm1 += (ptrw1[pos1] * ptrw1[pos1]);
            norm2 += (ptrw2[pos2] * ptrw2[pos2]);

            pos1++;
            pos2++;

	    #ifdef _DEBUG
                ereport(NOTICE, (111113, errmsg("ptr1[pos1] == ptr2[pos2] (pos1++ pos2++) \r\n")));
            #endif        
            continue;
        }
        // value of vector1 is smaller - set the next one
        else if(ptr1[pos1] < ptr2[pos2])
        {
            norm1 += (ptrw1[pos1] * ptrw1[pos1]);

            pos1++;

	    #ifdef _DEBUG
                ereport(NOTICE, (111113, errmsg("ptr1[pos1] < ptr2[pos2] (pos1++) \r\n")));
            #endif        
            continue;            
        }
        // cmpresult > 0 - try next position in vector2
        else
        {
            norm2 += (ptrw2[pos2] * ptrw2[pos2]);

            pos2++;

	    #ifdef _DEBUG
                ereport(NOTICE, (111113, errmsg("ptr1[pos1] > ptr2[pos2] (pos2++) \r\n")));
            #endif        
            continue;            
        }
    } // go through the two vectors

    // finish the vectors normalization (none if equal or one of them)
    int32 i;
    for (i = pos1; i < length1; i++) norm1 += (ptrw1[i] * ptrw1[i]);
    for (i = pos2; i < length2; i++) norm2 += (ptrw2[i] * ptrw2[i]);
    
    #ifdef _DEBUG
        ereport(NOTICE, (111114, errmsg("c_rating_cosine rating: %f norm1: %f norm2: %f (pos1: %d pos2: %d) \r\n", rating, sqrt(norm1), sqrt(norm2), pos1, pos2)));
    #endif

    // if the rating is 0 or it may cause division by 0, return 0
    if (rating == 0 || norm1 == 0 || norm2 == 0) PG_RETURN_FLOAT4(rating);

    // normalize
    rating /= (sqrt(norm1) * sqrt(norm2));

    #ifdef _DEBUG
        ereport(NOTICE, (111115, errmsg("c_rating_cosine return rating: %f \r\n", rating)));
    #endif
            
    PG_RETURN_FLOAT4(rating);
}



PG_FUNCTION_INFO_V1(c_rating_boolean_int4);
/****************************************************************************************************
 * Counts boolean rating of two vectors.
 * @param elements1 int4[]
 * @param elements2 int4[]
 */
Datum 
c_rating_boolean_int4(PG_FUNCTION_ARGS) {
    ArrayType*   vector1 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(0)));
    ArrayType*   vector2 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(1)));
    
    int32        length1 = ArrayGetNItems(ARR_NDIM(vector1), ARR_DIMS(vector1));   // array lengths
    int32        length2 = ArrayGetNItems(ARR_NDIM(vector2), ARR_DIMS(vector2));
    int32*       ptr1 = (int32*) ARR_DATA_PTR(vector1);         // array data pointers
    int32*       ptr2 = (int32*) ARR_DATA_PTR(vector2);
    int32        pos1 = 0;           // array position
    int32        pos2 = 0;
    int32        rating = 0;         // result

    #ifdef _DEBUG
        ereport(NOTICE, (111111, errmsg("c_rating_boolean_int4 length1: %d length2: %d", length1, length2)));
    #endif
    
    //
    // r(dq, dd) = |dq.dd|
    //
    // go through the two vectors
    while (pos1 < length1 && pos2 < length2) {    

        #ifdef _DEBUG
            ereport(NOTICE, (111111, errmsg("c_rating_boolean_impl pos1: %d (%d) pos2: %d (%d)", pos1, ptr1[pos1], pos2, ptr2[pos2])));
        #endif
            
        // this is done for the first time and when values equals	
        if (ptr1[pos1] == ptr2[pos2]) {
            rating++;
            
            pos1++;
            pos2++;
            continue;
        }
        // value of vector1 is smaller - set the next one
        else if(ptr1[pos1] < ptr2[pos2])
        {
            pos1++;
            continue;            
        }
        // cmpresult > 0 - try next position in vector2
        else
        {
            pos2++;
            continue;            
        }
    } // go through the two vectors
    
    PG_RETURN_INT32(rating);
}



PG_FUNCTION_INFO_V1(c_rating_boolean);
/*****************************************************************************************************
 * Counts the boolean rating of two vectors (equals to the number of identical elements).
 * Array must not contain any NULL elements nor be NULL itself.
 * @param anyarray1[]
 * @param anyarray2[]
 */
Datum c_rating_boolean(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT32(c_rating_boolean_anyarray(fcinfo));
}

// just an implementation
int32 c_rating_boolean_anyarray(FunctionCallInfo fcinfo) {
    ArrayType*  array1 = PG_GETARG_ARRAYTYPE_P(0);          // arrays to be compared
    ArrayType*  array2 = PG_GETARG_ARRAYTYPE_P(1);
    int32       ndims1 = ARR_NDIM(array1);                  // temporary dimension info
    int32       ndims2 = ARR_NDIM(array2);
    int32*      dims1 = ARR_DIMS(array1);
    int32*      dims2 = ARR_DIMS(array2);
    int32       length1 = ArrayGetNItems(ndims1, dims1);    // array lengths
    int32       length2 = ArrayGetNItems(ndims2, dims2);
    Oid         element_type = ARR_ELEMTYPE(array1);        // type of the array - int4 ~ 23, float4 ~ 800
    TypeCacheEntry* typentry;
    FunctionCallInfoData locfcinfo;
    bool        typbyval;
    int32       typlen;
    char        typalign;
    char*       ptr1 = ARR_DATA_PTR(array1);
    char*       ptr2 = ARR_DATA_PTR(array2);
    Datum       elt1;           // loop temps
    Datum       elt2;
    int32       cmpresult = 0;  // loop temp to hold information about compared info
    int32       pos1 = 0;      // array position (for the size info)
    int32       pos2 = 0;
    int32       rating = 0;        // result

    #ifdef _DEBUG
        ereport(NOTICE, (111111, errmsg("c_rating_boolean_impl length1: %d length2: %d", length1, length2)));
    #endif
        
    // if zero size... zero same elements and return zero :)
    if (length1 == 0 || length2 == 0) PG_RETURN_INT32(0);
    
    // check types of elements
    if (element_type != ARR_ELEMTYPE(array2))
            ereport(ERROR, (errcode(ERRCODE_DATATYPE_MISMATCH),
            errmsg("cannot compare arrays of different element types")));    
    
    /*
     * We arrange to look up the comparison function only once per series of
     * calls, assuming the element type doesn't change underneath us. The
     * typcache is used so that we have no memory leakage when being used as
     * an index support function.
     */
    typentry = (TypeCacheEntry *) fcinfo->flinfo->fn_extra;
    if (typentry == NULL || typentry->type_id != element_type)
    {
        typentry = lookup_type_cache(element_type, TYPECACHE_CMP_PROC_FINFO);
        if (!OidIsValid(typentry->cmp_proc_finfo.fn_oid))
                ereport(ERROR, (errcode(ERRCODE_UNDEFINED_FUNCTION),
                errmsg("could not identify a comparison function for type %i", format_type_be(element_type))));
        fcinfo->flinfo->fn_extra = (void *) typentry;
    }
    typlen = typentry->typlen;
    typbyval = typentry->typbyval;
    typalign = typentry->typalign;

    // apply the operator to each pair of array elements.
#if PG_VERSION_NUM > 90000
    InitFunctionCallInfoData(locfcinfo, &typentry->cmp_proc_finfo, 2, InvalidOid, NULL, NULL);
#else
    InitFunctionCallInfoData(locfcinfo, &typentry->cmp_proc_finfo, 2, NULL, NULL);
#endif 

    //
    // r(dq, dd) = |dq.dd|
    //
    // go through the two vectors
    while (pos1 < length1 && pos2 < length2) {
        // get the elements
        elt1 = fetch_att(ptr1, typbyval, typlen);
        elt2 = fetch_att(ptr2, typbyval, typlen);
        
        // Compare the pairs of elements
        locfcinfo.arg[0] = elt1;
        locfcinfo.arg[1] = elt2;
        locfcinfo.argnull[0] = false;
        locfcinfo.argnull[1] = false;
        locfcinfo.isnull = false;
        cmpresult = DatumGetInt32(FunctionCallInvoke(&locfcinfo));  // store the cmpresult to the next round
        
        #ifdef _DEBUG
            ereport(NOTICE, (111111, errmsg("c_rating_boolean_impl pos1: %d pos2: %d cmpresult: %d", pos1, pos2, cmpresult)));
        #endif

        // this is done for the first time and when values equals	
        if (cmpresult == 0) {
            pos1++;
            ptr1 = att_addlength_pointer(ptr1, typlen, ptr1);
            ptr1 = (char *) att_align_nominal(ptr1, typalign);
            
            pos2++;
            ptr2 = att_addlength_pointer(ptr2, typlen, ptr2);
            ptr2 = (char *) att_align_nominal(ptr2, typalign);            

            rating++;
            continue;
        }
        // value of vector1 is smaller - set the next one
        else if(cmpresult < 0)
        {
            pos1++;
            ptr1 = att_addlength_pointer(ptr1, typlen, ptr1);
            ptr1 = (char *) att_align_nominal(ptr1, typalign);            
            
            continue;            
        }
        // cmpresult > 0 - try next position in vector2
        else
        {
            pos2++;
            ptr2 = att_addlength_pointer(ptr2, typlen, ptr2);
            ptr2 = (char *) att_align_nominal(ptr2, typalign);            
            
            continue;            
        }
    } // go through the two vectors
    
    PG_RETURN_INT32(rating);
}


PG_FUNCTION_INFO_V1(c_distance_square_int4);
/****************************************************************************************************
 * Counts square distance of two vectors.
 * @param elements1 int4[]
 * @param elements2 int4[]
 */
Datum 
c_distance_square_int4(PG_FUNCTION_ARGS) {
    ArrayType*   vector1 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(0)));
    ArrayType*   vector2 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(1)));
    
    int32        length = ArrayGetNItems(ARR_NDIM(vector1), ARR_DIMS(vector1));   // array lengths
    if (length != ArrayGetNItems(ARR_NDIM(vector2), ARR_DIMS(vector2))) {
        ereport(ERROR, (errcode(ERRCODE_CARDINALITY_VIOLATION),
                        errmsg("both arrays must be of the same size")));
    }
    
    int32*       ptr1 = (int32*) ARR_DATA_PTR(vector1);         // array data pointers
    int32*       ptr2 = (int32*) ARR_DATA_PTR(vector2);
    int32        pos = 0;            // array position
    int64        distance = 0;       // result

    #ifdef _DEBUG
        ereport(NOTICE, (111111, errmsg("c_distance_square_int4 length: %d (%f)", length, (double)distance)));
    #endif
    
    // Euclidean distance without sqrt() normalization (square distance):
    // d(v1, v2) = Sum[ (v1i - v2i)^2 ]
    //              i
    //
    // go through the two vectors
    for (pos = 0; pos < length; pos++) {
        int64 diff = ptr1[pos] - ptr2[pos];
        distance += (diff * diff);
        #ifdef _DEBUG
            ereport(NOTICE, (111111, errmsg("c_distance_square_int4 pos: %d (%d, %d), diff: %f^2=%f, distance: %f", length, ptr1[pos], ptr2[pos], (double)diff, (double)(diff*diff), (double)distance)));
        #endif
    } // go through the two vectors

    PG_RETURN_INT64(distance);
}


PG_FUNCTION_INFO_V1(c_distance_square_float4);
/****************************************************************************************************
 * Counts square distance of two vectors.
 * @param elements1 int4[]
 * @param elements2 int4[]
 */
Datum 
c_distance_square_float4(PG_FUNCTION_ARGS) {
    ArrayType*   vector1 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(0)));
    ArrayType*   vector2 = (ArrayType *) DatumGetPointer(PG_DETOAST_DATUM(PG_GETARG_DATUM(1)));
    
    int32        length = ArrayGetNItems(ARR_NDIM(vector1), ARR_DIMS(vector1));   // array lengths
    if (length != ArrayGetNItems(ARR_NDIM(vector2), ARR_DIMS(vector2))) {
        ereport(ERROR, (errcode(ERRCODE_CARDINALITY_VIOLATION),
                        errmsg("both arrays must be of the same size")));
    }
    
    float4*     ptr1 = (float4*) ARR_DATA_PTR(vector1);         // array data pointers
    float4*     ptr2 = (float4*) ARR_DATA_PTR(vector2);
    int32        pos = 0;            // array position
    float8      distance = 0;       // result

    #ifdef _DEBUG
        ereport(NOTICE, (111111, errmsg("c_distance_square_float4 length: %d", length)));
    #endif
    
    // Euclidean distance without sqrt() normalization (square distance):
    // d(v1, v2) = Sum[ (v1i - v2i)^2 ]
    //              i
    //
    // go through the two vectors
    for (pos = 0; pos < length; pos++) {
        float8 diff = ptr1[pos] - ptr2[pos];
        distance += (diff * diff);
        #ifdef _DEBUG
            ereport(NOTICE, (111111, errmsg("c_distance_square_float4 pos: %d (%f, %f), diff: %f^2=%f, distance: %f", length, ptr1[pos], ptr2[pos], diff, (diff*diff), distance)));
        #endif
    } // go through the two vectors

    PG_RETURN_FLOAT8(distance);
}


