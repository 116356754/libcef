#pragma once

#include <Windows.h>

struct Interfacecef
{
protected:
	Interfacecef(void){};	
public:
	virtual ~Interfacecef(void){};
	virtual void CreateBrowser(HWND Parent_,const char* startUrl_=NULL)=NULL;
	virtual void LoadUrl(const char* url_=NULL)=NULL;
	virtual void OnSize(const RECT rc)=NULL;
};