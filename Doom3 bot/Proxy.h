#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
    void StartLoop();


private:
	/// Help methods to connect
    /**
    This challanges the server and stores vital information necessary
    to continue establishing the server*/
	void ChallangeServer();
    /**
    Sends the connect request to server. This has to be done twice during
    the entire hand shake.*/
    void SendConnectRequest();

    /**
    Recieves and returns a checksum to fool the server, I would think.
    It's a necessary step no less*/
    void HandleGamePakChecksum();

	/**
	Stores some server info when we're finally connected*/
	void RecieveFinalServerInfo();

	/**
	Sends message to server that we're finally connected*/
	void FinalizeConnectionHandshake();

	/**
	Puts iser info into message channel*/
    void SendUserInfo();


	/// Core loop methods
	/**
	Sends all queued messages to the server*/
	void SendMessages();

	/**
	Sends an empty message to the server. This is apparently really necessary*/
	void PokeServer();

	/**
	Synchronizes time with the server*/
	void SyncTime();

	/**
	Queues user input to be sent to the server.
	This is done as a response to the server sending a snapshot
	TODO Change this??*/
	void QueueUserInput();


    /// Simple methods to send and recieve messages
    // Sends the provided message to the server. Returns result from sendto
    int SendToServer(const idBitMsg& p_msg);
    // Recieves message from server and stores in p_msg. Does not need to be initialized (I think)
    int RecieveFromServer(idBitMsg* p_msg);
	int RecieveFromServerDEBUG(idBitMsg* p_msg);

    /// Methods to interpret the messages sent from the server
    void RecieveUpdateFromServer();

    void HandleReliableServerMessage();

    void HandleUnreliableServerMessage(const idBitMsg& p_msg);

	// Member variables
	SOCKET m_socket;
	sockaddr_in m_recieveAddress;

	int m_clientId;
	int m_clientChecksum;
	int m_messageSequence;
    int m_challangeNr;
    int m_serverId;

	int m_clientNr;

	int m_clientGameId;
	int m_clientGameFrame;
	int m_clientGameTime;
	int m_clientTime;

    int m_serverGameId;
    int m_serverGameFrame;
    int m_serverGameTime;
	int m_serverTime;

	int m_startTime;

	int m_deltaTime;
	int m_frame;

	int m_snapshotSequence;

	idCompressor* m_compressor;
	idMsgChannel m_msgChannel;

	WSADATA m_wsadata;

	std::queue<idBitMsg> m_messageQueue;
};

