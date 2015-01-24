
// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "Expandable.h"

#include "MainFrm.h"
#include "EnviromnmentDialog.h"
#include "ShowMenu.h"
#include "E_Global.h"
#include "E_Map.h"
#include "E_Notify.h"
#include "E_Server.h"
#include "InjDll.h"
#include "E_DesktopSwitcher.h"
#include "E_DragAndDropSwitcher.h"
//

//



#define WM_USER_NOTIFY (WM_USER + 4)
#define WM_USER_MAPR (WM_USER + 5)
#define WM_CHANGEDISPLAY (WM_USER + 7)
#define WM_INVALIDATE (WM_USER + 6)
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_TRAY_NOTIFICATION WM_APP + 1
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification)
	ON_COMMAND(ID_32775, &CMainFrame::On32775)
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
//	ON_WM_SYSKEYDOWN()
//	ON_WM_SYSKEYUP()
	ON_MESSAGE(WM_USER_NOTIFY, OnUserNotify)
	ON_MESSAGE(WM_TRAY_EVENT, OnTrayEvent)
	ON_MESSAGE(WM_USER_MAPR, OnMapRight)
	ON_MESSAGE(WM_CHANGEDISPLAY, OnChange)
	ON_COMMAND(ID_32777, &CMainFrame::On32777)
	ON_COMMAND(ID_32778, &CMainFrame::On32778)
	ON_COMMAND(ID_32779, &CMainFrame::On32779)
	ON_COMMAND(ID_32781, &CMainFrame::On32781)
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�
CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	DwmEnableComposition(DWM_EC_ENABLECOMPOSITION); //DWM_EC_ENABLECOMPOSITION // DWM_EC_DISABLECOMPOSITION
	//DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
	icondisable = false;
	alreadyrun = false;
	autoalready = false;
	////
	//���α׷� ���������� ���� �߰�
	

}

