#include "stdafx.h"
#include "E_DLLServer.h"

void writeJSON(__in NOTIFICATION_ITEM& item, __out std::string& buffer)
{
	Json::Value root;

	Json::Value information;
	information[pid_key] = item.infomation.pid;
	information[pname_key] = item.infomation.pname;
	information[hwnd_key] = item.infomation.hwnd;

	root[type_key] = item.type;
	root[value_key] = item.value;
	root[infomation_key] = information;

	Json::StyledWriter writer;

	buffer = writer.write(root);

	//TRACE_WIN32A("JSON DATA %s", buffer.c_str());
}
void readJSON(__out NOTIFICATION_ITEM& item, __in std::string& buffer)
{
	Json::Value root;
	Json::Reader reader;
	const char* buf = buffer.c_str();
	if (reader.parse(buf, root) == false){
		TRACE_WIN32A("[FAIL]reader.parse");
		return;
	}
	item.type = (TYPE)root.get(type_key, "defaultvalue").asInt();
	item.value = (VALUE)root.get(value_key, "defaultvalue").asUInt();
	Json::Value ps = root.get(infomation_key,"defaultvalue");
	
	item.infomation.hwnd = ps.get(pid_key, "defaultvalue").asUInt();
	std::string processName = ps.get(pname_key, "defaultvalue").asString();
	item.infomation.pid = ps.get(hwnd_key, "defaultvalue").asUInt();
	
	strcpy(item.infomation.pname, processName.c_str());
}
void readJSON(__out NOTIFICATION_ITEM& item, __in char* buffer)
{
	Json::Value root;
	Json::Reader reader;
	const char* buf = buffer;
	if (reader.parse(buf, root) == false){
		TRACE_WIN32A("[FAIL]reader.parse");
		return;
	}
	item.type = (TYPE)root.get(type_key, "defaultvalue").asInt();
	item.value = (VALUE)root.get(value_key, "defaultvalue").asUInt();
	Json::Value ps = root.get(infomation_key, "defaultvalue");
	
	item.infomation.hwnd = ps.get(hwnd_key, "defaultvalue").asUInt();
	std::string processName = ps.get(pname_key, "defaultvalue").asString();
	item.infomation.pid = ps.get(pid_key, "defaultvalue").asUInt();

	strcpy(item.infomation.pname, processName.c_str());
}

//소켓
static SOCKET e_sock=NULL;


//비동기 커넥션의 에러 값을 통해 연결되었는지 알 수 있는 로직
bool isConnected()
{
	bool result = false;
	int error = 0;
	int len = sizeof(error);
	if (SOCKET_ERROR == getsockopt(e_sock, SOL_SOCKET, SO_ERROR,(char*) &error, &len)) {
		// 값을 가져오는데 에러 발생
		// errno을 가지고 에러 값을 출력
		// 연결 오류로 처리
		TRACE_WIN32A("[FAIL] getsockopt (%d)",WSAGetLastError());
		return false;
	}
	if (error == 0){
		TRACE_WIN32A("[SUCCESS] Connection OK");
		result = true;
	}
	else if (WSAECONNREFUSED == error){
		TRACE_WIN32A("[FAIL] WSAECONNREFUSED");
		result = false;
	}
	else if (WSAETIMEDOUT == error){
		TRACE_WIN32A("[FAIL] WSAETIMEDOUT");
		result = false;
	}
	else{
		TRACE_WIN32A("[FAIL] UNKNOWN CONNECTION ERROR");
		result = false;
	}
	return result;
}

bool sendData(std::string& buffer)
{
	int retval;

	char payload[MAXBUFSIZE] = { 0, };
	memcpy(payload, buffer.c_str(), buffer.length());
	payload[buffer.length()] = '#';

	retval = send(e_sock, payload, strlen(payload), 0);
	if (retval == SOCKET_ERROR){
		TRACE_WIN32A("[FAIL] send() ");
		return false;
	}
	TRACE_WIN32A("[SUCCESS] send() ");
	return true;
}

int _recv_ahead(__out char* word){
	//쓰레드 로컬 스토리지
	__declspec(thread) static int nbytes = 0;
	__declspec(thread) static char buf[MAXBUFSIZE];
	__declspec(thread) static char *ptr;

	if (nbytes == 0 || nbytes == SOCKET_ERROR){
		nbytes = recv(e_sock, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (nbytes == 0)
			return 0;
		ptr = buf;
	}

	--nbytes;
	*word = *ptr++;
	return 1;
}

int recvData(__out char* buffer){
	int n, nbytes;
	char c;
	char *ptr = buffer;
	
	//MAXBUFSIZE보다 크다면 #을 만나지 못함
	for (n = 1; n < MAXBUFSIZE; n++){
		nbytes = _recv_ahead(&c);
		if (nbytes == 1){
			*ptr++ = c;
			if (c == '#')
				break;
		}
		else if (nbytes == 0){
			*ptr = 0;
			return n - 1;
		}
		else
			return SOCKET_ERROR;
	}

	*ptr = 0;	//마지막은 NULL
	return n;
}
void init(){
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		TRACE_WIN32A("[FAIL] WSAStartup");
	}
}
void clean()
{

	WSACleanup();
}

bool connectExpandable()
{
	int retval;
	//소켓 생성
	e_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (e_sock == INVALID_SOCKET){
		TRACE_WIN32A("[FAIL] socket() INVALID_SOCKET");
		return false;
	}

	//논블로킹으로 전환
	u_long on = 1;
	retval = ioctlsocket(e_sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
		TRACE_WIN32A("[FAIL] ioctlsocket() SOCKET_ERROR");
	
	//연결
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(e_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	
	if (WSAEWOULDBLOCK == GetLastError()){
		TRACE_WIN32A("[ASYNC] You can't know Connection state (%d)", GetLastError());
		return true;
	}
	/*if (retval == SOCKET_ERROR){
		TRACE_WIN32A("[FAIL] connect() SOCKET_ERROR %d", GetLastError());
		return false;
	}*/
	return true;
}

bool disconnectExpandable()
{
	closesocket(e_sock);
	return true;
}

bool isAvailable()
{
	int retval;

	char payload[MAXBUFSIZE] = { 0, };
	memcpy(payload, PING.c_str(), PING.length());
	payload[PING.length()] = '#';

	retval = send(e_sock, payload, strlen(payload), 0);
	if (retval == SOCKET_ERROR){
		TRACE_WIN32A("[FAIL] send() ");
		return false;
	}
	else{
		TRACE_WIN32A("[SUCCESS] PING ");
	}

	u_long on = 0;
	ioctlsocket(e_sock, FIONBIO, &on);
	
	char recvbuf[MAXBUFSIZE] = { 0, };
	recvData(recvbuf);

	on = 1;
	ioctlsocket(e_sock, FIONBIO, &on);

	char* sharp = NULL;
	if ((sharp = strrchr(recvbuf, '#'))){
		*sharp = NULL;
	}

	if (!strcmp(recvbuf, PONG.c_str())){
		TRACE_WIN32A("[SUCCESS] PONG ");
		return true;
	}
	else{
		return false;
	}
	return true;
}