#include "include/cef_client.h"
#include "include/cef_app.h"

#include "ClientApp.h"
#include "ClientHandler.h"

#include <windows.h>
#include <string>
#include   <shlobj.h>   
#include <tchar.h>

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\libcef")
#pragma comment(lib, "..\\Debug\\libcef_dll_wrapper")
#else
#pragma comment(lib, "..\\Release\\libcef")
#pragma comment(lib, "..\\Release\\libcef_dll_wrapper")
#endif

typedef void(*Fun)(int); //定义函数指针类型  

static CefRefPtr<ClientHandler> s_clientHandler;

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


static bool s_ignore_Cert_Err = false; 

extern "C"
{

	void __stdcall IgnoreCertErr(bool _bIgnore_Cert_Err =true)
	{
		s_ignore_Cert_Err = _bIgnore_Cert_Err;
	}

	void __stdcall  Init(HINSTANCE hInstance,bool bSingleProcess =true)
	{
		SetErrorMode(SEM_NOGPFAULTERRORBOX|SEM_FAILCRITICALERRORS|SEM_NOALIGNMENTFAULTEXCEPT|SEM_NOOPENFILEERRORBOX);
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
		settings.ignore_certificate_errors = s_ignore_Cert_Err;

		CefInitialize(main_args, settings, app.get(), NULL);  
	}

	void __stdcall CreateBrowser(HWND Parent_,const char* startUrl_=NULL)
	{
		s_clientHandler = new ClientHandler(NULL); 

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

	void __stdcall CreateBrowser_CallBack(HWND Parent_,const char* startUrl_=NULL,Fun stateFn_=NULL)
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


	void __stdcall LoadUrl( const char* url_/*=NULL */ )
	{
		if(strlen(url_)==0)	
			return;
		std::string m_StartupURL;
		m_StartupURL = string_To_UTF8(url_);	

		s_clientHandler->GetBrowser()->GetMainFrame()->LoadURL(m_StartupURL);
	}


	void __stdcall OnSize( const RECT newrc )
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

	void __stdcall ShutDown()
	{
		CefShutdown();
	}	
}

BOOL WINAPI DllMain(HINSTANCE hInstDll,DWORD fwReason,PVOID fLoad){
	switch(fwReason){
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		
		break;
	}
	return TRUE;
}