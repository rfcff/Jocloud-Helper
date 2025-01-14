// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "scene/main/ui/uiMainDlg.h"
#include "../src/scene/public/SouiRealWnd.h"
#include "../src/scene/public/PopupRealWnd.h"
//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 0
//#define SYSRES_TYPE 0
// #define RES_TYPE 0   //PE
// #define RES_TYPE 1   //ZIP
// #define RES_TYPE 2   //7z
// #define RES_TYPE 2   //文件
//去掉多项支持，以免代码显得混乱
#if (RES_TYPE==1)
#include "resprovider-zip\zipresprovider-param.h"
#else 
#if (RES_TYPE==2)
#include "resprovider-7zip\zip7resprovider-param.h"
#endif
#endif
#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

static const char* TAG = "SOUIEngine";

//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP

class SOUIEngine
{
private:
	SComMgr m_ComMgr;
	SApplication *m_theApp;
	bool m_bInitSucessed;
public:
	SOUIEngine(HINSTANCE hInstance):m_theApp(NULL), m_bInitSucessed(false){
		
		CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
		BOOL bLoaded = FALSE;
		//使用GDI渲染界面
		bLoaded = m_ComMgr.CreateRender_GDI((IObjRef * *)& pRenderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
		//设置图像解码引擎。默认为GDIP。基本主流图片都能解码。系统自带，无需其它库
		CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
		bLoaded = m_ComMgr.CreateImgDecoder((IObjRef * *)& pImgDecoderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));

		pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
		m_theApp = new SApplication(pRenderFactory, hInstance);	
		m_bInitSucessed = (TRUE==bLoaded);
	};
	operator bool()const
	{
		return m_bInitSucessed;
	}
	//加载系统资源
	bool LoadSystemRes()
	{
		BOOL bLoaded = FALSE;

		//从DLL加载系统资源
		{
			HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
			if (hModSysResource)
			{
				CAutoRefPtr<IResProvider> sysResProvider;
				CreateResProvider(RES_PE, (IObjRef * *)& sysResProvider);
				sysResProvider->Init((WPARAM)hModSysResource, 0);
				m_theApp->LoadSystemNamedResource(sysResProvider);
				FreeLibrary(hModSysResource);
			}
			else
			{
				SASSERT(0);
			}
		}

		return TRUE==bLoaded;
	}
	//加载用户资源
	bool LoadUserRes()
	{
		CAutoRefPtr<IResProvider>   pResProvider;
		BOOL bLoaded = FALSE;
#ifdef _DEBUG		
		//选择了仅在Release版本打包资源则在DEBUG下始终使用文件加载
		{
			CreateResProvider(RES_FILE, (IObjRef * *)& pResProvider);
			bLoaded = pResProvider->Init((LPARAM)_T("uires"), 0);
			SASSERT(bLoaded);
		}
#else
		{
			CreateResProvider(RES_PE, (IObjRef * *)& pResProvider);
			bLoaded = pResProvider->Init((WPARAM)m_theApp->GetInstance(), 0);
			SASSERT(bLoaded);
		}
#endif
		m_theApp->InitXmlNamedID(namedXmlID, ARRAYSIZE(namedXmlID), TRUE);
		m_theApp->AddResProvider(pResProvider);

		pugi::xml_document xmlDoc;
		bool ret = m_theApp->LoadXmlDocment(xmlDoc, _T("LAYOUT:xml_messagebox"));
		SetMsgTemplate(xmlDoc.first_child());
		return TRUE==bLoaded;
	}
	//加载LUA支持
	bool LoadLUAModule()
	{
		BOOL bLoaded =FALSE;
		CAutoRefPtr<SOUI::IScriptFactory> pScriptLuaFactory;
		bLoaded = m_ComMgr.CreateScrpit_Lua((IObjRef * *)& pScriptLuaFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("scirpt_lua"));
		m_theApp->SetScriptFactory(pScriptLuaFactory);
		return TRUE == bLoaded;
	}
	//加载多语言支持
	bool LoadMultiLangsModule()
	{
		BOOL bLoaded = FALSE;
		CAutoRefPtr<ITranslatorMgr> trans;
		bLoaded = m_ComMgr.CreateTranslator((IObjRef * *)& trans);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("translator"));
		if (trans)
		{//加载语言翻译包
			m_theApp->SetTranslator(trans);
			pugi::xml_document xmlLang;
			if (m_theApp->LoadXmlDocment(xmlLang,  _T("translator:lang_cn")))
			{
				CAutoRefPtr<ITranslator> langCN;
				trans->CreateTranslator(&langCN);
				langCN->Load(&xmlLang.child(L"language"), 1);//1=LD_XML
				trans->InstallTranslator(langCN);
			}

			
		}
		return TRUE == bLoaded;
	}
	//注册用户自定义皮肤和控件
	void Regitercustom()
	{
		m_theApp->RegisterWindowClass<SGroupList>();
		m_theApp->RegisterWindowClass<SAniWindow>();
		m_theApp->RegisterWindowClass<SCheckBox2>();
		//m_theApp->RegisterWindowClass<CPopupRealWnd>();
	}

	~SOUIEngine()
	{
		if (m_theApp)
		{
			delete m_theApp;
			m_theApp = NULL;
		}
	}

	template<class MainWnd>
	int Run()
	{
		MainWnd dlgMain;
		/*dlgMain.Create(GetActiveWindow());
		dlgMain.SendMessage(WM_INITDIALOG);
		dlgMain.CenterWindow(dlgMain.m_hWnd);
		dlgMain.ShowWindow(SW_SHOWNORMAL);
		MediaManager::instance()->setMessageDlg(dlgMain.m_hWnd);
		*/

		int cxScreen, cyScreen;
		cxScreen = GetSystemMetrics(SM_CXSCREEN);//获取显示器屏幕宽度像素值
		cyScreen = GetSystemMetrics(SM_CYSCREEN);//获取显示器屏幕高度像素值

		dlgMain.Create(GetActiveWindow(), (cxScreen-1349)/2, (cyScreen-768)/2, -1, -1);
		dlgMain.SendMessage(WM_INITDIALOG);
		//dlgMain.CenterWindow(dlgMain.m_hWnd);
		dlgMain.ShowWindow(SW_SHOWNORMAL);
		MediaManager::instance()->setMessageDlg(dlgMain.m_hWnd);

		return m_theApp->Run(dlgMain.m_hWnd);
	}

	SApplication* GetApp()
	{
		return m_theApp;
	}
};
//debug时方便调试设置当前目录以便从文件加载资源
void SetDefaultDir()
{
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));

	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
