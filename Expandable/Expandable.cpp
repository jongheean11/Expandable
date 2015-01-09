
// Expandable.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Expandable.h"
#include "MainFrm.h"

#include "ExpandableDoc.h"
#include "ExpandableView.h"

#include "E_EnvironmentManager.h"
#include "E_GlobalUpdater.h"
#include "E_Global.h"
#include "E_WindowSwitcher.h"

#include "E_WindowSwitcherTest.h"
#include "E_GlobalTest.h"
#include "E_WindowTest.h"
#include "E_UtilTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExpandableApp

BEGIN_MESSAGE_MAP(CExpandableApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CExpandableApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CExpandableApp ����

CExpandableApp::CExpandableApp()
{
	m_bHiColorIcons = TRUE;

	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���� ���α׷��� ���� ��� ��Ÿ�� ������ ����Ͽ� ������ ���(/clr):
	//     1) �� �߰� ������ �ٽ� ���� ������ ������ ����� �۵��ϴ� �� �ʿ��մϴ�.
	//     2) ������Ʈ���� �����Ϸ��� System.Windows.Forms�� ���� ������ �߰��ؾ� �մϴ�.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Expandable.AppID.NoVersion"));

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

// ������ CExpandableApp ��ü�Դϴ�.

CExpandableApp theApp;


// CExpandableApp �ʱ�ȭ

BOOL CExpandableApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	// AfxInitRichEdit2();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�.  ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CExpandableDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CExpandableView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	//â �̸� ����
	m_pMainWnd->SetWindowTextW(E_Global::testFrameName);


	//E_Global �迭 �ʱ�ȭ
	E_Global* cwnd_global = E_Global::getSingleton();
	cwnd_global->init();

	//Switcher �迭 �ʱ�ȭ
	
	//Window Switcher �ʱ�ȭ
	CWnd* cwnd_windowSwicher = E_WindowSwitcher::getSingleton();

	CBrush brush_window;
	UINT nClassStyle_window = 0;// CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName_window = AfxRegisterWndClass(nClassStyle_window, 0, (HBRUSH)CreateSolidBrush(E_WindowSwitcher::backgroundColor), 0);
	cwnd_windowSwicher->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName_window, E_WindowSwitcher::caption, WS_VISIBLE | WS_POPUP, CRect(0, 0, 200, 400), CWnd::GetDesktopWindow(), 0);
	
	CDC* pDC = cwnd_windowSwicher->GetDC();
	pDC->SetBkColor(RGB(0x0, 0x0, 0x0));
	pDC->SetBkMode(TRANSPARENT);

	cwnd_windowSwicher->ShowWindow(SW_HIDE);
	cwnd_windowSwicher->UpdateWindow();

	//Environment Manager �ʱ�ȭ
	CWnd* cwnd_env = (CWnd*)E_EnvironmentManager::getSingleton();
	((E_EnvironmentManager*)cwnd_env)->setGlobalUpdater((E_GlobalUpdater*)cwnd_global);
	
	CBrush m_oBkgndBrush;
	m_oBkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
	UINT nClassStyle = CS_NOCLOSE | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	CString szClassName = AfxRegisterWndClass(nClassStyle, 0, (HBRUSH)m_oBkgndBrush.GetSafeHandle(), 0);

	cwnd_env->Create(szClassName, _T(""), WS_VISIBLE, CRect(0, 0, 200, 400), CWnd::GetDesktopWindow(), 1234);
	//SetClassLong(cwnd_env->)
	// nID : ID of the Window -> ����ȵ��� : �ش� ID�� affordable���� üũ �� �� ����.
	cwnd_env->ShowWindow(SW_HIDE);
	cwnd_env->UpdateWindow();
	
	//Unit Test
	E_WindowSwitcherTest::testGetAeroSize();
	E_WindowSwitcherTest::testGetIconSize();
	E_WindowSwitcherTest::testGetPaddingSize();
	E_GlobalTest::testGetKakaoWindow();
	//E_GlobalTest::testGetAllWindows();
	E_WindowSwitcherTest::testGetShape();
	//E_WindowTest::testTakeScreenshot();
	E_UtilTest::testGetDefaultIconSize();
	E_UtilTest::testGetIconHandle();
	//E_UtilTest::testConvertIconToBitmape();


	
	return TRUE;
}

// CExpandableApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CExpandableApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CExpandableApp ����� ���� �ε�/���� �޼���

void CExpandableApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CExpandableApp::LoadCustomState()
{
}

void CExpandableApp::SaveCustomState()
{
}

// CExpandableApp �޽��� ó����



