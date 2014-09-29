// client
#include <iostream>
#include <string.h> 
#include <gmpxx.h>
#include <sys/time.h>
#include "OT.h"
#include "rsa.h"
#include "tcpip.h"

using namespace std; 

int main(int argc, char *argv[]) {	
	int sockfd;
	if(argc != 3){
		cout << "Usage: " << argv[0] << "<ip of server> <port>" << endl;
		return -1;
	} 	
	sockfd = client_init(argv[1],atoi(argv[2]));
	if ( sockfd == -1)
		return -1;	
	
	// initialize rng 
	srand(time(NULL));
	gmp_randstate_t state;		

	int NO_P = 8;
	int j;
	// generate choice vector b
	char b[NO_P];
	for (j = 0; j < NO_P; j++){
		b[j] = rand()%2;
	}
	// message to get
	//mpz_t  mb[NO_P];
	mpz_t *mb = new mpz_t[NO_P];
	for (j = 0; j < NO_P; j++){
		mpz_init2 (mb[j], M_LEN);	
	}
#if 1 // def DEBUG_TIME	
	struct timeval tp;
	double t0, t1;
	gettimeofday(&tp, NULL);
	t0 = (tp.tv_sec)*1000 + (tp.tv_usec)/1000;
#endif
	OT_ext_receiver(sockfd, mb, b, NO_P);
#if 1 // def DEBUG_TIME	
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << endl << "Total time: \t" << (t1 - t0) << " ms" << endl;
#endif		
	
	cout << std::hex;
	for (j = 0; j < NO_P; j++){
#ifdef DEBUG 
		cout << "pair " << j << ":\t\tchoice: " << (int)b[j]  << endl;
		cout << "Message received: " << mb[j] << endl << endl;
#endif
	}
	// verify
	mpz_t mr;
	mpz_init2(mr, M_LEN);
	int mismatch = 0;
	cout << "Mismatch: ";
	for (j = 0; j < NO_P; j++){
		write(sockfd, (b+j), sizeof(char));
		recv_bgInt(sockfd, mr);
		if (mpz_cmp(mb[j], mr)) {
			mismatch = 1;
			cout << j << " ";
			}
	}
	if(!mismatch) cout << "none";
	cout <<  endl;
	
	return 0;
} 
