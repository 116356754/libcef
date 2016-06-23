#include "CefFacade.h"

#include   <shlobj.h>   
#include <tchar.h>

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\libcef")
#pragma comment(lib, "..\\Debug\\libcef_dll_wrapper")
#else
#pragma comment(lib, "..\\Release\\libcef")
#pragma comment(lib, "..\\Release\\libcef_dll_wrapper")
#endif

std::string string_To_UTF8(const std::string & str) 
{ 
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0); 

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2); 

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen); 

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL); 

	char * pBuf = new char[nLen + 1]; 
	ZeroMemory(pBuf, nLen + 1); 

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL); 

	std::string retStr(pBuf); 
	delete []pwBuf; 
	delete []pBuf; 

	pwBuf = NULL; 
	pBuf  = NULL; 
	return retStr; 
} 

CefFacade::CefFacade(const char* startUrl,HINSTANCE hInstance,HWND pParent)
{
	CefMainArgs main_args(hInstance);  

	CefRefPtr<ClientApp> app(new ClientApp);  

	int exit_code = CefExecuteProcess(main_args, app.get(), NULL);  
	if (exit_code >= 0){  
		exit(exit_code);  
	}  

	CefSettings settings;  
	CefSettingsTraits::init(&settings);  
	settings.multi_threaded_message_loop = true;  
	settings.single_process = true;
	TCHAR szBuffer[MAX_PATH];  
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_INTERNET_CACHE, FALSE);   
	_stprintf(szBuffer,_T("%s\\cache"), szBuffer);  
	CefString(&settings.cache_path) = szBuffer;

	settings.log_severity = LOGSEVERITY_DISABLE;
	CefString(&settings.locale) =  "zh-CN";

	CefInitialize(main_args, settings, app.get(), NULL);  

	CreateBrowser(pParent,startUrl);
}
 

void CefFacade::CreateBrowser(HWND Parent_,const char* startUrl_,pFun pFn_)
{	
	m_clientHandler = new ClientHandler(pFn_); 

	CefWindowInfo info;  
	CefBrowserSettings b_settings;  

	RECT rect;
	GetClientRect(Parent_,&rect);
	info.SetAsChild(Parent_, rect);  

	if(strlen(startUrl_)==0)
		m_StartupURL="about://blank";
	else
		m_StartupURL = string_To_UTF8(startUrl_);
	CefBrowserHost::CreateBrowser(info, m_clientHandler.get(), m_StartupURL, b_settings, NULL);
}

CefFacade::~CefFacade(void)
{
	CefShutdown();
}

void CefFacade::LoadUrl( const char* url_/*=NULL */ )
{
	if(strlen(url_)==0)	
		return;
	m_StartupURL = string_To_UTF8(url_);	

	m_clientHandler->GetBrowser()->GetMainFrame()->LoadURL(m_StartupURL);
}

void CefFacade::OnSize( const RECT newrc )
{
	if(m_clientHandler.get())
	{
		CefRefPtr<CefBrowser>browser = m_clientHandler->GetBrowser();
		if(browser)
		{
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			::MoveWindow(hwnd,0,0,newrc.right-newrc.left,newrc.bottom-newrc.top, true); 
		}
	}
}

//Interfacecef * __stdcall  CreateInterfaceModule(const char* startUrl,HINSTANCE hInstance,HWND pParent)
//{
//	Interfacecef * pCefPlug = (Interfacecef *) new CefFacade(startUrl,hInstance,pParent);

//	return pCefPlug;
//}

extern "C"
{
	__declspec(dllexport) void Init(HINSTANCE hInstance,bool bSingleProcess =true)
	{
		CefMainArgs main_args(hInstance);  

		CefRefPtr<ClientApp> app(new ClientApp);  

		int exit_code = CefExecuteProcess(main_args, app.get(), NULL);  
		if (exit_code >= 0){  
			exit(exit_code);  
		}  

		CefSettings settings;  
		CefSettingsTraits::init(&settings);  
		settings.multi_threaded_message_loop = true;  
		settings.single_process = bSingleProcess;
		TCHAR szBuffer[MAX_PATH];  
		SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_INTERNET_CACHE, FALSE);   
		_stprintf(szBuffer,_T("%s\\cache"), szBuffer);  
		//CefString(&settings.cache_path) = szBuffer;
		CefString(&settings.cache_path).FromWString(szBuffer);
		settings.log_severity = LOGSEVERITY_DISABLE;
		CefString(&settings.locale) =  "zh-CN";

		CefInitialize(main_args, settings, app.get(), NULL);  
	}
}

typedef void(*Fun)(int); //定义函数指针类型  

static CefRefPtr<ClientHandler> s_clientHandler;
extern "C"
{
	__declspec(dllexport) void  CreateBrowser(HWND Parent_,const char* startUrl_,Fun stateFn_)
	{
		s_clientHandler = new ClientHandler(stateFn_); 

		CefWindowInfo info;  
		CefBrowserSettings b_settings;  

		RECT rect;
		GetClientRect(Parent_,&rect);
		info.SetAsChild(Parent_, rect);  
		std::string m_StartupURL;
		if(strlen(startUrl_)==0)
			m_StartupURL="about://blank";
		else
			m_StartupURL = string_To_UTF8(startUrl_);
		CefBrowserHost::CreateBrowser(info, s_clientHandler.get(), m_StartupURL, b_settings, NULL);
	}
}

extern "C"
{
	__declspec(dllexport) void LoadUrl( const char* url_/*=NULL */ )
	{
		if(strlen(url_)==0)	
			return;
		std::string m_StartupURL;
		m_StartupURL = string_To_UTF8(url_);	

		s_clientHandler->GetBrowser()->GetMainFrame()->LoadURL(m_StartupURL);
	}
}

extern "C"
{
	__declspec(dllexport) void OnSize( const RECT newrc )
	{
		if(s_clientHandler.get())
		{
			CefRefPtr<CefBrowser>browser = s_clientHandler->GetBrowser();
			if(browser)
			{
				CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
				::MoveWindow(hwnd,0,0,newrc.right-newrc.left,newrc.bottom-newrc.top, true); 
			}
		}
	}
}

extern "C"
{
	__declspec(dllexport) void ShutDown()
	{
		CefShutdown();
	}
}