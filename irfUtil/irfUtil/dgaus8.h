#ifndef dgaus8_h
#define dgaus8_h

typedef double (*D_fp)(double*);    // "from" f2c.h

extern "C" {
   int dgaus8_(D_fp fun, double *a, double *b, 
               double *err, double *ans, long *ierr);
}

#endif // dgaus8_h
