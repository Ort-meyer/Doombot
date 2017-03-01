#pragma once

#include <WinSock2.h>
#include <queue>
#include <iostream>

#include "Compressor.h"
#include "MsgChannel.h"

using namespace std;
class Proxy
{
public:
	Proxy();
	~Proxy();

	void EstablishConnection();
	void HandleMessage();


private:
	/// Help methods
    /*
    This challanges the server and stores vital information necessary
    to continue establishing the server*/
	void ChallangeServer();

    // Sends the provided message to the server. Returns result from sendto
    int SendToServer(const idBitMsg& p_msg);
    // Recieves message from server and stores in p_msg. Does not need to be initialized (I think)
    int RecieveFromServer(idBitMsg* p_msg);


	// Member variables
	SOCKET m_socket;
	sockaddr_in m_recieveAddress;

	int m_clientId;
	int m_clientChecksum;
	int m_messageSequence;
    int m_challangeNr;
    int m_serverId;

	int m_deltaTime;
	int m_frame;

	idCompressor* m_compressor;
	idMsgChannel m_msgChannel;

	WSADATA m_wsadata;

	std::queue<idBitMsg> m_messageQueue;
};