HRESULT CMainFrame::OnUserNotify(WPARAM wParam, LPARAM lParam)
{
	// TODO: Your Code
	//if(noti->deleteAble();
	NOTIFICATION_ITEM* item = (NOTIFICATION_ITEM*)wParam;
	TRACE_WIN32A("OnUserNotify");
	E_Notify* noti = new E_Notify();
	noti->showNotify(item->infomation.pid, item->infomation.pname, (HWND)item->infomation.hwnd);
	delete item;
	return TRUE;
}
CMainFrame::~CMainFrame()
{

}
HRESULT CMainFrame::OnTrayEvent(WPARAM wParam, LPARAM lParam)
{
	// TODO: Your Code
	OutputDebugString(L"OnTrayEvent ");
	E_Map* e_map = E_Map::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	if (!(e_map->ison2))
	{
		OutputDebugString(L"OnTrayEvent ifenter ");
		e_map->drawMap();
	}
	
	changetray((int)wParam + 1);
	
	//::SetFocus(e_map->hwnd_cwnd_emap->m_hWnd);
	//::SetFocus(e_map->hwnd_cwnd_emap->m_hWnd);
	//e_map->leave2 = true;
	return TRUE;
}
HRESULT CMainFrame::OnMapRight(WPARAM wParam, LPARAM lParam)
{
	// TODO: Your Code
	E_Global* e_global = E_Global::getSingleton();
	bool dockicon = false;
	hwnd = (HWND)wParam;

	std::list<E_Desktop*> desklist2 = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//�� ����ũž �������
	{
		std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
		for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//�� ����ũž �������
		{
			if ((*itr_window)->getWindow() == hwnd)
			{
				if ((*itr_window)->dock)
				{
					dockicon = true;
					break;
				}
			}
		}
		if (dockicon)
			break;
	}


	UINT uMouseMsg = (UINT)lParam;
	POINT MousePos;
	GetCursorPos(&MousePos);
	CMenu menu, *pPopup;
	UINT id;
	E_Map* e_map = E_Map::getSingleton();
	menu.LoadMenu(IDR_MAINFRAME);
	if (dockicon)
		pPopup = menu.GetSubMenu(3);
	else
		pPopup = menu.GetSubMenu(2);

	id = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, MousePos.x, MousePos.y, this);
	switch (id)
	{
	case ID_32778:
		On32778();
		break;
	case ID_32779:
		On32779();
		//PostMessage(WM_QUIT);
		break;
	case ID_32781:
		On32781();
		//PostMessage(WM_QUIT);
		break;

	}



	return TRUE;
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	E_Global* e_global = E_Global::getSingleton();
	e_global->hwnd_frame = this->GetSafeHwnd();
	
	


	//::SetWindowText(this->GetSafeHwnd(), TEXT("E_expandable"));

	//

	HANDLE hEvent;

	hEvent = CreateEvent(NULL, FALSE, TRUE, AfxGetAppName());

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(TEXT("�̹� ���α׷��� �������Դϴ�."));
		autoalready = true;
		PostQuitMessage(WM_QUIT);
	}

	//


	SetTimer(1, 10, NULL);
	
	//

	SetWindowLongW(e_global->hwnd_frame, GWL_EXSTYLE, GetWindowLong(e_global->hwnd_frame, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(e_global->hwnd_frame, 0, 0, LWA_ALPHA); //â����
	//

	if (!autoalready)
	ShellExecute(this->GetSafeHwnd(), TEXT("open"), TEXT("AutoHotkey.exe"), NULL, NULL, SW_HIDE);

	//tray ������ ����
	this->ShowWindow(SW_HIDE);
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = IDR_MAINFRAME;
	nid.hWnd = GetSafeHwnd();

	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	lstrcpy(nid.szTip, TEXT("Expandable"));
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	BOOL bRet = ::Shell_NotifyIcon(NIM_ADD, &nid);
	//E_Global* e_global = E_Global::getSingleton();
	//::SetFocus(e_global->getTaskbarWindow()->m_hWnd);
	//::SetFocus(this->GetSafeHwnd());
	//SendMessage(WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);//Ʈ����ȭ�鿡 ���̱�(UI����)
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	//tray ������ ����





	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("�޴� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// �޴� ������ Ȱ��ȭ�ص� ��Ŀ���� �̵����� �ʰ� �մϴ�.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	// TODO: ���� ���� �� �޴� ������ ��ŷ�� �� ���� �Ϸ��� �� �ټ� ���� �����Ͻʽÿ�.
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// Visual Studio 2005 ��Ÿ�� ��ŷ â ������ Ȱ��ȭ�մϴ�.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 ��Ÿ�� ��ŷ â �ڵ� ���� ������ Ȱ��ȭ�մϴ�.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// ������ ���� ���� ���־� ������ �� ��Ÿ���� �����մϴ�.
	OnApplicationLook(theApp.m_nAppLook);

	// ���� ���� �� ��ŷ â �޴� �ٲٱ⸦ Ȱ��ȭ�մϴ�.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ����(<Alt> Ű�� ���� ä ����) ���� ���� ����� ������ Ȱ��ȭ�մϴ�.
	CMFCToolBar::EnableQuickCustomization();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	//cs.style &= ~WS_THICKFRAME; 
	return TRUE;
}

// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame �޽��� ó����

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* �޴��� �˻��մϴ�. */);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp, LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp, lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	UINT uMouseMsg = (UINT)lParam;
	POINT MousePos;
	CMenu menu, *pPopup;
	UINT id;
	E_Map* e_map = E_Map::getSingleton();
	switch (uMouseMsg)
	{
	case WM_RBUTTONDOWN:
		SetForegroundWindow();
		GetCursorPos(&MousePos);

		menu.LoadMenu(IDR_MAINFRAME);
		pPopup = menu.GetSubMenu(0);
		id = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, MousePos.x, MousePos.y, this);
		switch (id)
		{
		case ID_32775:
			On32775();
			break;
		case ID_APP_EXIT:
			OnDestroy();
			//PostMessage(WM_QUIT);
			break;
		case ID_32777:
			On32777();
			break;

		}
		break;
	case WM_LBUTTONDBLCLK:
		//DestroyTrayIcon();
		//changetray(1);
		e_map->drawMap();
		break;
	}
	return 1;

}

