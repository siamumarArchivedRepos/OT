// client
#include <iostream>
#include <string.h> 
#include <sys/time.h>
#include <gmpxx.h>
#include "OT.h"
#include "rsa.h"
#include "tcpip.h"

using namespace std; 

int main(int argc, char *argv[]) 
{	
	//client initialization
	if(argc != 3){
		cout << "Usage: " << argv[0] << "<ip of server> <port>" << endl;
		return -1;
	} 
	int sockfd;	
	int port = atoi(argv[2]);
	sockfd = client_init(argv[1], port);
	if ( sockfd == -1)
		return -1;	
	
	/*** initialize rng ***/
	srand(time(NULL));
	gmp_randstate_t state;		
	
	// messages
	mpz_t m0, m1; 
	mpz_init2 (m0, 2);
	mpz_init2 (m1, 2);
	
	// RSA params
	mpz_t n, e, d;
	mpz_init2 (n, 2*PQ_BITS);
	mpz_init2 (e, E_BITS);
	mpz_init2 (d, D_BITS);
	generate_rsa_keys(n ,e, d);
	cout << "n: " << endl << n << endl << "e: " << endl << e << endl << "d: " << endl << d << endl;
	// $$$ send n and e to Bob
	send_bgInt(sockfd, n);
	send_bgInt(sockfd, e);
	
	struct timeval tp;
	double t0, t1;
	gettimeofday(&tp, NULL);
	t0 = (tp.tv_sec)*1000 + (tp.tv_usec)/1000;

	for (int n_ot = 0; n_ot < 80; n_ot++){
	
		cout << endl << "nOT: " << n_ot << endl;
		
		setup_rng(state);
		mpz_urandomb (m0, state, 2);
		mpz_urandomb (m1, state, 2);
		
		cout << "Message 0:" << endl << m0 << endl;
		cout << "Message 1:" << endl << m1 << endl;
		
		OT_send(sockfd, m0 ,m1, n, d);
	}	
	
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << endl << "Total time: \t" << (t1 - t0) << " ms" << endl;

	
	return 0;
} 


