// server
#include <iostream>
#include <string.h> 
#include <sys/time.h>
#include <gmpxx.h>
#include "OT.h"
#include "rsa.h"
#include "tcpip.h"

using namespace std; 

int main(int argc, char *argv[]) 
{	if(argc < 2){
		cout << "Usage: " << argv[0] << " <port>" << endl;
		return -1;
	}
	cout << "Bob started, waiting for Alice" << endl;
	int connfd;	
	int port = atoi(argv[1]);
	connfd = server_init(port);
	if (connfd == -1)
		return -1;
	
	srand(time(NULL));
	
	// private
	int b;
	mpz_t mb;
	// public 
	mpz_t N, e;
	
	// RSA params
	// $$$ get N and e from Alice 
	mpz_init2 (N, 2*PQ_BITS);
	mpz_init2 (e, 2*PQ_BITS);
	recv_bgInt(connfd, N);
	recv_bgInt(connfd, e);
	cout << "N: " << endl << N << endl << "e: " << endl << e << endl ;
	
	mpz_init2 (mb, 2);
	
	struct timeval tp;
	double t0, t1;
	gettimeofday(&tp, NULL);
	t0 = (tp.tv_sec)*1000 + (tp.tv_usec)/1000;

	for (int n_ot = 0; n_ot < 80; n_ot++){
	
		cout << endl << "nOT: " << n_ot << endl;
		// choose b 
		b = rand()%2;
		cout << "Choice: " <<  b << endl;
		
		OT_receive(connfd, mb, b, N, e);
				
		cout << "Message received: " << endl << mb << endl; 	
	}	
	
	gettimeofday(&tp, NULL);
	t1 = (tp.tv_sec)*1000 + ((double)tp.tv_usec)/1000;
	cout << endl << "Total time: \t" << (t1 - t0) << " ms" << endl;
	
	close(connfd); // close connection
	
	return 0;
} 