void CMainFrame::DestroyTrayIcon()
{
	NOTIFYICONDATA nid;
	BOOL bRet;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = IDR_MAINFRAME;
	nid.hWnd = GetSafeHwnd();
	bRet = ::Shell_NotifyIcon(NIM_DELETE, &nid);
	//HWND ahk = ::FindWindow(NULL, L"AutoHotKey.exe");
	//::SendMessage(autohwnd, WM_CLOSE, 0, 0);
	//::DestroyWindow(autohwnd);
	


	return;


}

////



void CMainFrame::On32775()
{
	EnviromnmentDialog aboutDlg;

	aboutDlg.DoModal();

	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CMainFrame::OnDestroy()
{
	CFrameWndEx::OnDestroy();
	//������
	Injector(L"explorer.exe", EJECTION_MODE, L"ExpandableDLL_x86.dll");

	DestroyTrayIcon();
	E_Global* e_global = E_Global::getSingleton();


	std::list<E_Desktop*> desklist = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)
	{
		(*itr_desk)->setAllOpaque();
		(*itr_desk)->setAllIconVisible();
		(*itr_desk)->setAllShow();
	}

	//AHK����
	terminateAHK();
	//


	HWND hTaskbarWnd = ::FindWindowW(_T("Shell_TrayWnd"), NULL);
	::SetLayeredWindowAttributes(hTaskbarWnd, 0, 255, LWA_ALPHA); //��������
	::SetWindowLongW(hTaskbarWnd, GWL_EXSTYLE, GetWindowLong(hTaskbarWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);

	PostMessage(WM_QUIT);
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
void CMainFrame::terminateAHK()
{
	if (!alreadyrun)
	{
		CString ProcessName("AutoHotkey");  //������ ���μ��� �̸�
		ProcessName.MakeUpper();
		//ProcessName.Format()
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if ((int)hSnapshot != -1)
		{
			PROCESSENTRY32 pe32;
			pe32.dwSize = sizeof(PROCESSENTRY32);
			BOOL bContinue;
			CString tempProcessName;
			if (Process32First(hSnapshot, &pe32))
			{
				//���μ��� ��� �˻� ����
				do
				{
					tempProcessName = pe32.szExeFile;  //���μ��� ��� �� ���� ���μ��� �̸�;
					tempProcessName.MakeUpper();
					if ((tempProcessName.Find(ProcessName, 0) != -1))
					{
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID);  //���μ��� �ڵ� ���
						if (hProcess)
						{
							DWORD dwExitCode;
							GetExitCodeProcess(hProcess, &dwExitCode);
							TerminateProcess(hProcess, dwExitCode);
							CloseHandle(hProcess);
						}
					}
					bContinue = Process32Next(hSnapshot, &pe32);
				} while (bContinue);
			}
			CloseHandle(hSnapshot);
		}
	}



}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	TRACE_WIN32A("frame on key down %c", nChar);
	switch (nChar){
		//��Ʈ ��
	case 'A':
	{

				//TRACE_WIN32A("��Ʈ ��");
					//�߰�(DesktopSwitcher �Ǵ� Drag&DropSwitcher Breaking
					if (E_DesktopSwitcher::getSingleton()->ison || E_DragAndDropSwitcher::getSingleton()->started)
						return;

				E_WindowSwitcher* switcher = E_WindowSwitcher::getSingleton();
				if (switcher->isRunning() == false){
					//����ó�� �������� �ƴ� ��
					switcher->startSwitcher();
					keydown = 1;
					startChecking();
					switcher->stealFocus2(switcher->GetSafeHwnd());
					
				}
				else{
					//����Ʈ ��
				//	switcher->stealFocus2(switcher->GetSafeHwnd());
					bool shift = GetKeyState(VK_LSHIFT) < 0 ? true : false;
					if (shift == false){
						E_WindowSwitcher::getSingleton()->selectNextWindow();
					}
					else{
						E_WindowSwitcher::getSingleton()->selectPrevWindow();
					}
				}
	}
		break;
		//��Ʈ ��
	case 'B':
	{
				E_WindowSwitcher* switcher = E_WindowSwitcher::getSingleton();
				TRACE_WIN32A("ALT UP");
				if (switcher->isRunning() == true){
					TRACE_WIN32A("ALT UP [true]");
					//����ó�� �������� �ƴ� ��
					switcher->selectTabWindow();
					switcher->terminateSwitcher();
				}
	}
		break;
	case 'C':
		//����ũž ����ó
	{
		if (E_WindowSwitcher::getSingleton()->isRunning() || E_DragAndDropSwitcher::getSingleton()->started)
			return;
		if (!E_AeroPeekController::getSingleton()->isAeroPeekMode())
			MessageBoxEx(GetDesktopWindow()->m_hWnd, TEXT("����ũž ����ó�� �̿��Ϸ���\nAero�� ���ּ���."), TEXT("Not Apply"), MB_OK, 0);
		else
			E_DesktopSwitcher::getSingleton()->startSwitcher();
	}
		break;
	case 'D':
		// ��Ʈ�� ��Ʈ ��
	{
					//�߰�(DesktopSwitcher �Ǵ� Drag&DropSwitcher Breaking
					if (E_DesktopSwitcher::getSingleton()->ison || E_DragAndDropSwitcher::getSingleton()->started)
						return;
				//TRACE_WIN32A("��Ʈ�� ��Ʈ ��");
				E_WindowSwitcher* switcher = E_WindowSwitcher::getSingleton();
				if (switcher->isRunning() == false){
					//����ó�� �������� �ƴ� ��
					switcher->startSwitcher();
					keydown = 1;
					switcher->stealFocus2(switcher->GetSafeHwnd());
				}
				else{
					//����Ʈ ��
					//	switcher->stealFocus2(switcher->GetSafeHwnd());
					bool shift = GetKeyState(VK_LSHIFT) < 0 ? true : false;
					if (shift == false){
						E_WindowSwitcher::getSingleton()->selectNextWindow();
					}
					else{
						E_WindowSwitcher::getSingleton()->selectPrevWindow();

					}
				}
	}
	}

	CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	TRACE_WIN32A("frame on key up");
	CFrameWndEx::OnKeyUp(nChar, nRepCnt, nFlags);
}


