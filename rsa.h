#ifndef RSA
#define RSA

#include <gmpxx.h>

#define MAX_ITER 100000
#define PQ_BITS 1024
#define E_BITS 64
#define D_BITS (2*PQ_BITS - E_BITS)

#define DEBUG_RSA 

void generate_rsa_keys(mpz_t, mpz_t, mpz_t);
void setup_rng(gmp_randstate_t);

#endif