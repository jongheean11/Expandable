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
	if (!AfxSocketInit())                          //소켓초기화
	{
		return false;
	}
	
	if (th == NULL){
		th = new std::thread{ &E_Server::onServer, this };
	}
	TRACE_WIN32A("[E_Server::startServer] 정상적으로 서버가 시작하였습니다.");
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
	//http://kuaaan.tistory.com/118 즉시 데이터를 버리는 옵션
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
		TRACE_WIN32A("[E_Server::onServer] 클라이언트 접속");
		std::thread* th_client = new std::thread{ &E_Server::onClient, this, client};
		
		//맵에 추가
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
	TRACE_WIN32A("[E_Server::onClient] 클라이언트: %s, 포트: %d", clientName, clientPort);

	char buff[MAXBUFFERSIZE];
	memset(buff, 0, sizeof(buff));

	//클라이언트로부터 메시지 읽기 
	int received = client->Receive(buff, sizeof(buff));
	TRACE_WIN32A("[E_Server::onClient] %d 바이트를 수신함", received);

	//받은 메시지를 다시 클라이언트로 메아리 치기
	int sent = client->Send(buff, received);
	TRACE_WIN32A("[E_Server::onClient] %d 바이트를 송신함", sent);

	//클라이언트 소켓 담기
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
				socket->Close();//소켓 닫음 ( UI 스레드 )
				delete socket;	//소켓 삭제
				iter->first->detach();
				delete iter->first;	//스레드 삭제
				TRACE_WIN32A("소켓 및 스레드 삭제");
			}
			else{
				TRACE_WIN32A("소켓 삭제 중.. 자료구조 불일치 발견...");
			}
		}
	}
}
bool E_Server::stopServer()
{
	stopFlag = true;
	return true;
}