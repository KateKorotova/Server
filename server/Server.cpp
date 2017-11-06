#include <iostream>
#include "Winsock2.h"


#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(){
	// initialize socket
	WSADATA wsData; 
	WORD ver = MAKEWORD(2, 2);
	int isOk = WSAStartup(ver, &wsData);
	if (isOk != 0) {
		cerr << "Can't initialize" << endl;
		return 1;
	}

	// create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listening = INVALID_SOCKET) {
		cerr << "Can't create" << endl; 
		return 2;
	}

	// bind an ip adress and port to the socket 

	sockaddr_in hint; 
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (sockaddr*)&hint, sizeof(hint));
	
	// tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// wait for connection 

	sockaddr_in client; 
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);





}