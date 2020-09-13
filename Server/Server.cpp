#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <string>


WSADATA Winsockdata;
int     iWsaStartup;
int     iWsaCleanup;

SOCKET  TCPServerSocket;
int     iCloseSocket;

struct  sockaddr_in TCPServerAdd;
struct  sockaddr_in TCPClientAdd;
int     iTCPClientAdd = sizeof(TCPClientAdd);

char    ipAddr[32] = "127.0.0.1";
int     port = 8000;

int     iBind;

int     iListen;

int     iRecv;
bool    ErrorInSendMessage = false;

SOCKET  sAcceptSocket;


// Functions
int  TCPServerStart();
void AcceptConnection();
void SendMsg(std::string msg);
void ReceiveMsg();
void CloseSocket();
void UDF_WSACleanup();


int main() {
	if (TCPServerStart() == 0) {

		AcceptConnection();

		if (sAcceptSocket != INVALID_SOCKET) {
			while (true) {
				std::string msg;

				std::cout << "--------------------> ";
				std::getline(std::cin, msg);

				SendMsg(msg);
				ReceiveMsg();
			}
			CloseSocket();
			UDF_WSACleanup();
		}
	}

	system("PAUSE");
	return 0;
}


int TCPServerStart() {
	// WSAStartup
	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &Winsockdata);
	if (iWsaStartup != 0) {
		std::cout << "WSAStartUp was Failed!" << std::endl;
	}
	else {
		std::cout << "WSAStartup was successful!" << std::endl;

		// Filling Structure
		TCPServerAdd.sin_family = AF_INET;
		TCPServerAdd.sin_addr.s_addr = inet_addr(ipAddr);
		TCPServerAdd.sin_port = htons(port);

		// Socket Creation
		TCPServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (TCPServerSocket == INVALID_SOCKET) {
			std::cout << "An error occurred while creating TCP socket for server!" << WSAGetLastError() << std::endl;
		}
		else {
			std::cout << "TCP socket Creation was successful!" << std::endl;

			// Binding Socket
			iBind = bind(TCPServerSocket, (SOCKADDR*)&TCPServerAdd, sizeof(TCPServerAdd));
			if (iBind == SOCKET_ERROR) {
				std::cout << "An error occurred while binding socket! Error no: " << WSAGetLastError() << std::endl;
			}
			else {
				std::cout << "Binding socket was successful!" << std::endl;

				// Start Listening
				iListen = listen(TCPServerSocket, 1);
				if (iListen == SOCKET_ERROR) {
					std::cout << "Listen Func was failed! Error no: " << WSAGetLastError() << std::endl;
				}
				else {
					std::cout << "Listen Func was successful!" << std::endl << "Listening on " << ipAddr << ":" << port << std::endl;
					return 0;
				}
			}
		}
	}
}

void AcceptConnection() {
	sAcceptSocket = accept(TCPServerSocket, (SOCKADDR*)&iTCPClientAdd, &iTCPClientAdd);
	if (sAcceptSocket == INVALID_SOCKET) {
		std::cout << "An error occurred while accepting the connection! Error no: " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "Accepted the connection!" << std::endl << "A client has joined!" << std::endl;
	}
}

void SendMsg(std::string msg) {
	char SenderBuffer;

	// Appending "EOT" char to message
	msg += (char)3;
	msg += (char)3;
	msg += (char)3;

	// Sending the message to client
	for (int i = 0; i < msg.size(); i++) {
		SenderBuffer = msg.at(i);
		if (send(sAcceptSocket, &SenderBuffer, sizeof(SenderBuffer), 0) == SOCKET_ERROR) {
			ErrorInSendMessage = true;
			break;
		}
	}

	// Checking if message was sent successfully
	if (ErrorInSendMessage) {
		std::cout << "An error occurred while sending message to client! Error no: " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "Message was sent successfuly!" << std::endl;
	}
}

void ReceiveMsg() {
	char    RecvBuffer;
	int     iRecvBuffer = sizeof(RecvBuffer);
	int     checks = 0;

	std::string msg;
	std::string tmp;

	std::cout << "Receiving Message: ";
	while (true) {
		iRecv = recv(sAcceptSocket, &RecvBuffer, iRecvBuffer, 0);
		if (iRecv == SOCKET_ERROR) {
			std::cout << std::endl << "Receive Data Failed! Error no: " << WSAGetLastError() << std::endl;
		}
		else {
			msg += RecvBuffer;
			if ((RecvBuffer == (char)3) && checks == 0) {// && (check1 = false) && (check2 = false) && (check3 = false)
				checks = 1;
				tmp += RecvBuffer;
			}
			else if ((RecvBuffer == (char)3) && checks == 1) {// && (check1 = true) && (check2 = false) && (check3 = false)
				checks = 2;
				tmp += RecvBuffer;
			}
			else if ((RecvBuffer == (char)3) && checks == 2) {// && (check1 = true) && (check2 = true) && (check3 = false)
				checks = 3;
				std::cout << std::endl;
				break;
			}
			else {
				checks = 0;
				std::cout << tmp;
				std::cout << RecvBuffer;
				tmp = "";
			}

			/*std::cout << RecvBuffer;
			int MsgSize = msg.size();
			if (msg.size() > 5) {
				if (msg.substr(msg.size() - 5, msg.size()) == "~#EOM#~") {
					std::cout << '\b' << " " << '\b' << " " << '\b' << " " << '\b' << " " << '\b' << " ";

					break;
				}
			}*/

		}
	}
}

void CloseSocket() {
	iCloseSocket = closesocket(TCPServerSocket);
	if (iCloseSocket == SOCKET_ERROR) {
		std::cout << "Closing Socket Failed! Error no: " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "Closing Socket Success!" << std::endl;
	}
}

void UDF_WSACleanup() {
	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR) {
		std::cout << "Cleanup Func Failed! Error no: " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "Cleanup Func Success!" << std::endl;
	}
}