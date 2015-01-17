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
	std::unordered_map<std::thread*, CSocket*> socketMap;
public:
	CSocket serverSocket;
	bool startServer();
	bool stopServer();
	void onServer();
	void onClient(CSocket* client);
	void garbageCollect();
	bool stopFlag;
public:
	static E_Server* getSingleton();
};

