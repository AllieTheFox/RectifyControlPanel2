#pragma once

#include "CControlPanelNavLink.h"

class CControlPanelNavLinks : public IUnknown
{
public:
	CControlPanelNavLinks();

	HRESULT Add(CControlPanelNavLink *pLink);
	HRESULT AddLinkNotify(CPNAV_LIST list, HINSTANCE hInstance, UINT nNameResId, int nLinkId, CControlPanelNavLink **ppLink);
	HRESULT AddLinkShellEx(CPNAV_LIST list, HINSTANCE hInstance, UINT nNameResId, LPCWSTR pszCommand, LPCWSTR pszParams, CControlPanelNavLink **ppLink);
	HRESULT AddLinkControlPanel(CPNAV_LIST list, HINSTANCE hInstance, UINT nNameResId, LPCWSTR pszApplet, LPCWSTR pszAppletPage, CControlPanelNavLink **ppLink);

	int GetCount()
	{
		return _hdpaNavLinks ? DPA_GetPtrCount(_hdpaNavLinks) : 0;
	}

	HRESULT GetAt(int i, CControlPanelNavLink **ppLink)
	{
		*ppLink = (CControlPanelNavLink *)DPA_GetPtr(_hdpaNavLinks, i);
		return *ppLink != nullptr ? S_OK : E_FAIL;
	}

	IFACEMETHOD(QueryInterface)(REFIID riid, _Out_ void **ppv) override;
	IFACEMETHOD_(ULONG, AddRef)() override;
	IFACEMETHOD_(ULONG, Release)() override;

protected:
	virtual ~CControlPanelNavLinks();

private:
	HDPA _hdpaNavLinks;
	long _cRef;
};