//void CMainFrame::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	TRACE_WIN32A("OnSysKeyDown");
//	CFrameWndEx::OnSysKeyDown(nChar, nRepCnt, nFlags);
//}


//void CMainFrame::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	TRACE_WIN32A("OnSysKeyUp");
//	CFrameWndEx::OnSysKeyUp(nChar, nRepCnt, nFlags);
//}


// alt up üŷ
int CMainFrame::startChecking()
{
	if (keydown == 0){
		return 0;
	}
	if (isCheckRunning == false && t == NULL){
		for (list<std::thread*>::iterator itr = thread_list.begin(); itr != thread_list.end(); itr++){
			delete  *itr;
		}
		thread_list.clear();
		isCheckRunning = true;
		t = new thread{ &CMainFrame::onChecking, this };
		//SetTimer(5, 3000, NULL);
	}
	return 0;
}

// Alt ���� ������ âȿ�� Ÿ�̸� �����
//void CMainFrame::restartTimer()
//{
//	if (startAnimate == true){
//		
//	}
//	else{
//		KillTimer(5);
//		SetTimer(5, 3000, NULL);
//	}
//}


int CMainFrame::stopChecking()
{
	keydown = 0;
	isCheckRunning = false;
	t->detach();
	thread_list.push_back(t);
	t = NULL;
	//startAnimate = false;
	return 0;
}


