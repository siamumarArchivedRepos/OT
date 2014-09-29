#include <iostream>
#include <string.h> 
#include <sys/time.h>
#include <gmpxx.h>
#include "OT.h"
#include "rsa.h"
#include "tcpip.h"

using namespace std; 

void OT_ext_sender(int sock, mpz_t *m0, mpz_t *m1, int NO_P){	
#ifdef DEBUG_TIME	
	struct timeval tp;
	double t0, t1;
#endif
	int i, j;
	gmp_randstate_t state;
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t0 = (tp.tv_sec)*1000 + (tp.tv_usec)/1000;
	write(sock, &t0, sizeof(double)/sizeof(char));
#endif	
	// generate s
	mpz_t bs, bs1;
	mpz_init2(bs, SEC_K);
	mpz_init2(bs1, SEC_K+1); // for an extra '1' bit at LSB
	setup_rng(state);
	mpz_urandomb (bs, state, SEC_K);
	mpz_set(bs1, bs);
	mpz_setbit(bs1, SEC_K);
	char *s = (char *) mpz_export (NULL, NULL, -1, 1, 1, 7, bs1);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated s\t\t" << (t1 - t0) << " ms" << endl;
#endif			
	// RSA params
	mpz_t n, e;
	mpz_init2 (n, 2*PQ_BITS);
	mpz_init2 (e, 2*PQ_BITS);
	recv_bgInt(sock, n );
	recv_bgInt(sock, e );	
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "received RSA params\t" << (t1 - t0) << " ms" << endl;
#endif	
	mpz_t Q[SEC_K];	
	for (i = 0; i < SEC_K; i++){
#ifdef DEBUG_TIME	
		cout << endl << "OT: " << i << endl;
#endif		
		mpz_init2 (Q[i], NO_P);
		OT_receive(sock, Q[i], s[i], n, e); 
	}
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << endl << "performed OT\t\t" << (t1 - t0) << " ms" << endl;
#endif	
	mpz_t *Q1 = new mpz_t[NO_P];
	for (j = 0; j < NO_P; j++){
		mpz_init2 (Q1[j], SEC_K);
	}
	col2row(Q1, Q, NO_P);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated row matrix\t" << (t1 - t0) << " ms" << endl;
#endif	
	// make and send ys
	mpz_t y0, y1, h0, h1, sxQ; 	
	mpz_init2 (y0, M_LEN);
	mpz_init2 (y1, M_LEN);	
	mpz_init2 (h0, M_LEN);
	mpz_init2 (h1, M_LEN);
	mpz_init2(sxQ, SEC_K);
	
	for (j = 0; j < NO_P; j++){
		H(h0, j, Q1[NO_P-1-j]); 
		mpz_xor(y0, m0[j], h0);
		send_bgInt(sock, y0 );
		mpz_xor(sxQ, Q1[NO_P-1-j], bs); 
		H(h1, j, sxQ);
		mpz_xor(y1, m1[j], h1);
		send_bgInt(sock, y1 );
	}
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "sent y0, y1\t\t" << (t1 - t0) << " ms" << endl << endl;
#endif
}

void OT_ext_receiver(int sock, mpz_t *mb, char *b, int NO_P){
#ifdef DEBUG_TIME	
	struct timeval tp;
	double t0, t1;
#endif
	int i, j;
	gmp_randstate_t state;
#ifdef DEBUG_TIME
	read(sock, &t0, sizeof(double)/sizeof(char));
#endif	
	mpz_t bb;
	mpz_init2 (bb, NO_P);
	mpz_import (bb, (NO_P), 1, 1, 1, 7, b);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated bb\t" << (t1 - t0) << " ms" << endl;
#endif	
	// generate T, bxT column matrices
	mpz_t T[SEC_K], bxT[SEC_K]; 	
	for (i = 0; i < SEC_K; i++){
		mpz_init2 (T[i], NO_P); 
		setup_rng(state);
		mpz_urandomb (T[i], state, NO_P); 
	}	
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated T\t" << (t1 - t0) << " ms" << endl;
#endif	
	for (i = 0; i < SEC_K; i++){
		mpz_init2 (bxT[i], NO_P);
		mpz_xor(bxT[i], T[i], bb);
	}
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated bxT\t" << (t1 - t0) << " ms" << endl;
#endif		
	// RSA params
	mpz_t n, e, d;
	mpz_init2 (n, 2*PQ_BITS);
	mpz_init2 (e, E_BITS);
	mpz_init2 (d, D_BITS);
	generate_rsa_keys(n ,e, d);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	double t_rsa = t1;
	cout << "generated RSA params\t" << (t1 - t0) << " ms" << endl;
#endif	
	send_bgInt(sock, n );
	send_bgInt(sock, e );		
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "sent RSA params\t" << (t1 - t0) << " ms" << endl;
#endif	
	for (i = 0; i < SEC_K; i++){
#ifdef DEBUG_TIME	
		cout << endl << "OT: " << i << endl;
#endif	
		OT_send(sock, T[i] ,bxT[i], n, d);
	}
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << endl << "performed OT\t\t" << (t1 - t0) << " ms" << endl;
#endif	
	mpz_t *T1 = new mpz_t[NO_P];
	for (j = 0; j < NO_P; j++){
		mpz_init2 (T1[j], SEC_K);
	}
	col2row(T1, T, NO_P);	
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated row matrix\t" << (t1 - t0) << " ms" << endl;
#endif	
	//get ys and generate mb
	mpz_t  y0, y1, h; 	
	mpz_init2 (y0, M_LEN);	
	mpz_init2 (y1, M_LEN);
	mpz_init2 (h, M_LEN);
	
	for (j = 0; j < NO_P; j++){
		recv_bgInt(sock, y0 );
		recv_bgInt(sock, y1 );	
		H(h, j, T1[NO_P-1-j]); 
		if (b[j]) mpz_xor(mb[j], y1, h);
		else mpz_xor(mb[j], y0, h);
	}
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "completed\t\t" << (t1 - t0) << " ms" << endl;
	cout << "Time without RSA\t" << (t1 - t_rsa) << " ms" << endl;
#endif
}

