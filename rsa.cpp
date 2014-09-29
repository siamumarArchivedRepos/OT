#include <gmpxx.h>
#include <iostream>
#include <ctime>
#include "rsa.h"

using namespace std; 

void setup_rng(gmp_randstate_t state){
	mpz_t a;
	int L = (PQ_BITS)/(sizeof(int)*8);	
	mpz_init2 (a, PQ_BITS/2);
	mpz_set_ui (a, rand());
	for (int i = 1; i < L; i++)
		mpz_mul_ui (a, a, rand());
	gmp_randinit_lc_2exp (state, a, rand(), PQ_BITS);	
	//gmp_randinit_mt(state);	
}

void generate_rsa_keys(mpz_t n, mpz_t e, mpz_t d){
	/*** initialize rng ***/
	//srand(time(NULL));
	gmp_randstate_t state;
	setup_rng(state);
	
	/*** find prime factors p and q ***/
	mpz_t seed1, seed2, p, p_1, q, q_1;
	int is_prime, iter;	
			
	mpz_init2 (seed1, PQ_BITS/2);
	mpz_init2 (seed2, PQ_BITS/2);
	mpz_init2 (p, PQ_BITS);
	mpz_init2 (p_1, PQ_BITS);
	mpz_init2 (q, PQ_BITS);
	mpz_init2 (q_1, PQ_BITS);		
	
	iter = 0;
	is_prime = 0;
	while (!is_prime){
		iter++;
		if (iter == MAX_ITER) {
			cout << "\nERROR: Maximum number of iteration reached while generating P for RSA\n" << endl;
			return;
		}
		mpz_urandomb (seed1, state, PQ_BITS/2); 
		mpz_clrbit (seed1, 0);
		mpz_urandomb (seed2, state, PQ_BITS/2); 
		mpz_clrbit (seed2, 0);	
		if (!mpz_cmp (seed1, seed2)){
#ifdef DEBUG_RSA		
			cout << "Both seeds same, restarting RNG" << endl;	
#endif			
			setup_rng(state);
			continue;
		}
		mpz_mul (p_1, seed1, seed2);
		mpz_add_ui (p, p_1, 1);
		is_prime = mpz_probab_prime_p (p, 25);
	}
#ifdef DEBUG_RSA
	cout << "\nNo of iterations:" << iter << endl << "p = \t" << p << endl;	
#endif

	iter = 0;
	is_prime = 0;
	while (!is_prime || !mpz_cmp (p, q)){
		iter++;
		if (iter == MAX_ITER) {
			cout << "\nERROR: Maximum number of iteration reached while generating Q for RSA\n" << endl;
			return;
		}
		mpz_urandomb (seed1, state, PQ_BITS/2);  
		mpz_clrbit (seed1, 0);
		mpz_urandomb (seed2, state, PQ_BITS/2);  
		mpz_clrbit (seed2, 0);	
		if (!mpz_cmp (seed1, seed2)){
#ifdef DEBUG_RSA
			cout << "Both seeds same, restarting RNG" << endl;		
#endif			
			setup_rng(state);
			continue;
		}
		mpz_mul (q_1, seed1, seed2);
		mpz_add_ui (q, q_1, 1);
		is_prime = mpz_probab_prime_p (q, 25);
	}
#ifdef DEBUG_RSA
	cout << "\nNo of iterations:" << iter << endl << "q = \t" << q << endl;	
#endif
	
	/*** find n and phi ***/
	mpz_t phi;	
	//mpz_init2 (n, 2*PQ_BITS);	
	mpz_init2 (phi, 2*PQ_BITS);
	mpz_mul (n, p, q);
	mpz_sub_ui (p_1, p, 1);
	mpz_sub_ui (q_1, q, 1);
	mpz_mul (phi, p_1, q_1);
#ifdef DEBUG_RSA
	cout << "\nn =\t" << n << endl << "phi =\t" << phi << endl;
#endif
	
	/*** find e **///
	mpz_t e_bak, gcd_e_phi;		
	//mpz_init2 (e, E_BITS);
	mpz_init2 (e_bak, E_BITS);
	mpz_init2 (gcd_e_phi, E_BITS);
	
	iter = 0;
	do{
		iter++;
		if (iter == MAX_ITER) {
			cout << "ERROR: Maximum number of iteration reached while generating E for RSA" << endl;
			return;
		}		
		mpz_urandomb (e, state, E_BITS); 
		mpz_setbit (e, 0);
		if (!mpz_cmp (e, e_bak)){
#ifdef DEBUG_RSA
			cout << "Both seeds same, restarting RNG" << endl;
#endif			
			setup_rng(state);
			continue;
		}
		mpz_gcd (gcd_e_phi, e, phi);
		mpz_set (e_bak, e);
	}while(mpz_cmp_ui(gcd_e_phi, 1));
#ifdef DEBUG_RSA
	cout << "\nNo of iterations:" << iter << endl << "e = \t" << e << endl;	
#endif

	/*** find d ***/
	mpz_t  r1, r0, temp_r0, s1, s0, temp_s0, t1, t0, temp_t0, qr0r1, qr1, qs1, qt1;
	//mpz_init2 (d, D_BITS);
	mpz_init2 (r1, PQ_BITS);	
	mpz_init2 (r0, PQ_BITS);	
	mpz_init2 (temp_r0, PQ_BITS);	
	mpz_init2 (qr1, PQ_BITS);	
	mpz_init2 (s1, D_BITS);	
	mpz_init2 (s0, D_BITS);	
	mpz_init2 (temp_s0, D_BITS);	
	mpz_init2 (qs1, PQ_BITS);	
	mpz_init2 (t1, D_BITS);	
	mpz_init2 (t0, D_BITS);	
	mpz_init2 (temp_t0, D_BITS);	
	mpz_init2 (qt1, PQ_BITS);	
	mpz_init2 (qr0r1, PQ_BITS);	
	
	iter = 0;
	mpz_set (r1, phi);
	mpz_set (r0, e);
	mpz_set_ui (s1, 0);
	mpz_set_ui (s0, 1);
	mpz_set_ui (t1, 1);
	mpz_set_ui (t0, 0);
	while(mpz_cmp_ui(r1, 0)){
		iter++;
		if (iter == MAX_ITER) {
			cout << "ERROR: Maximum number of iteration reached while generating E for RSA" << endl;
			return;
		}
		mpz_fdiv_q (qr0r1, r0, r1);
		
		mpz_set (temp_r0, r0);
		mpz_set (r0, r1);
		mpz_mul (qr1, qr0r1, r1);
		mpz_sub (r1, temp_r0, qr1);
		
		mpz_set (temp_s0, s0);
		mpz_set (s0, s1);
		mpz_mul (qs1, qr0r1, s1);
		mpz_sub (s1, temp_s0, qs1);

		mpz_set (temp_t0, t0);
		mpz_set (t0, t1);
		mpz_mul (qt1, qr0r1, t1);
		mpz_sub (t1, temp_t0, qt1);
	}
	mpz_mod (d, s0, phi);	
#ifdef DEBUG_RSA
	cout << "\nNo of iterations:" << iter << endl << "d = \t" << d << endl << endl;		 	
#endif
}