.PHONY: Alice Bob Alice_Ext Bob_Ext rsa_test

all: 
	g++ -g -o Alice Alice.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp
	g++ -g -o Bob Bob.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp
	g++ -g -o Alice_Ext Alice_Ext.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp
	g++ -g -o Bob_Ext Bob_Ext.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp
	g++ -g -o rsa_test rsa_test.cpp rsa.cpp rsa.h -lgmpxx -lgmp
	
single: 
	g++ -g -o Alice Alice.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp
	g++ -g -o Bob Bob.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp

ext: 
	g++ -g -o Alice_Ext Alice_Ext.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp
	g++ -g -o Bob_Ext Bob_Ext.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp
 
 
ext_debgug: 
	g++ -g -o Alice_Ext Alice_Ext.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp -O0
	g++ -g -o Bob_Ext Bob_Ext.cpp rsa.cpp rsa.h OT.cpp OT.h tcpip.cpp tcpip.h -lgmpxx -lgmp -O0
 
rsa_test: 
	g++ -g -o rsa_test rsa_test.cpp rsa.cpp rsa.h -lgmpxx -lgmp