void OT_send(int sock, mpz_t m0 ,mpz_t m1, mpz_t N, mpz_t d){
#ifdef DEBUG_TIME	
	struct timeval tp;
	double t0, t1;
#endif
	// initialize rng 
	gmp_randstate_t state;
	
	// private
	mpz_t k0, k1; 
	// public 
	mpz_t m0_prime, m1_prime, x0, x1, v, en_k, v_x0, v_x1;
#ifdef DEBUG_TIME	
	gettimeofday(&tp, NULL);
	t0 = (tp.tv_sec)*1000 + (tp.tv_usec)/1000;
	write(sock, &t0, sizeof(double)/sizeof(char));
#endif
	// generate two random messages
	mpz_init2 (x0, 2*PQ_BITS);
	mpz_init2 (x1, 2*PQ_BITS);
	setup_rng(state);
	mpz_urandomb (x0, state, 2*PQ_BITS);
	mpz_urandomb (x1, state, 2*PQ_BITS);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated x0, x1\t" << (t1 - t0) << " ms" << endl;
#endif
#ifdef DEBUG_OT
	cout << "Alice side, x0 = " << endl << x0 << endl;
	cout << "Alice side, x1 = " << endl << x1 << endl;	
#endif
	// send x0 and x1 to Bob	
	send_bgInt(sock, x0 ); 
	send_bgInt(sock, x1 ); 	
#ifdef DEBUG_OT
	cout << "Done: sent two random messages" << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "sent x0, x1\t\t" << (t1 - t0) << " ms" << endl;
#endif
	
	// get v from Bob	
	mpz_init2 (v, 2*PQ_BITS);
	recv_bgInt(sock, v );
#ifdef DEBUG_OT
	if(DEBUG_OT) cout << "Alice side, v = " << endl << v << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "received v\t\t" << (t1 - t0) << " ms" << endl;
#endif
		
	// decryption on Alice's side
	mpz_init2 (v_x0, 2*PQ_BITS);
	mpz_init2 (v_x1, 2*PQ_BITS);
	mpz_sub(v_x0, v, x0);
	mpz_sub(v_x1, v, x1);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "Calculated (v-x)\t" << (t1 - t0) << " ms" << endl;
#endif
	
	// one of these will equal k, but Alice does not know which
	mpz_init2 (k0, 2*PQ_BITS);
	mpz_init2 (k1, 2*PQ_BITS);
	mpz_powm (k0, v_x0, d, N);
	mpz_powm (k1, v_x1, d, N);
#ifdef DEBUG_OT
	cout << "Done: generate k0 and k1 on Alice's side" << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generate k0, k1\t\t" << (t1 - t0) << " ms" << endl;
#endif
	
	// generate m_primes
	mpz_init2 (m0_prime, 2*PQ_BITS);
	mpz_init2 (m1_prime, 2*PQ_BITS);
	mpz_add(m0_prime, m0, k0);
	mpz_add(m1_prime, m1, k1);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated m0', m1'\t" << (t1 - t0) << " ms" << endl;
#endif
	
	// send m0_prime and m1_prime to Bob
#ifdef DEBUG_OT
	if(DEBUG_OT)cout << "Alice side: m0_prime = " << endl << m0_prime << endl;
	if(DEBUG_OT)cout << "Alice side: m1_prime = " << endl << m1_prime << endl;
#endif
	send_bgInt(sock, m0_prime ); 
	send_bgInt(sock, m1_prime ); 
#ifdef DEBUG_OT	
	cout << "Done: sent m0_prime and m1_prime to Bob" << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "sent m0', m1'\t\t" << (t1 - t0) << " ms" << endl;
#endif
}

