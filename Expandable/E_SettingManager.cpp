#include "stdafx.h"
#include "E_SettingManager.h"

E_SettingManager* E_SettingManager::singleton = NULL;


E_SettingManager::E_SettingManager()
{
}


E_SettingManager::~E_SettingManager()
{
}

E_SettingManager* E_SettingManager::getSingleton()
{
	if (singleton == NULL){
		singleton = new E_SettingManager();
	}
	return singleton;
}

bool E_SettingManager::writeData(SettingInfo& info)
{
	Json::Value root;
	root["mapsize"] = info.mapsize;
	root["iconsize"] = info.iconsize;
	root["transparent"] = info.transparent;
	root["settingTimer"] = info.settingTimer;
	root["desktopwidth"] = info.desktopwidth;
	root["desktopheight"] = info.desktopheight;
	 
	Json::StyledWriter writer;

	string buffer;
	buffer = writer.write(root);

	char fileName[255] = { NULL };
	GetModuleFileNameA(NULL, fileName, 255);

	if (strlen(fileName) == NULL)
		return false;

	//현재 실행 파일 위치
	char* filePath = strrchr(fileName, '\\');
	strcpy(filePath, "setting.ini");

	FILE* fp = fopen(filePath, "w");
	if (NULL == fp)
		return false;
	fwrite(buffer.c_str(), strlen(buffer.c_str()), 1, fp);
	fclose(fp);
	return true;
}
bool E_SettingManager::readData(SettingInfo& info)
{
	char settingBuffer[1024] = { 0, };

	char fileName[255] = { NULL };
	GetModuleFileNameA(NULL, fileName, 255);

	if (strlen(fileName) == NULL)
		return false;

	//현재 실행 파일 위치
	char* filePath = strrchr(fileName, '\\');
	strcpy(filePath, "setting.ini");

	if (TRUE == PathFileExistsA(filePath)){
		FILE* fp = fopen(filePath, "r");
		if (fp == NULL)
			return false;

		fread(settingBuffer, 1024, 1, fp);
		fclose(fp);
	}

	Json::Value result;
	Json::Reader reader;
	const char* buf = settingBuffer;
	if (reader.parse(buf, result) == false){
		TRACE_WIN32A("[FAIL]reader.parse");
		return false;
	}

	//settingTimer = 5;
	//transparent = 160;
	//mapsize = 0.06;
	//iconsize = 1;
	//desktopwidth = 3;
	//desktopheight = 3;
	//값 세팅
	info.mapsize = result.get("mapsize", 0.06).asDouble();
	info.iconsize = result.get("iconsize", 1).asDouble();
	info.settingTimer = result.get("settingTimer", 5).asInt();
	info.transparent = result.get("transparent", 160).asInt();
	info.desktopwidth = result.get("desktopwidth", 3).asInt();
	info.desktopheight = result.get("desktopheight", 3).asInt();
	return true;
}
bool E_SettingManager::saveSetting()
{
	E_Global* global = E_Global::getSingleton();
	SettingInfo setting;
	setting.mapsize = global->getMapsize();//
	setting.iconsize = global->getIconsize();//
	setting.transparent = global->getTransparent();//
	setting.settingTimer = global->getTimer();//
	setting.desktopwidth = global->desktopwidth;//
	setting.desktopheight = global->desktopheight;//

	if (writeData(setting)){
		return true;
	}
	else{
		return false;
	}
}

bool E_SettingManager::loadSetting(SettingInfo& info)
{
	if (readData(info)){
		return true;
	}
	else{
		return false;
	}
}