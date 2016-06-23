 /************************************************************************************************ 
    *   Copyright (c) 2013 Álan Crístoffer 
    * 
    *   Permission is hereby granted, free of charge, to any person obtaining a copy 
    *   of this software and associated documentation files (the "Software"), to deal 
    *   in the Software without restriction, including without limitation the rights 
    *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
    *   of the Software, and to permit persons to whom the Software is furnished to do so, 
    *   subject to the following conditions: 
    * 
    *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
    *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
    *   PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
    *   FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
    *   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    *   DEALINGS IN THE SOFTWARE. 
    ************************************************************************************************/  

    #include "ClientHandler.h"  
      
    #include "include/cef_app.h"  
    #include "include/cef_base.h"  
    #include "include/cef_client.h"  
    #include "include/cef_command_line.h"  
    #include "include/cef_frame.h"  
    #include "include/cef_runnable.h"  
    #include "include/cef_web_plugin.h"  
      
	#include <tchar.h>

	

    ClientHandler::ClientHandler(pFun pFn_)
    {  
		m_statFn = pFn_;
    }  
      
    bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser)  
    {  
        return false;  
    }  
      
    void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)  
    {  
        if ( !m_Browser.get() ) {  
            // We need to keep the main child window, but not popup windows  
            m_Browser     = browser;  
            m_BrowserHwnd = browser->GetHost()->GetWindowHandle();  
        }  
    }  
      
    void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)  
    {  
        if ( m_BrowserHwnd == browser->GetHost()->GetWindowHandle() ) {  
            // Free the browser pointer so that the browser can be destroyed  
            m_Browser = NULL;  
        }  
    }  

	void ClientHandler::OnTitleChange( CefRefPtr<CefBrowser> browser, const CefString& title )
	{
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		SetWindowText(hwnd, std::wstring(title).c_str());
	}

	void ClientHandler::OnLoadStart( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame )
	{
		/*SYSTEMTIME st;
		GetSystemTime(&st);
		char buf[255]={0};
		sprintf(buf,"%d:%d",st.wSecond,st.wMilliseconds);*/
		//OutputDebugString((LPCWSTR)buf);

		OutputDebugString(_T("OnLoadStart \n"));
		start = GetTickCount();  

	}

	void ClientHandler::OnLoadEnd( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode )
	{		
		OutputDebugString(_T("OnLoadEnd:"));
		double  ellapse=GetTickCount()-start;  

		OutputDebugString(frame->GetURL().c_str());

		char buf[255]={0};
		sprintf(buf,":%f\n",ellapse);		

		WCHAR wszClassName[256];  
		memset(wszClassName,0,sizeof(wszClassName));  
		MultiByteToWideChar(CP_ACP,0,buf,strlen(buf)+1,wszClassName,  
			sizeof(wszClassName)/sizeof(wszClassName[0]));  

		OutputDebugString(wszClassName);

		sprintf(buf,":%d\n",httpStatusCode);
		memset(wszClassName,0,sizeof(wszClassName));  
		MultiByteToWideChar(CP_ACP,0,buf,strlen(buf)+1,wszClassName,  
			sizeof(wszClassName)/sizeof(wszClassName[0]));  
		OutputDebugString(wszClassName);

		if(m_statFn!=NULL)
			m_statFn(httpStatusCode);
		//printf("GetTickCount:%f",end-start);  
		//s 4380 2621 2777 1372 1529 1046 1357 1638 4244 1436/ 2122ms
		//m 2605 1513 8190 2294 998 1324 3260 1467 1685 1747/  1987ms
	}