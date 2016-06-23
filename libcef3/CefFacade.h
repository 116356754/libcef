#pragma once

#include "include/cef_client.h"
#include "include/cef_app.h"

#include "ClientApp.h"
#include "ClientHandler.h"

#include <windows.h>
#include <string>
#include "ModuleInterface.h"


typedef void(*pFun)(int); //定义函数指针类型  

class CefFacade:public Interfacecef
{
public:
	CefFacade(const char* startUrl,HINSTANCE hInstance,HWND pParent);
	virtual ~CefFacade(void);

	virtual void CreateBrowser(HWND Parent_,const char* startUrl_=NULL,pFun pFn_=NULL);
	virtual void LoadUrl( const char*  url_=NULL );

	virtual void OnSize( const RECT newrc );
private:
	std::string m_StartupURL;
	CefRefPtr<ClientHandler> m_clientHandler;
};



