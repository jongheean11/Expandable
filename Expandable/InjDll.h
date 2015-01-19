#pragma once
enum INJECTMODE{ INJECTION_MODE = 0, EJECTION_MODE };
int Injector(TCHAR* processName, INJECTMODE mode, TCHAR* dllpath);