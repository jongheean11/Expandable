#include "stdafx.h"
#include "E_Server.h"s


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
	E_MyCSocket* client = new E_MyCSocket;
	while (serverSocket.Accept(*client) && stopFlag == false){
		TRACE_WIN32A("[E_Server::onServer] Ŭ���̾�Ʈ ����");
		std::thread* th_client = new std::thread{ &E_Server::onClient, this, client};
		
		//�ʿ� �߰�
		threadState.insert(std::unordered_map<std::thread*, STATE>::value_type(th_client, RUNNING));
		socketMap.insert(std::unordered_map<std::thread*, E_MyCSocket*>::value_type(th_client, client));

		client = new E_MyCSocket;

		garbageCollect();
	}
}

void E_Server::onClient(E_MyCSocket* param)
{
	CString clientName = L"";
	UINT clientPort = 0;
	E_MyCSocket client;
	client.Attach(*param);
	client.GetPeerName(clientName, clientPort);
	TRACE_WIN32A("[E_Server::onClient] Ŭ���̾�Ʈ: %s, ��Ʈ: %d", clientName, clientPort);

	while (client.closeFlag == false)
	{
		char buff[MAXBUFFERSIZE];
		memset(buff, 0, sizeof(buff));

		//Ŭ���̾�Ʈ�κ��� �޽��� �б� 
		int received = 0;
		
		received = client.Receive(buff, MAXBUFFERSIZE, 0);
		if (received == 0 || received == SOCKET_ERROR)
		{
			TRACE_WIN32A("[E_Server::onClient] ���� ���� �α� (%d)", received == SOCKET_ERROR ? GetLastError() : 0 );
			break;
		}
		TRACE_WIN32A("[E_Server::onClient] %d ����Ʈ�� ������ %s", received, buff);
		//#����
		if (buff[received - 1] == '#'){
			
			buff[received - 1] = '\0';
			std::string sendData = PONG;
			sendData.append("#");
			if (strcmp(buff, PING.c_str()) == 0){
				int sendlen = client.Send(sendData.c_str(), strlen(sendData.c_str()));
				TRACE_WIN32A("[E_Server::onClient] %d ����Ʈ�� ������ %s", sendlen, sendData.c_str());

			}
			else{
				NOTIFICATION_ITEM item;
				readJSON(item, buff);
				if (item.type == PROGRESSSTATE)
				{
					E_Global* e_global = E_Global::getSingleton();
					
						E_Notify* e_noti = new E_Notify();

						e_noti->showNotify(item.infomation.pid, item.infomation.pname, (HWND)item.infomation.hwnd);
					
				}
			}
		}

		////���� �޽����� �ٽ� Ŭ���̾�Ʈ�� �޾Ƹ� ġ��
		//int sent = client->Send(buff, received);
		//TRACE_WIN32A("[E_Server::onClient] %d ����Ʈ�� �۽���", sent);
	}

	//���� �ٿ�
	client.ShutDown(2);

	for (std::unordered_map<std::thread*, E_MyCSocket*>::iterator iter = socketMap.begin(); iter != socketMap.end(); iter++){
		if (iter->second == param){
			std::thread* target = iter->first;
			threadState.find(target)->second = NOTRUNNING;
			client.Detach();
			client.Close();
			return;
		}
	}
}

void E_Server::garbageCollect()
{
	std::list<std::thread*> list;
	for (std::unordered_map<std::thread*, STATE>::iterator iter = threadState.begin(); iter != threadState.end(); iter++){
		if (iter->second == NOTRUNNING)
		{
			E_MyCSocket* socket;
			std::thread* th;
			if (socketMap.find(iter->first) != socketMap.end())
			{
				socket = socketMap.find(iter->first)->second;
				//socket->Close();		//���� ���� ( UI �����忡���� ������/ �����ӿ�ũ )
				delete socket;		//���� ���� (�Ҹ���)
				iter->first->detach();	//������ ���� �� ȣ��
				th = iter->first;
				list.push_back(th);
				//TRACE_WIN32A("���� �� ������ ����");
			}
			else{
				//TRACE_WIN32A("���� ���� ��.. �ڷᱸ�� ����ġ �߰�...");
			}
		}
	}

	for (std::list<std::thread*>::iterator iter = list.begin(); iter != list.end(); iter++){
		threadState.erase(*iter);
		socketMap.erase(*iter);
		delete *iter;//������ ����
		TRACE_WIN32A("���� �� ������ �ڷᱸ�� ����");
	}
	
}
bool E_Server::stopServer()
{
	stopFlag = true;
	return true;
}