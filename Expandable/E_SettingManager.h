#pragma once
#include <json/json.h>
#include "E_Global.h"

struct SettingInfo{
	double mapsize;
	double iconsize;
	int transparent;
	int settingTimer;
	int desktopwidth;
	int desktopheight;
};

class E_SettingManager
{
private:
	static E_SettingManager* singleton;
public:
	E_SettingManager();
	~E_SettingManager();
	static E_SettingManager* getSingleton();
	bool writeData(SettingInfo& info);
	bool readData(SettingInfo& info);
	bool saveSetting();
	bool loadSetting(SettingInfo& info);
};

