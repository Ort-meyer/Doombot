#include "Proxy.h"



Proxy::Proxy()
{
    // Setup connection stuff

	//DEBUGSHIT();
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    m_recieveAddress.sin_family = AF_INET;
    m_recieveAddress.sin_port = htons(27666);
    m_recieveAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	//m_recieveAddress.sin_addr.s_addr = inet_addr("194.47.155.248");
    int res = WSAStartup(MAKEWORD(2, 2), &m_wsadata);
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    m_clientId = 32;
    m_clientChecksum = 84287817;
    m_messageSequence = 1;

    m_compressor = idCompressor::AllocRunLength_ZeroBased();
    m_msgChannel = idMsgChannel();
    m_msgChannel.Init(32);    

	m_startTime = timeGetTime();
	m_prevTime = 0;
}


Proxy::~Proxy()
{

}

void Proxy::EstablishConnection()
{
	ChallangeServer();
    SendConnectRequest();
	HandleGamePakChecksum();
	// Do we really need to do this again?
	SendConnectRequest();
    RecieveFinalServerInfo();
	FinalizeConnectionHandshake();
}

void Proxy::StartLoop()
{
    while (true)
    {
        RecieveUpdateFromServer();
		PokeServer(); // Needed?
		SyncTime();
		m_frame++;
    }
}

void Proxy::SetMovement(Movement p_movementMask)
{
	m_movementMask = (int)p_movementMask;
}

void Proxy::SetAngles(int p_x, int p_y, int p_z)
{
	m_rotatex = p_x;
	m_rotatey = p_y;
	m_rotatez = p_z;
}

void Proxy::ChallangeServer()
{
    /// Send challange first
	idBitMsg t_msg;
	byte t_msgBuffer[16384];
    t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));
	
    t_msg.WriteShort(-1);
    t_msg.WriteString("challenge");
	
    int result = SendToServer(t_msg);
	t_msg = idBitMsg();
    /// Receive reply
    result = RecieveFromServer(&t_msg);

    char buffer[1024];
    t_msg.ReadShort();
    t_msg.ReadString(buffer, 1024);
    m_challangeNr = t_msg.ReadLong();
    m_serverId = t_msg.ReadShort();
}

void Proxy::SendConnectRequest()
{
    idBitMsg t_msg;
    byte t_msgBuffer[16384];

    t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

    // How these messages look have been extracted empirically from doom3
    t_msg.WriteShort(-1);
    t_msg.WriteString("connect");
    t_msg.WriteLong(65577);
    t_msg.WriteShort(0);
    t_msg.WriteLong(m_clientChecksum);
    t_msg.WriteLong(m_challangeNr);
    t_msg.WriteShort(m_clientId);
    t_msg.WriteLong(12000);
    t_msg.WriteString("");
    t_msg.WriteString("");
    t_msg.WriteShort(0);

    SendToServer(t_msg);
}

void Proxy::HandleGamePakChecksum()
{
    idBitMsg t_msg;
    int result = RecieveFromServer(&t_msg);
    // t_msg.SetSize(sizeof(msgBuf)); // This SHOULD NOT have to be done. At all
    t_msg.SetReadBit(0);
    t_msg.SetReadCount(0);

	// Read some stuff that's in the way for our checksum
    t_msg.ReadShort();
    char temp[2048];
    t_msg.ReadString(temp, sizeof(temp));
    for (int c = 0; c < 10; c++)
    {
		t_msg.ReadLong();
    }

    long t_checksum = t_msg.ReadLong();

	// Write message to send to server
	byte t_msgBuffer[16384];
	t_msg = idBitMsg();
	t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

	int	t_sendChecksums[128];
	t_sendChecksums[0] = 598628755;
	t_sendChecksums[1] = 1718344508;
	t_sendChecksums[2] = 156984747;
	t_sendChecksums[3] = -1879296479;
	t_sendChecksums[4] = 1985892235;
	t_sendChecksums[5] = -847654872;
	t_sendChecksums[6] = -987836979;
	t_sendChecksums[7] = 1076120544;
	t_sendChecksums[8] = 684853489;
	t_sendChecksums[9] = 0;

	t_msg.SetWriteBit(0);

	t_msg.WriteShort(-1);
	t_msg.WriteString("pureClient");
	t_msg.WriteLong(m_challangeNr);
	t_msg.WriteShort(m_clientId);

	// TODO rewrite into for loop
	int i = 0;
	while (t_sendChecksums[i]) {
		t_msg.WriteLong(t_sendChecksums[i++]);
	}
	t_msg.WriteLong(0);
	t_msg.WriteLong(t_checksum);

	SendToServer(t_msg);
}

