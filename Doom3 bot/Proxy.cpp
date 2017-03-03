#include "Proxy.h"



Proxy::Proxy()
{
    // Setup connection stuff
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    m_recieveAddress.sin_family = AF_INET;
    m_recieveAddress.sin_port = htons(27666);
    m_recieveAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    WSAStartup(MAKEWORD(2, 2), &m_wsadata);

    m_clientId = 32;
    m_clientChecksum = 84287817;
    m_messageSequence = 1;

    m_compressor = idCompressor::AllocRunLength_ZeroBased();
    m_msgChannel = idMsgChannel();
    m_msgChannel.Init(32);    
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
    }
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
    RecieveFromServer(&t_msg);

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
    RecieveFromServer(&t_msg);
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

	//t_msg.SetSize(sizeof(msgBuf));
	t_msg.SetReadBit(0);// not needed, is it?
	t_msg.SetReadCount(0);// not needed, is it?

	char temp[1024];
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
    memcpy(t_msgDataBuffer, t_recieveBuffer, sizeof(t_recieveBuffer));

    // Initialize recieve message
    p_msg = new idBitMsg();
    p_msg->Init(t_msgDataBuffer, sizeof(t_msgDataBuffer));

    return t_res;
}

void Proxy::RecieveUpdateFromServer()
{
    idBitMsg t_msg;


    RecieveFromServer(&t_msg);

    t_msg.ReadShort();

    if (!m_msgChannel.Process(m_clientGameTime, t_msg, m_messageSequence)) {
        return;		// out of order, duplicated, fragment, etc.
    }
    
    ////lastPacketTime = clientTime;
    ////std::cout << "Process Lyckades" << std::endl;
    HandleReliableServerMessage();
    //ProcessUnreliableServerMessage(msg);

}

void Proxy::HandleReliableServerMessage()
{
    idBitMsg	t_msg;
    byte		t_msgBuffer[16384];

    t_msg.Init(t_msgBuffer, sizeof(t_msgBuffer));

    while (m_msgChannel.GetReliableMessage(t_msg))
    {
        byte type = t_msg.ReadByte();
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
            //serverMessageSequence = t_msg.ReadLong();
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

void Proxy::HandleUnreliableServerMessage()
{
    //int i, j, index, id, numDuplicatedUsercmds, aheadOfServer, numUsercmds, delta;
    //int serverGameInitId, serverGameFrame, serverGameTime;
    ////idDict serverSI;
    ////usercmd_t *last;
    //bool pureWait;
    //
    //serverGameInitId = msg.ReadLong();
    //
    //id = msg.ReadByte();
    //switch (id)
    //{
    //    //Empty
    //case 0:
    //{
    //    break;
    //
    //}
    ////Ping
    //case 1:
    //{
    //    ProcessPingFromServer();
    //}
    ////Snapshot
    //case 3:
    //{
    //    //std::cout << "Snapshot" << std::endl;
    //    // if the snapshot is from a different game
    //    if (serverGameInitId != gameInitId)
    //    {
    //        std::cout << "ignoring snapshot with != gameInitId\n";
    //    }
    //
    //
    //    snapshotSequence = msg.ReadLong();
    //    snapshotGameFrame = msg.ReadLong();
    //    snapshotGameTime = msg.ReadLong();
    //    numDuplicatedUsercmds = msg.ReadByte();
    //    aheadOfServer = msg.ReadShort();
    //
    //    ProcessSnapshot(clientNum, snapshotSequence, snapshotGameFrame, snapshotGameTime, numDuplicatedUsercmds, aheadOfServer, msg);
    //
    //    // if the snapshot is newer than the clients current game time
    //    if (gameTime < snapshotGameTime)
    //    {
    //        gameTime = snapshotGameTime;
    //    }
    //    gameFrame = snapshotGameFrame;
    //    clientTime = snapshotGameTime;
    //    gameTime = snapshotGameTime;
    //    SendUserCmdsToServer();
    //    delta = gameTime - (snapshotGameTime + clientPrediction);
    //
    //    lastSnapshotTime = clientTime;
    //
    //
    //    //std::cout << "received snapshot, gameInitId = " << gameInitId << " gameTime = " << gameTime << std::endl;
    //
    //
    //    if (numDuplicatedUsercmds)
    //    {
    //        //std::cout << "server duplicated %d user commands before snapshot %d\n", numDuplicatedUsercmds, snapshotGameFrame;
    //    }
    //    break;
    //}
    //
    //}
}