int CMainFrame::onChecking()
{
	while (isCheckRunning)
	{
		if (GetKeyState(VK_MENU) < 0)
		{
			//TRACE_WIN32A("ALT KEY DOWN\n");
		}
		else
		{
			//TRACE_WIN32A("ALT KEY UP\n");
			stopChecking();

			::SendMessage(this->GetSafeHwnd(), WM_KEYDOWN, 'B', 0);
		}
	}
	return 0;
}



void CMainFrame::On32777()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	ShowMenu menu;

	menu.DoModal();
}


void CMainFrame::changetray(int num)
{
	DestroyTrayIcon();
	//::BringWindowToTop(this->GetSafeHwnd());
	//this->ShowWindow(SW_HIDE);
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = IDR_MAINFRAME;
	nid.hWnd = GetSafeHwnd();

	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	switch (num)
	{
	case 1:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
		break;
	case 2:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
		break;
	case 3:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
		break;
	case 4:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON4);
		break;
	case 5:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON5);
		break;
	case 6:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON6);
		break;
	case 7:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON7);
		break;
	case 8:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON8);
		break;
	case 9:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON9);
		break;
	case 10:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON10);
		break;
	case 11:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON11);
		break;
	case 12:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON12);
		break;
	case 13:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON13);
		break;
	case 14:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON14);
		break;
	case 15:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON15);
		break;
	case 16:
		nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON16);
		break;
	}
	lstrcpy(nid.szTip, TEXT("Expandable"));
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	BOOL bRet = ::Shell_NotifyIcon(NIM_ADD, &nid);
	::SendMessage(E_Map::getSingleton()->hwnd_cwnd_emap->m_hWnd, WM_INVALIDATE, (WPARAM)hwnd, 1);

	//E_Map* e_map = E_Map::getSingleton();
	//e_map->leave2 = true;
	//::BringWindowToTop(e_map->maphwnd);
}


void CMainFrame::On32778()
{
	//����
	E_Map* e_map = E_Map::getSingleton();
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	E_Global* e_global = E_Global::getSingleton();
	std::list<HWND> docklist = e_global->dockedWindowList;
	for (std::list<HWND>::iterator itr_dock = docklist.begin(); itr_dock != docklist.end(); itr_dock++)
	{
		if ((*itr_dock) == hwnd)
		{
			e_global->dockedWindowList.remove((*itr_dock));
			e_global->dockcount--;
		}
	}
	
	::SendMessage(hwnd, WM_CLOSE, 0, 0);
	::SendMessage(e_map->hwnd_cwnd_emap->m_hWnd, WM_INVALIDATE, (WPARAM)hwnd, 0);

}


void CMainFrame::On32779()
{
	//��ŷ
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	E_Map* e_map = E_Map::getSingleton();
	E_Global* e_global = E_Global::getSingleton();
	
	e_global->dockedWindowList.push_back(hwnd);



	
	std::list<E_Desktop*> desklist = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist.begin(); itr_desk != desklist.end(); itr_desk++)	//�� ����ũž �������
	{
		std::list<E_Window*> winlist = (*itr_desk)->getWindowList();
		for (std::list<E_Window*>::iterator itr_window = winlist.begin(); itr_window != winlist.end(); itr_window++)	//�� ����ũž �������
		{
			if ((*itr_window)->getWindow() == hwnd)
			{
				(*itr_window)->dock = true;
				e_global->dockcount++;
				::SendMessage(e_map->hwnd_cwnd_emap->m_hWnd, WM_INVALIDATE, (WPARAM)hwnd, 1);
				if (e_global->getSelectedIndex() != (*itr_desk)->getIndex())
					::ShowWindow(hwnd, SW_SHOW);

				e_global->hwnd_desk.insert(hash_map<HWND, int>::value_type((*itr_window)->getWindow(), (*itr_desk)->getIndex()));


				return;
			}
		}
	}
	return;
}


