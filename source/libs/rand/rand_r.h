#ifndef rand_rH
#define rand_rH

//! returns a 31 bit random number
extern int rand_r (unsigned int *seed);

const int rand_r_max = (1 << 30) - 1;


#endif
