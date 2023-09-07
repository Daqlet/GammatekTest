#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#include "device_info.pb.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;


int main(void)
{
	const char IP_SERV[] = "127.0.0.1";
	const int PORT_NUM = 8888;
	const short BUFF_SIZE = 1024;

	int erStat;

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);
	
	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}
	
	WSADATA wsData;
		
	erStat = WSAStartup(MAKEWORD(2,2), &wsData);
	
	if ( erStat != 0 ) {
		cout << "Error WinSock version initializaion #";
		cout << WSAGetLastError();
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;
	
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl; 
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));
				
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;	
	servInfo.sin_port = htons(PORT_NUM);

	erStat = ::bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if ( erStat != 0 ) {
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else 
		cout << "Binding socket to Server info is OK" << endl;
	
	erStat = listen(ServSock, SOMAXCONN);

	if ( erStat != 0 ) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listening..." << endl;
	}

	sockaddr_in clientInfo; 
	ZeroMemory(&clientInfo, sizeof(clientInfo));

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Connection to a client established successfully" << endl;
		char clientIP[22];

		inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);

		cout << "Client connected with IP address " << clientIP << endl;

	}

	vector<char> servBuff(BUFF_SIZE);
	short packet_size = 0;
	
	while (true) {
		packet_size = ::recv(ClientConn, servBuff.data(), servBuff.size(), 0);
		cout << "Client's message: " << servBuff.data() << endl; 

    if (string(servBuff.data(), 13) == "GetDeviceInfo") {
      DeviceInfo device_info;
      device_info.set_device_name("MyDevice");
      device_info.set_os_version("Linux");
      device_info.set_serial_number("12345");
      device_info.set_description("Embedded Device");

      string serialized_data;
      device_info.SerializeToString(&serialized_data);
      packet_size = send(ClientConn, serialized_data.c_str(), serialized_data.size(), 0);

      if (packet_size == SOCKET_ERROR) {
        cout << "Can't send message to Client. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        closesocket(ClientConn);
        WSACleanup();
        return 1;
      }
    } else {
      cout << "Unknown command from host: " << string(servBuff.begin(), servBuff.end()) << endl;
    }

	}

	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();

	return 0;

}