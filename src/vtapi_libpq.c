
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "vtapi_libpq.h"

/* cube type handlers */
int cube_put (PGtypeArgs *args) {

    PGcube *cube = va_arg(args->ap, PGcube *);
    unsigned int * buf = NULL;
    char * out = NULL;
    int len = 0, xcnt = 0, tmp = 0;

    if (!args || !cube) return 0;

    /* expand buffer enough */
    xcnt = cube->dim * 2;
    len = (2 * sizeof(int)) + (xcnt * sizeof(double));
    if (args->put.expandBuffer(args, len) == -1) return -1;

    /* put header - varlena and dimensions count */
    out = args->put.out;
    tmp = htonl(cube->vl_len);
    memcpy(out, &tmp, sizeof(int));
    out += sizeof(int);
    tmp = htonl(cube->dim);
    memcpy(out, &tmp, sizeof(int));
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
    int xcnt = (len - 2 * sizeof(int)) / sizeof(double); /* coordinates count */
    double *xp = (double *) (val + 2 * sizeof(int)); /* pointer to coordinates */

    PGcube *cube = va_arg(args->ap, PGcube *);
    pq_swap4(&cube->vl_len, val, 1); /* varlena header */
    pq_swap4(&cube->dim, val + sizeof(int), 1); /* number of dimensions */

    /* allocate and extract coordinates */
    if (xcnt == 0) cube->x = 0;
    else {
        cube->x = (double *) PQresultAlloc((PGresult *) args->get.result, xcnt * sizeof(double));
        if (!cube->x) return 0;
        for (int i = 0; i < xcnt; i++) {
            pq_swap8(&cube->x[i], xp++, 1);
        }
    }   
    return 0;
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