void OT_receive(int sock, mpz_t mb, int b, mpz_t N, mpz_t e){	
#ifdef DEBUG_TIME	
	struct timeval tp;
	double t0, t1;
#endif
	// initialize rng 
	gmp_randstate_t state;
	
	// private
	mpz_t xb, k;
	// public 
	mpz_t m0_prime, m1_prime, x0, x1, v, en_k;
#ifdef DEBUG_TIME		
	read(sock, &t0, sizeof(double)/sizeof(char));
#endif		
	// make random k
	mpz_init2 (k, PQ_BITS);
	setup_rng(state);
	mpz_urandomb (k, state, PQ_BITS);
#ifdef DEBUG_OT
	cout << "Done: generate random k" << endl;	
#endif
	mpz_init2 (en_k, 2*PQ_BITS);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "generated k\t\t" << (t1 - t0) << " ms" << endl;
#endif
	mpz_powm (en_k, k, e, N);
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "encrypted k\t\t" << (t1 - t0) << " ms" << endl;
#endif
	// get x0 and x1 from Alice 
	mpz_init2 (x0, 2*PQ_BITS);
	mpz_init2 (x1, 2*PQ_BITS);
	recv_bgInt(sock, x0 );
	recv_bgInt(sock, x1 );
#ifdef DEBUG_OT
	if(DEBUG_OT) cout << "x0 on Bob's side: " << endl << x0 << endl;
	if(DEBUG_OT) cout << "x1 on Bob's side: " << endl << x1 << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "received x0, x1\t\t" << (t1 - t0) << " ms" << endl;
#endif
	// chose xb
	mpz_init2 (xb, 2*PQ_BITS);
	if (b) mpz_set(xb, x1);
	else  mpz_set(xb, x0);
#ifdef DEBUG_OT
	cout << "Done: choose x_b" << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "chosen xb\t\t" << (t1 - t0) << " ms" << endl;
#endif
		
	mpz_init2 (v, 2*PQ_BITS);
	mpz_add(v, xb, en_k);
#ifdef DEBUG_OT
	if(DEBUG_OT) cout << "Bob side: v = " << endl << v << endl;	
#endif
	// send v to Alice 
	send_bgInt(sock, v ); 
#ifdef DEBUG_OT	
	cout << "Done: compute the encryption of k, blind with x_b" << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "sent v\t\t\t" << (t1 - t0) << " ms" << endl;
#endif
	
	// get m0_prime and m1_prime from Alice 
	mpz_init2 (m0_prime, 2*PQ_BITS);
	mpz_init2 (m1_prime, 2*PQ_BITS);
	recv_bgInt(sock, m0_prime );
	recv_bgInt(sock, m1_prime );
#ifdef DEBUG_OT
	if(DEBUG_OT) cout << "Bob side: m0_prime = " << endl << m0_prime << endl;
	if(DEBUG_OT) cout << "Bob side: m1_prime = " << endl << m1_prime << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "received m0', m1'\t" << (t1 - t0) << " ms" << endl;
#endif
	// decryption of mb
	if (b) mpz_sub(mb, m1_prime, k);
	else mpz_sub(mb, m0_prime, k);	
#ifdef DEBUG_OT
	cout << "Done: generate mb on Bob's side" << endl;
#endif
#ifdef DEBUG_TIME
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << "completed\t\t" << (t1 - t0) << " ms" << endl;
#endif
}

void col2row(mpz_t *T2, mpz_t *T1, int NO_P){
	int i, j;

	unsigned char *matrix[SEC_K];
	for (i = 0; i < SEC_K; i++){
		matrix[i] = new unsigned char[NO_P/8];
		size_t count = 0;
		memset(matrix[i], 0, NO_P/8);
		mpz_export (matrix[i], &count, -1, 1, 1, 0, T1[i]);
	}
	
	unsigned char temp[SEC_K/8];
	for (j = 0; j < NO_P; j++){
		memset(temp, 0, SEC_K/8);
		for (i = 0; i < SEC_K; i++){
			temp[i/8] |= ((matrix[i][j/8] >> j%8)&1) << i%8;
		}
		mpz_import (T2[j], SEC_K/8, -1, 1, 1, 0, temp);
	}
}

void H(mpz_t h, int j, mpz_t q){
	// length of q is SEC_K and length of h is M_LEN
	mpz_set(h, q);
}