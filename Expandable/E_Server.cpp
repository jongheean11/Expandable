#include "stdafx.h"
#include "E_Server.h"


E_Server* E_Server::singleton = NULL;
E_Server::E_Server() : th(NULL), stopFlag(false)
{
}


E_Server::~E_Server()
{
}

E_Server* E_Server::getSingleton()
{
	if (singleton == NULL){
		singleton = new E_Server;
	}
	return singleton;
}
bool E_Server::startServer()
{
	if (!AfxSocketInit())                          //�����ʱ�ȭ
	{
		return false;
	}
	
	if (th == NULL){
		th = new std::thread{ &E_Server::onServer, this };
	}
	TRACE_WIN32A("[E_Server::startServer] ���������� ������ �����Ͽ����ϴ�.");
	return true;
}
void E_Server::onServer()
{
	//serverSocket
	serverSocket.Create(PORT, SOCK_STREAM, L"127.0.0.1");
	BOOL reuse = TRUE;
	if (!serverSocket.SetSockOpt(SO_REUSEADDR, &reuse, sizeof(int))) {
		TRACE_WIN32A("[SetSockOpt] SO_REUSEADDR ERROR");
		return;
	}
	//http://kuaaan.tistory.com/118 ��� �����͸� ������ �ɼ�
	LINGER linger;
	linger.l_linger = 0;
	linger.l_onoff = 1;
	if (!serverSocket.SetSockOpt(SO_LINGER, &linger, sizeof(linger))) {
		TRACE_WIN32A("[SetSockOpt] SO_LINGER ERROR");
		return;
	}
	if (serverSocket.Listen() == FALSE)
		return;
	CSocket* client = new CSocket;
	while (serverSocket.Accept(*client) && stopFlag == false){
		TRACE_WIN32A("[E_Server::onServer] Ŭ���̾�Ʈ ����");
		std::thread* th_client = new std::thread{ &E_Server::onClient, this, client};
		
		//�ʿ� �߰�
		threadState.insert(std::unordered_map<std::thread*, STATE>::value_type(th_client, RUNNING));
		socketMap.insert(std::unordered_map<std::thread*, CSocket*>::value_type(th_client, client));

		client = new CSocket;

		garbageCollect();
	}
}

void E_Server::onClient(CSocket* client)
{
	CString clientName = L"";
	UINT clientPort = 0;

	client->GetPeerName(clientName, clientPort);
	TRACE_WIN32A("[E_Server::onClient] Ŭ���̾�Ʈ: %s, ��Ʈ: %d", clientName, clientPort);

	char buff[MAXBUFFERSIZE];
	memset(buff, 0, sizeof(buff));

	//Ŭ���̾�Ʈ�κ��� �޽��� �б� 
	int received = client->Receive(buff, sizeof(buff));
	TRACE_WIN32A("[E_Server::onClient] %d ����Ʈ�� ������", received);

	//���� �޽����� �ٽ� Ŭ���̾�Ʈ�� �޾Ƹ� ġ��
	int sent = client->Send(buff, received);
	TRACE_WIN32A("[E_Server::onClient] %d ����Ʈ�� �۽���", sent);

	//Ŭ���̾�Ʈ ���� ���
	((CAsyncSocket*)client)->ShutDown(2);

	for (std::unordered_map<std::thread*, CSocket*>::iterator iter = socketMap.begin(); iter != socketMap.end(); iter++){
		if (iter->second == client){
			std::thread* target = iter->first;
			threadState.find(target)->second = NOTRUNNING;
		}
	}
}

void E_Server::garbageCollect()
{
	for (std::unordered_map<std::thread*, STATE>::iterator iter = threadState.begin(); iter != threadState.end(); iter++){
		if (iter->second == NOTRUNNING)
		{
			CSocket* socket;
			if (socketMap.find(iter->first) != socketMap.end())
			{
				socket = socketMap.find(iter->first)->second;
				socket->Close();//���� ���� ( UI ������ )
				delete socket;	//���� ����
				iter->first->detach();
				delete iter->first;	//������ ����
				TRACE_WIN32A("���� �� ������ ����");
			}
			else{
				TRACE_WIN32A("���� ���� ��.. �ڷᱸ�� ����ġ �߰�...");
			}
		}
	}
}
bool E_Server::stopServer()
{
	stopFlag = true;
	return true;
}