#ifdef _DEBUG
	_tcscpy(lpInsertPos + 1, _T("..\\AivacomAdvanceScene-PC"));
#else
	_tcscpy(lpInsertPos + 1, _T("\0"));
#endif
	SetCurrentDirectory(szCurrentDir);
}

static void createLogDir(std::string& path) {
	// 1. Determine whether the log directory has been created, if not, do not create it
	// 2. Create a directory
	if (_access(path.c_str(), 0) == -1) {
		if (::CreateDirectoryA(path.c_str(), NULL)) {
			Logd(TAG, Log("createLogDir").setMessage("Dir path[%s] create OK!!!!", path.c_str()));
		}
		else {
			Logd(TAG, Log("createLogDir").setMessage("Dir path[%s] create Failed!!!!", path.c_str()));
		}
	}
	else {
		Logd(TAG, Log("createLogDir").setMessage("Dir path[%s] is exist!!!!", path.c_str()));
	}
}

static std::string getModuleDir() {
	HMODULE module = GetModuleHandle(0);
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(module, szCurrentDir, sizeof(szCurrentDir));
	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
	int pos = lpInsertPos - (LPTSTR)(szCurrentDir);
	std::string path = ws2s(szCurrentDir);
	return path.substr(0, pos);
}

static void initLogFile() {
	// 1. Get the current directory address
	std::string localPath = getModuleDir();
	if (localPath.empty()) {
		Logw(TAG, Log("initLogFile").setMessage("Get localPath is empty!!!"));
		return;
	}

	// 2. Determine whether the log directory has been created, if not, do not create it
	std::string logPath = localPath + "\\log";
	std::string exePath = logPath + "\\advance";
	std::string sdkPath = logPath + "\\sdk";
	createLogDir(logPath);
	createLogDir(exePath);
	createLogDir(sdkPath);

	MediaManager::instance()->getThunderManager()->setLogFilePath(sdkPath.c_str());

#ifdef DEBUG
	MediaManager::instance()->getThunderManager()->setLogLevel(LOG_LEVEL_TRACE);
#else
	// 3. Create a log file
	CreateLogFile(exePath, "advance");

	// 4. set sdk log level
	MediaManager::instance()->getThunderManager()->setLogLevel(LOG_LEVEL_WARN);

	// 5. if set sdkPath += L"\\1.txt", set sdk log level is LOG_LEVEL_TRACE, means all log
	sdkPath += "\\1.txt";
	if (_access(sdkPath.c_str(), 0) != -1) {
		MediaManager::instance()->getThunderManager()->setLogLevel(LOG_LEVEL_TRACE);
	}
#endif // DEBUG
}

static void uninitLogFile() {
	DestoryLogFile();
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));
	MediaManager::create();
	MediaManager::instance()->init(g_AppId, 0);
	initLogFile();
	MSJExceptionHandler crashHandle("Jocloud");
	SetDefaultDir();
	int nRet = 0;
	{
		SOUIEngine souiEngine(hInstance);
		if (souiEngine)
		{
			//加载系统资源
			souiEngine.LoadSystemRes();
			//加载用户资源
			souiEngine.LoadUserRes();
			//注册用户自定义皮肤/控件/布局/其它自定义
			souiEngine.Regitercustom();
			//加载多语言翻译模块。
			souiEngine.LoadMultiLangsModule();

			//设置真窗口处理接口
			CSouiRealWnd * pRealWndHandler = new CSouiRealWnd();
			souiEngine.GetApp()->SetRealWndHandler(pRealWndHandler);
			pRealWndHandler->Release();

			nRet = souiEngine.Run<UIMainDlg>();
		}
		else
		{
			MessageBox(NULL, _T("无法正常初使化SOUI"), _T("错误"), MB_OK | MB_ICONERROR);
		}
	}
	uninitLogFile();
	MediaManager::release();
	OleUninitialize();
	return nRet;
}