void Proxy::RecieveFinalServerInfo()
{
	idBitMsg t_msg;
	RecieveFromServer(&t_msg);

	t_msg.SetReadBit(0);// not needed, is it?
	t_msg.SetReadCount(0);// not needed, is it?

	char temp[2048];
	t_msg.ReadShort();
	t_msg.ReadString(temp, sizeof(temp));

	m_clientNr = t_msg.ReadLong();

    m_serverGameId = t_msg.ReadLong();
    m_serverGameFrame = t_msg.ReadLong();
    m_serverGameTime = t_msg.ReadLong();

    m_clientGameId = m_serverGameId;
    m_clientGameFrame = m_serverGameFrame;
    m_clientGameTime = m_serverGameTime;


}

void Proxy::FinalizeConnectionHandshake()
{
	idBitMsg t_msg;
	byte t_msgBuffer[16384];
	t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

	//EmptyMessage
	t_msg.WriteLong(m_messageSequence);
	//t_msg.WriteLong(load ? gameInitId : -2);
	t_msg.WriteLong(m_clientGameId);
	t_msg.WriteLong(1);
	t_msg.WriteByte(0); //CLIENT_UNRELIABLE_MESSAGE_EMPTY

	idBitMsg t_msgToSend;

	t_msgToSend = m_msgChannel.AppendMessageInfo(t_msgToSend, m_clientGameTime, t_msg);

	m_messageQueue.push(t_msgToSend);
	SendMessages();
}

void Proxy::SendUserInfo()
{
    idBitMsg t_msg;

	byte t_msgBuffer[16384];
    t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

    t_msg.WriteByte(1);


    t_msg.WriteString("ui_showGun");
    t_msg.WriteString("1");

    t_msg.WriteString("ui_autoReload");
    t_msg.WriteString("1");

    t_msg.WriteString("ui_autoSwitch");
    t_msg.WriteString("1");

    t_msg.WriteString("ui_team");
    t_msg.WriteString("1");

    t_msg.WriteString("ui_skin");
    t_msg.WriteString("skins/characters/player/marine_mp");

    t_msg.WriteString("ui_name");
    t_msg.WriteString("BotPlayer");

    t_msg.WriteString("ui_chat");
    t_msg.WriteString("0");

    t_msg.WriteString("ui_spectate");
    t_msg.WriteString("Play");

    t_msg.WriteString("ui_ready");
    t_msg.WriteString("Not Ready");

    t_msg.WriteString("");

    m_msgChannel.SendReliableMessage(t_msg);

	// This apparently has to be done a few times...
	PokeServer();
	PokeServer();
	PokeServer();
	PokeServer();
    //SendEmptyToServer(false);
    //SendEmptyToServer(false);
    //SendEmptyToServer(false);
    //SendEmptyToServer(false);
}

void Proxy::SendMessages()
{
	//if (m_messageQueue.size() > 0)
	//{
	//	idBitMsg t_msg;
	//	while (m_messageQueue.size() > 0)
	//	{
	//		t_msg = m_messageQueue.front();
	//		m_messageQueue.pop();
	//		SendToServer(t_msg);
	//	}
	//}
	for (size_t i = 0; i < m_messageQueue.size(); i++)
	{
		idBitMsg t_msg = m_messageQueue.front();
		m_messageQueue.pop();
		SendToServer(t_msg);
	}
}

