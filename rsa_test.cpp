#include <sys/time.h>
#include <signal.h>
#include <gmpxx.h>
#include <iostream>
#include "rsa.h"

using namespace std; 

volatile int sigcount=0;
void catcher( int sig ) {
    sigcount++;
}

int main(){
	struct itimerval value;
    int which = ITIMER_REAL;

    struct sigaction sact;
    volatile double count;

    sigemptyset( &sact.sa_mask );
    sact.sa_flags = 0;
    sact.sa_handler = catcher;
    sigaction( SIGALRM, &sact, NULL );

    value.it_interval.tv_sec = 0;        /* Zero seconds */
    value.it_interval.tv_usec = 100;  /* Two hundred milliseconds */
    value.it_value.tv_sec = 0;           /* Zero seconds */
    value.it_value.tv_usec = 100;     /* Two hundred milliseconds */

    setitimer( which, &value, NULL );

	mpz_t n, e, d;
	mpz_init2 (n, 2*PQ_BITS);
	mpz_init2 (e, E_BITS);
	mpz_init2 (d, D_BITS);
	
	srand(time(NULL));
	
	generate_rsa_keys(n ,e, d);
	
	cout << "Time taken to generate keys: " <<  sigcount*0.1 << " ms"  << endl;
	
	
	gmp_randstate_t state;
	setup_rng(state);	
	
	mpz_t plain, cipher, decipher;	
	mpz_init2 (plain, E_BITS);
	mpz_init2 (cipher, 2*PQ_BITS);
	mpz_init2 (decipher, 2*PQ_BITS);	
	
	mpz_urandomb (plain, state, E_BITS);
	
	mpz_powm (cipher, plain, e, n);	
	cout << "Time taken to encrypt: " <<  sigcount*0.1 << " ms"  << endl;
	mpz_powm (decipher, cipher, d, n);	
	cout << "Time taken to decrypt: " <<  sigcount*0.1 << " ms"  << endl;
	
	cout << "plain: " << endl << plain << endl << "decipher: " << endl << decipher << endl;
	if (!mpz_cmp(decipher, plain)) cout << "SUCCESS" << endl;
	else cout << "!!FAIL!!" << endl;
}
