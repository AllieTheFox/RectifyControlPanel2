#pragma once

#include "CControlPanelPage.h"

class RectifyThemeCfgPage : public CControlPanelPage
{
public:
	RectifyThemeCfgPage();
	virtual ~RectifyThemeCfgPage() override;

	static DirectUI::IClassInfo *Class;
	static HRESULT Create(Element *pParent, DWORD *pdwDeferCookie, Element **ppElement);

	//Element
	virtual DirectUI::IClassInfo *GetClassInfoW() override { return Class; }

	static HRESULT Register();

	IFACEMETHOD(QueryInterface)(REFIID riid, void **ppv) override;
	IFACEMETHOD_(ULONG, AddRef)() override { return CElementWithSite::AddRef(); }
	IFACEMETHOD_(ULONG, Release)() override { return CElementWithSite::Release(); }

	// Element overrides
	virtual void OnEvent(DirectUI::Event *iev) override;

	// Important methods
	virtual HRESULT LayoutInitialized() override;

	void GoBack();

private:
	bool initializing = true;
};