void Proxy::PokeServer()
{
	idBitMsg t_msg;
	byte t_msgBuffer[16384];
	t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

	//EmptyMessage
	t_msg.WriteLong(m_messageSequence);
	//t_msg.WriteLong(load ? gameInitId : -2);
	t_msg.WriteLong(-2);
	t_msg.WriteLong(1);
	t_msg.WriteByte(0); //CLIENT_UNRELIABLE_MESSAGE_EMPTY

	idBitMsg t_msgToSend;

	t_msgToSend = m_msgChannel.AppendMessageInfo(t_msgToSend, m_clientGameTime, t_msg);

	m_messageQueue.push(t_msgToSend);
	SendMessages();
}

void Proxy::SyncTime()
{
	int t_currentTime = timeGetTime();
	int t_addTime = 0;
	if (t_currentTime - m_prevTime < 0)
	{
		t_addTime = 0;
	}
	else if (t_currentTime - m_prevTime > 100)
	{
		t_addTime = 100;
	}
	else
	{
		t_addTime = t_currentTime - m_prevTime;
	}

	m_prevTime = t_currentTime;
	m_clientTime += t_addTime;
}

void Proxy::QueueUserInput()
{
	idBitMsg t_msg;
	byte t_msgBuffer[16384];
	t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

	t_msg.WriteLong(m_messageSequence);
	t_msg.WriteLong(m_clientGameId);
	t_msg.WriteLong(m_snapshotSequence);
	t_msg.WriteByte(2);
	t_msg.WriteShort(17); //clientPrediction

	t_msg.WriteLong(m_frame);
	t_msg.WriteByte(1);//numUserCmds

	t_msg.WriteLong(m_clientGameTime);

	t_msg.WriteByte(24);//Buttons
	t_msg.WriteShort(0);//mx
	t_msg.WriteShort(0);//my


	static int counter = 0;
	static int i = 0;
	counter++;
	if (counter == 120)
	{
		counter = 0;
		i += 15000;
		m_rotatex = i;
		if (m_movementMask == Movement::Forward)
		{
			m_movementMask = Movement::Right;
		}
		else
		{
			m_movementMask = Movement::Forward;
		}
	}




	// Forward movement
	if(m_movementMask & Movement::Forward)
		t_msg.WriteChar(100);
	else if (m_movementMask & Movement::Backward)
		t_msg.WriteChar(-100);
	else
		t_msg.WriteChar(0);
	
	// Backward movement
	if (m_movementMask & Movement::Right)
		t_msg.WriteChar(100); 
	else if (m_movementMask & Movement::Left)
		t_msg.WriteChar(-100); 
	else
		t_msg.WriteChar(0);

	t_msg.WriteChar(0);//upmove

	t_msg.WriteShort(m_rotatex);//Angles[0] 1742
	t_msg.WriteShort(m_rotatey);//Angles[1] -9211
	t_msg.WriteShort(m_rotatez);//Angles[2]

	 
	 

	idBitMsg t_sendMsg;

	t_sendMsg = m_msgChannel.AppendMessageInfo(t_sendMsg, m_clientTime, t_msg);

	m_messageQueue.push(t_sendMsg);
	SendMessages();
}

int Proxy::SendToServer(const idBitMsg & p_msg)
{
   return sendto(m_socket, (char*)p_msg.GetData(), p_msg.GetSize(), 0, (SOCKADDR*)&m_recieveAddress, sizeof(m_recieveAddress));
}

int Proxy::RecieveFromServer(idBitMsg * p_msg)
{
    char t_recieveBuffer[16384]; //// Might need to be bigger!
    int t_recieveAddrSize = sizeof(m_recieveAddress);
    int t_res = recvfrom(m_socket, t_recieveBuffer, 16384, 0, (SOCKADDR*)&m_recieveAddress, &t_recieveAddrSize);
    if (t_res == SOCKET_ERROR)
    {
        return t_res;
    }

    // Copy over data from server
    byte t_msgDataBuffer[16384];
    memcpy(t_msgDataBuffer, t_recieveBuffer, sizeof(t_recieveBuffer));

    // Initialize recieve message
    p_msg->Init(t_msgDataBuffer, sizeof(t_msgDataBuffer));
	p_msg->SetSize(sizeof(t_msgDataBuffer));

    return t_res;
}

