//#if defined(_DEBUG) && defined(WIN32) && !defined(_AFX) && !defined(_AFXDLL) && !defined(TRACE)
//#define TRACE	TRACE_WIN32
void TRACE_WIN32(LPCTSTR lpszFormat, ...);
void TRACE_WIN32A(LPCSTR lpszFormat, ...);
//#define TRACE_WIN32_REQUIRE_IMPL
//#endif
void CreateConsole();
