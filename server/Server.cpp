#include <iostream>
#include "Winsock2.h"
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

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
	if (listening == INVALID_SOCKET) {
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

	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXHOST]; // service (i.e. port) the client is connect to

	ZeroMemory(service, NI_MAXSERV);
	ZeroMemory(host, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		cout << host << " connected on port " << service << endl;
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port)<< endl;

	}

	// close listening socket
	closesocket(listening);

	// while loop: accept and echo message back to client
	char buf[4096];
	vector <string>origtxt; 
	ifstream data("text.txt");
	while (!(data.eof())) {
		string tmp;
		getline(data,tmp); 
		origtxt.push_back(tmp);
	}
	data.close();
	struct Mess {
		int num;
		string mes;
	};

	vector <Mess> chtxt;

	while (true) {
		ZeroMemory(buf, 4096);
		// wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv()" << endl;
			break;
		}
		if (bytesReceived == 0) {
			cout << "Client disconneced" << endl;
			break;
		}
		// echo message back to client
		ifstream file("text.txt");
		int i = 0; 
		while (!(file.eof())) {
			string tmp;
			getline(file, tmp);
			if (i >= origtxt.size()) {
				origtxt.push_back(tmp);
				Mess mess;
				mess.num = i;
				mess.mes = tmp;
				chtxt.push_back(mess);
			}
			else {
				if (origtxt[i] != tmp) {
					origtxt[i] = tmp;
					Mess mess;
					mess.num = i;
					mess.mes = tmp;
					chtxt.push_back(mess);
				}
			}
			i++;
		}
		file.close();
		string curmess;
		if (chtxt.size() > 0) {
			curmess = to_string(chtxt[0].num) + "," + chtxt[0].mes;
			chtxt.erase(chtxt.begin(), chtxt.begin()+1);
		}
		else
			curmess = "No changes";
		send(clientSocket, curmess.c_str(),curmess.size() + 1, 0);

	}
	// close the socket 
	closesocket(clientSocket);

	//cleanup winsock
	WSACleanup();
	
}