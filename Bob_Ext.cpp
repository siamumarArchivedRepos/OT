// server
#include <iostream>
#include <string.h> 
#include <sys/time.h>
#include <gmpxx.h>
#include "OT.h"
#include "rsa.h"
#include "tcpip.h"

using namespace std; 

int main(int argc, char *argv[]){	
	int connfd;
	if(argc < 2){
		cout << "Usage: " << argv[0] << " <port>" << endl;
		return -1;
	}
	int port = atoi(argv[1]);
	cout << "Bob started, waiting for Alice" << endl;	
	connfd = server_init(port);
	if (connfd == -1) 
		return -1;
	
	srand(time(NULL));
	gmp_randstate_t state;
	
	int NO_P = 8;
	int j;
	// generate messages
	//mpz_t m0[NO_P], m1[NO_P];
	mpz_t *m0 = new mpz_t[NO_P];
	mpz_t *m1 = new mpz_t[NO_P];
	
	cout << std::hex;	
	for (j = 0; j < NO_P; j++){
		mpz_init2 (m0[j], M_LEN);
		mpz_init2 (m1[j], M_LEN);
		setup_rng(state);
		mpz_urandomb (m0[j], state, M_LEN);
		mpz_urandomb (m1[j], state, M_LEN);
#ifdef DEBUG 
		cout << "pair " << j << endl ;
		cout << "Message 0: " << m0[j] << endl;
		cout << "Message 1: " << m1[j] << endl;
		cout << endl;
#endif
	}
	cout << std::dec;
#if 1 // def DEBUG_TIME	
	struct timeval tp;
	double t0, t1;
	gettimeofday(&tp, NULL);
	t0 = (tp.tv_sec)*1000 + (tp.tv_usec)/1000;
#endif	
	OT_ext_sender(connfd, m0, m1, NO_P);
#if 1 // def DEBUG_TIME	
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << endl << "Total time: \t" << (t1 - t0) << " ms" << endl;
#endif
	// verify
	char b;
	for (j = 0; j < NO_P; j++){
		read(connfd, &b, sizeof(char));
		if (b) send_bgInt(connfd, m1[j]);
		else send_bgInt(connfd, m0[j]);
	}
	
	close(connfd); // close connection
	
return 0;
} 
