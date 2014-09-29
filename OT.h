#ifndef OT
#define OT

#include <gmpxx.h>

//#define NO_P 1024
#define M_LEN 80
#define SEC_K 80

//#define DEBUG_OT 
//#define DEBUG 
//#define DEBUG_TIME

void OT_ext_sender(int, mpz_t*, mpz_t*, int);
void OT_ext_receiver(int, mpz_t*, char*, int);
void OT_send(int, mpz_t, mpz_t, mpz_t, mpz_t);
void OT_receive(int, mpz_t, int , mpz_t, mpz_t);
void col2row(mpz_t*, mpz_t*, int);
void H(mpz_t, int, mpz_t);

#endif