void CMainFrame::On32781()
{
	//��ŷ����
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	E_Global* e_global = E_Global::getSingleton();
	E_Map* e_map = E_Map::getSingleton();

	std::list<HWND> docklist = e_global->dockedWindowList;
	for (std::list<HWND>::iterator itr_dock = docklist.begin(); itr_dock != docklist.end(); itr_dock++)
	{
		if ((*itr_dock) == hwnd)
		{
			e_global->dockedWindowList.remove((*itr_dock));
			e_global->dockcount--;
		}
	}
	
	std::list<E_Desktop*> desklist2 = e_global->desktopList;
	for (std::list<E_Desktop*>::iterator itr_desk = desklist2.begin(); itr_desk != desklist2.end(); itr_desk++)	//�� ����ũž �������
	{
		std::list<E_Window*> winlist2 = (*itr_desk)->getWindowList();
		for (std::list<E_Window*>::iterator itr_window = winlist2.begin(); itr_window != winlist2.end(); itr_window++)	//�� ����ũž �������
		{
			if ((*itr_window)->getWindow() == hwnd)
			{
				(*itr_window)->dock = false;
				::SendMessage(e_map->hwnd_cwnd_emap->m_hWnd, WM_INVALIDATE, (WPARAM)hwnd, 1);
				if (e_global->getSelectedIndex() != (*itr_desk)->getIndex())
					::ShowWindow(hwnd, SW_HIDE);

				e_global->hwnd_desk.erase(e_global->hwnd_desk.find((*itr_window)->getWindow()));

				return;
			}
		}

	}
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == 1)
	{
		if (!icondisable)
		{
			icondisable = true;
			E_Window::setIconInvisible(E_Global::getSingleton()->hwnd_frame);
			::MoveWindow(this->GetSafeHwnd(), E_EnvironmentManager::getSingleton()->getWidth(), E_EnvironmentManager::getSingleton()->getHeight(), 200, 200, TRUE);
			KillTimer(1);

			bool dualMonitorMode = false;
			//Some code for resolution...
			int monitorCount = GetSystemMetrics(SM_CMONITORS);
			if (monitorCount > 1) {
				HWND hwnd = GetDesktopWindow()->m_hWnd;
				MessageBoxEx(hwnd, TEXT("��� ����ʹ� �������� �ʽ��ϴ�.\n\nExpandable �����մϴ�."), TEXT("Not Apply"), MB_OK, 0);
				terminateAHK();
				PostQuitMessage(WM_QUIT);
			}
		}
	}
	//if (nIDEvent == 5)
	//{
	//	startAnimate = true;
	//	KillTimer(5);
	//	//â ��׶��� ȿ�� ����
	//	E_WindowSwitcher::getSingleton()->enableAnimate();
	//}
	
	CFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	//TRACE_WIN32A("OnSysCommand");
	CFrameWndEx::OnSysCommand(nID, lParam);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED){
		//TRACE_WIN32A("CMAINFRAME ONSIZE");
		this->ShowWindow(SW_SHOWNORMAL);
	}
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

HRESULT CMainFrame::OnChange(WPARAM wParam, LPARAM lParam)
{
	E_DesktopSwitcher::getSingleton()->DestroyWindow();
	E_DesktopSwitcher::getSingleton()->initial = true;
	bool dualMonitorMode = false;
	//Some code for resolution...
	int monitorCount = GetSystemMetrics(SM_CMONITORS);
	if (monitorCount > 1) {
		MessageBoxEx(GetDesktopWindow()->m_hWnd, TEXT("��� ����ʹ� �������� �ʽ��ϴ�.\n\nExpandable �����մϴ�."), TEXT("Not Apply"), MB_OK, 0);
		terminateAHK();
		PostQuitMessage(WM_QUIT);
	}


	return true;
}