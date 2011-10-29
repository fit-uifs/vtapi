/* 
 * File:   vt-print.h
 * Author: chmelarp
 *
 * Created on 29. říjen 2011, 1:34
 */

#ifndef VT_PRINT_H
#define	VT_PRINT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <postgresql/libpq-fe.h>

/*
 * vtPQprint()
 *
 * Format results of a query for printing.
 *
 * PQprintOpt is a typedef (structure) that containes
 * various flags and options. consult libpq-fe.h for
 * details
 *
 * This function was copied here because in fe-print.c there is:
 * This function should probably be removed sometime since psql
 * doesn't use it anymore. It is unclear to what extent this is used
 * by external clients, however.
 */
void
vtPQprint(FILE *fout, const PGresult *res, const PQprintOpt *po, const int pTuple);

static char *
do_header(FILE *fout, const PQprintOpt *po, const int nFields, int *fieldMax,
		  const char **fieldNames, unsigned char *fieldNotNum,
		  const int fs_len, const PGresult *res);

static void
do_field(const PQprintOpt *po, const PGresult *res,
		 const int i, const int j, const int fs_len,
		 char **fields,
		 const int nFields, char const ** fieldNames,
		 unsigned char *fieldNotNum, int *fieldMax,
		 const int fieldMaxLen, FILE *fout);

#ifdef	__cplusplus
}
#endif

#endif	/* VT_PRINT_H */

