#include "Proxy.h"



Proxy::Proxy()
{


	// Card coded values extrated from doom3
	m_clientId = 32;
	m_clientChecksum = 84287817;
	m_messageSequence = 1;

	m_compressor = idCompressor::AllocRunLength_ZeroBased();
	m_msgChannel = idMsgChannel();
	
	m_msgChannel.Init(32);

	int result = WSAStartup(MAKEWORD(2, 2), &m_wsadata);
	//if (result != NO_ERROR) {
	//	std::cout << "WSAStartup failed with error:  " << result << std::endl;
	//}

	// Create socket
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//if (m_socket == INVALID_SOCKET)
	//{
	//	std::cout << "Socket failed with error:  " << result << std::endl;
	//	WSACleanup();
	//}
	// Create address
	m_recieveAddress.sin_family = AF_INET;
	m_recieveAddress.sin_port = htons(27666); // hard coded port
	m_recieveAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // hard coded to local address

	m_messageQueue = queue<idBitMsg>();

	m_deltaTime = 0;
	m_frame = 0;

}


Proxy::~Proxy()
{

}

void Proxy::EstablishConnection()
{
	ChallangeServer();
}

void Proxy::ChallangeServer()
{
    /// Send challange first
	idBitMsg message;
	byte messageBuffer[16384];
	message.Init(messageBuffer, sizeof(messageBuffer));

	message.WriteShort(-1);
	message.WriteString("challange");

	int result = sendto(m_socket, (char*)message.GetData(), message.GetSize(), 0, (SOCKADDR*)&m_recieveAddress, sizeof(m_recieveAddress));

	if (result == SOCKET_ERROR)
	{
		cout << "failed to send challange" << endl;
	}

    /// Receive reply
    char recieveBuffer[1000]; //// Might need to be bigger!
    int recieveAddrSize = sizeof(m_recieveAddress);
    result = recvfrom(m_socket, recieveBuffer, 1024, 0, (SOCKADDR*)&m_recieveAddress, &recieveAddrSize);
    if (result == SOCKET_ERROR)
    {
        std::cout << "recvfrom failed with error:  " << WSAGetLastError() << std::endl;
        return;
    }
    memcpy(&messageBuffer, recieveBuffer, sizeof(recieveBuffer));
    message.Init(messageBuffer, sizeof(messageBuffer));
    message.SetSize(sizeof(messageBuffer));


    char buffer[1024];
    message.ReadShort();
    message.ReadString(buffer, 1024);
    m_challangeNr = message.ReadLong();
    m_serverId = message.ReadShort();
}
