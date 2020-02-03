#pragma once
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "WS2_32.lib")
#pragma once

namespace WinSockets
{
#define DEFAULT_BUFF 1024
#define MAXPORTLEN 8

	class WinSocket
	{
	protected:
		struct addrinfo* socketInfoAddr = NULL, * ptr = NULL, socketInfo;
		SOCKET Socket = INVALID_SOCKET;
		void closeSocket();
	public:
		void initialiseSocket(int family, int socketType, IPPROTO protocol, int flags, char* port);
	};

	class ListeningWinSocket : public WinSocket
	{
	public:
		SOCKET getSocket();
		void createSocket();
		void bindSocket();
		void setToListen();
		~ListeningWinSocket();
	};

	class ConnectionWinSocket : public WinSocket
	{
	private:
		char recvBuff[DEFAULT_BUFF] = "";
	public:
		void connectSocket();
		void acceptConn(SOCKET ListeningSocket);
		int acceptBytes();
		char* getBuffer();
		~ConnectionWinSocket();
	};
}