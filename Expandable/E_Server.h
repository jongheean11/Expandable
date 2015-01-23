#include "E_MyCSocket.h"
#include "E_DLLServer.h"
#include "E_Notify.h"

#pragma once
#define PORT 8888
#define MAXBUFFERSIZE 1024

enum STATE{NOTRUNNING, RUNNING};
class E_Server
{
private:
	static E_Server* singleton;
	E_Server();
	~E_Server();
	std::thread* th;
	std::unordered_map<std::thread*, STATE> threadState;
	std::unordered_map<std::thread*, E_MyCSocket*> socketMap;
public:
	E_MyCSocket serverSocket;
	bool startServer();
	bool stopServer();
	//void onServer();
	void onServer(SOCKET param);
	void onClient(E_MyCSocket* client);
	void garbageCollect();
	bool stopFlag;
public:
	static E_Server* getSingleton();
};

