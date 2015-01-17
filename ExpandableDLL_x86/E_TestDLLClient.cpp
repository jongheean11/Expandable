#include "stdafx.h"
#include "E_TestDLLClient.h"
#include "E_DLLClient.h"



void testWriteJSON(){
	std::string buffer;
	NOTIFICATION_ITEM item;
	item.type = PROGRESSVALUE;
	item.value = 66;
	item.infomation.hwnd = 50;
	item.infomation.pid = 4500;
	memcpy(item.infomation.pname, "chrome.exe", 11);
	writeJSON(item, buffer);
	//TRACE_WIN32A("json1: %s", buffer.c_str());
}

void testSocket(){
	init();
	connectExpandable();

	std::string buffer;
	NOTIFICATION_ITEM item;
	item.type = PROGRESSVALUE;
	item.value = 66;
	item.infomation.hwnd = 50;
	item.infomation.pid = 4500;
	memcpy(item.infomation.pname, "chrome.exe", 11);
	writeJSON(item, buffer);

	if (isConnected() && isAvailable()){
		sendData(buffer);
		char output[1024] = { 0, };
		recvData(output);
		TRACE_WIN32A("output: %s", output);
	}
	disconnectExpandable();
	clean();
}

void testConnectServer(){
	init();
	if (connectExpandable()){
		TRACE_WIN32A("[connectExpandable] ¼º°ø");

		std::string buffer;
		NOTIFICATION_ITEM item;
		item.type = PROGRESSVALUE;
		item.value = 66;
		item.infomation.hwnd = 50;
		item.infomation.pid = 4500;
		memcpy(item.infomation.pname, "chrome.exe", 11);
		writeJSON(item, buffer);
		if (isConnected()){
			sendData(buffer);
		}
	}
	while (1){
	}
	disconnectExpandable();
	clean();
}