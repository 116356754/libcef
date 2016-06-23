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
      
    #ifndef __CEFSimpleSample__ClientHandler__  
    #define __CEFSimpleSample__ClientHandler__  
      
    #include "include/cef_render_process_handler.h"  
    #include "include/cef_client.h"  
    #include "include/cef_v8.h"  
    #include "include/cef_browser.h"  
    
	typedef void(*pFun)(int); //定义函数指针类型  

    class ClientHandler : public CefClient, public CefDisplayHandler ,public CefLifeSpanHandler,public CefLoadHandler {  
    public:  
        ClientHandler(pFun pFn_);  
      
        CefRefPtr<CefBrowser> GetBrowser()  
        {  
            return m_Browser;  
        }  
      
        CefWindowHandle GetBrowserHwnd()  
        {  
            return m_BrowserHwnd;  
        }  
      
        // CefClient methods  
        virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE  
        {  
            return this;  
        }  
      
        // Virutal on CefLifeSpanHandler  
        virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;  
        virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;  
        virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;  
      
		virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
			return this;
		}

		virtual void OnTitleChange( CefRefPtr<CefBrowser> browser, const CefString& title ) OVERRIDE;


		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
			return this;
		}

		virtual void OnLoadStart( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame ) OVERRIDE;

		virtual void OnLoadEnd( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode ) OVERRIDE;


    protected:  
        // The child browser window  
        CefRefPtr<CefBrowser> m_Browser;  
      
        // The child browser window handle  
        CefWindowHandle m_BrowserHwnd;  
      
        // /  
        // Macro that provides a reference counting implementation for classes extending  
        // CefBase.  
        // /  
        IMPLEMENT_REFCOUNTING(ClientHandler);  

	private:
		double start;	
		pFun m_statFn;
    };  
      
    #endif /* defined(__CEFSimpleSample__ClientHandler__) */