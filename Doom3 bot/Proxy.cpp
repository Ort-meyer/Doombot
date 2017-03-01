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
	idBitMsg t_msg;
	byte t_msgBuffer[16384];
    t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

    t_msg.WriteShort(-1);
    t_msg.WriteString("challange");

    int result = SendToServer(t_msg);

    /// Receive reply
    result = RecieveFromServer(&t_msg);
    if (result == SOCKET_ERROR)
    {
        std::cout << "recvfrom failed with error:  " << WSAGetLastError() << std::endl;
        return;
    }
    char buffer[1024];
    t_msg.ReadShort();
    t_msg.ReadString(buffer, 1024);
    m_challangeNr = t_msg.ReadLong();
    m_serverId = t_msg.ReadShort();
}

int Proxy::SendToServer(const idBitMsg & p_msg)
{
   return sendto(m_socket, (char*)p_msg.GetData(), p_msg.GetSize(), 0, (SOCKADDR*)&m_recieveAddress, sizeof(m_recieveAddress));
}

int Proxy::RecieveFromServer(idBitMsg * p_msg)
{
    char t_recieveBuffer[1000]; //// Might need to be bigger!
    int t_recieveAddrSize = sizeof(m_recieveAddress);
    int t_res = recvfrom(m_socket, t_recieveBuffer, 1024, 0, (SOCKADDR*)&m_recieveAddress, &t_recieveAddrSize);
    if (t_res == SOCKET_ERROR)
    {
        return t_res;
    }

    // Copy over data from server
    byte t_msgDataBuffer[16384];
    memcpy(p_msg->GetData, t_recieveBuffer, sizeof(t_recieveBuffer));

    // Initialize recieve message
    p_msg = new idBitMsg();
    p_msg->Init(t_msgDataBuffer, sizeof(t_msgDataBuffer));

    return t_res;
}