int Proxy::RecieveFromServerDEBUG(idBitMsg * t_msg)
{
	char t_recieveBuffer[16384]; //// Might need to be bigger!
	int t_recieveAddrSize = sizeof(m_recieveAddress);
	int t_res = recvfrom(m_socket, t_recieveBuffer, 1024, 0, (SOCKADDR*)&m_recieveAddress, &t_recieveAddrSize);
	if (t_res == SOCKET_ERROR)
	{
		return t_res;
	}

	// Copy over data from server
	byte t_msgDataBuffer[16384];
	memcpy(t_msgDataBuffer, t_recieveBuffer, sizeof(t_recieveBuffer));

	// Initialize recieve message
	//p_msg = new idBitMsg();
	t_msg->Init(t_msgDataBuffer, sizeof(t_msgDataBuffer));
	t_msg->SetSize(sizeof(t_msgDataBuffer));
	t_msg->SetReadBit(0);// not needed, is it?
	t_msg->SetReadCount(0);// not needed, is it?

	char temp[2048];
	t_msg->ReadShort();
	t_msg->ReadString(temp, sizeof(temp));

	m_clientNr = t_msg->ReadLong();

	m_serverGameId = t_msg->ReadLong();
	m_serverGameFrame = t_msg->ReadLong();
	m_serverGameTime = t_msg->ReadLong();

	m_clientGameId = m_serverGameId;
	m_clientGameFrame = m_serverGameFrame;
	m_clientGameTime = m_serverGameTime;

	return t_res;
}

void Proxy::RecieveUpdateFromServer()
{

	// For some certainly really amazing reason, my RecieveFromServer method doesn't work here...
	idBitMsg		t_msg;
	char			t_recieveBuffer[16384];
	byte			t_msgBuffer[16384];

	int size = sizeof(m_recieveAddress);
	int result = recvfrom(m_socket, t_recieveBuffer, 16384, 0, (SOCKADDR*)&m_recieveAddress, &size);
	if (result == SOCKET_ERROR)
	{
		std::cout << "recvfrom failed with error:  " << WSAGetLastError() << std::endl;
		return;
	}
	memcpy(&t_msgBuffer, t_recieveBuffer, sizeof(t_recieveBuffer));
	t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));
	t_msg.SetSize(sizeof(t_msgBuffer));

	t_msg.ReadShort();

	if (!m_msgChannel.Process(m_clientTime, t_msg, m_messageSequence)) {
		return;		// out of order, duplicated, fragment, etc.
	}

	HandleReliableServerMessage();
	HandleUnreliableServerMessage(t_msg);
}

void Proxy::HandleReliableServerMessage()
{
    idBitMsg t_msg;
    byte t_msgBuffer[16384];
	byte type;

    t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

    while (m_msgChannel.GetReliableMessage(t_msg))
    {
        type = t_msg.ReadByte();
        switch (type)
        {
        //Print
        case 4:
        {
            char string[1024];
            t_msg.ReadString(string, 1024);
            std::cout << string << std::endl;
            break;
        }
        //Disconnect
        case 5:
        {
            //ProcessDisconnect();
            break;
        }
        //Applysnapshot
        case 6:
        {
            m_messageSequence = t_msg.ReadLong();
           // ProcessApplySnapshot();
            break;
        }
        //EnterGame
        case 8:
        {
            SendUserInfo();
            break;
        }
        }
    }
}

void Proxy::HandleUnreliableServerMessage(idBitMsg p_msg)
{

	int temp = p_msg.ReadLong(); // Think this is the server id
	int t_msgId = p_msg.ReadByte();
	switch (t_msgId)
	{
	// Just empty
	case 0:
		break;
	// Snapshot
	case 1:
	case 3:	
		m_snapshotSequence = p_msg.ReadLong();
		m_frame = p_msg.ReadLong();
		m_clientGameTime = p_msg.ReadLong();
		m_clientTime = m_clientGameTime;
		QueueUserInput();
		break;
	}
}
