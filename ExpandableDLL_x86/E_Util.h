//HWND GetWindowFromPID(DWORD dwPID);
//BOOL GetProcessModule(DWORD dwPID, LPCTSTR pszProcessName);
//DWORD GetProcessID(LPCTSTR pszProcessName);
BOOL isVisible(char* processName);
wchar_t* ConverCtoWC(char* str);
DWORD GetProcessID(LPCTSTR pszProcessName);
DWORD getppid(LPCTSTR pszProcessName, DWORD pid);