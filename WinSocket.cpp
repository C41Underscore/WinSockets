#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include "WinSocket.h"

#pragma comment(lib, "WS2_32.lib")

//Using WinSock Version 2.2
//A namespace to create a manage sockets for Windows OS, using the WinSock libraries.
//Each class is designed to encapsulate a socket, meaning the use of sockets is less procedural in application, and nearly all operations are invoked va these classes.
//WinSock itself is not initialised from these classes, nor is it cleaned up from these classes either.

#define DEFAULT_BUFF 1024
#define MAXPORTLEN 8

void WinSockets::WinSocket::closeSocket() { closesocket(this->Socket); }

void WinSockets::WinSocket::initialiseSocket(int family, int socketType, IPPROTO protocol, int flags, char* port)
{
	ZeroMemory(&socketInfo, sizeof(socketInfo));
	this->socketInfo.ai_family = family;
	this->socketInfo.ai_socktype = socketType;
	this->socketInfo.ai_protocol = protocol;
	this->socketInfo.ai_flags = flags;
	int Result = getaddrinfo(NULL, port, &this->socketInfo, &socketInfoAddr);
	//Result of address creation next
	if (Result != 0)
	{
		std::cout << "getaddrinfo function failed on error code " << Result << std::endl;
		this->~WinSocket();
	}
}

SOCKET WinSockets::ListeningWinSocket::getSocket() { return this->Socket; }

void WinSockets::ListeningWinSocket::createSocket()
{
	this->Socket = socket(this->socketInfoAddr->ai_family, this->socketInfoAddr->ai_socktype, this->socketInfoAddr->ai_protocol);
	if (this->Socket == INVALID_SOCKET)
	{
		std::cout << "Creation of ListeningWinSocket object failed: " << WSAGetLastError() << std::endl;
		this->~ListeningWinSocket();
	}
}

void WinSockets::ListeningWinSocket::bindSocket()
{
	int bindResult = bind(this->Socket, this->socketInfoAddr->ai_addr, (int)this->socketInfoAddr->ai_addrlen);
	if (bindResult == SOCKET_ERROR)
	{
		std::cout << "ListeningWinSocket bind failed: " << WSAGetLastError() << std::endl;
		this->closeSocket();
		this->~ListeningWinSocket();
	}
	freeaddrinfo(this->socketInfoAddr);
}

void WinSockets::ListeningWinSocket::setToListen()
{
	if (listen(this->Socket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Socket listen failed: " << WSAGetLastError() << std::endl;
		this->closeSocket();
		this->~ListeningWinSocket();
	}
	else
		std::cout << "ListeningWinSocket object listening on socket " << this->socketInfoAddr->ai_addr << std::endl;
}

WinSockets::ListeningWinSocket::~ListeningWinSocket()
{
	freeaddrinfo(socketInfoAddr);
	std::cout << "Detroyed ListeningWinSocket" << std::endl;
}

void WinSockets::ConnectionWinSocket::connectSocket()
{
	int cResult = connect(this->Socket, this->socketInfoAddr->ai_addr, (int)this->socketInfoAddr->ai_addrlen);
	if (cResult == INVALID_SOCKET || cResult == SOCKET_ERROR)
	{
		std::cout << "Error connecting socket:" << WSAGetLastError() << std::endl;
		this->closeSocket();
	}
}

void WinSockets::ConnectionWinSocket::acceptConn(SOCKET ListeningSocket)
{
	this->Socket = accept(ListeningSocket, NULL, NULL);
	if (this->Socket == INVALID_SOCKET)
	{
		std::cout << "Client connection failed: " << WSAGetLastError() << std::endl;
		this->closeSocket();
		this->~ConnectionWinSocket();
	}
}

int WinSockets::ConnectionWinSocket::acceptBytes()
{
	int recvResult = recv(this->Socket, recvBuff, DEFAULT_BUFF, 0);
	return recvResult;
}

char* WinSockets::ConnectionWinSocket::getBuffer()
{
	return this->recvBuff;
}

WinSockets::ConnectionWinSocket::~ConnectionWinSocket()
{
	freeaddrinfo(socketInfoAddr);
	std::cout << "Detroyed ConnectionWinSocket" << std::endl;
}