#pragma once

#include "CControlPanelNavLink.h"

class CControlPanelNavLinks : public IUnknown
{
public:
    CControlPanelNavLinks();

    HRESULT Add(CControlPanelNavLink* pLink);
    HRESULT AddLinkNotify(
        CPNAV_LIST list, HINSTANCE hInstance, UINT nNameResId, int nLinkId, CControlPanelNavLink** ppLink);
    HRESULT AddLinkShellEx(
        CPNAV_LIST list, HINSTANCE hInstance, UINT nNameResId, const WCHAR* pszCommand, const WCHAR* pszParams,
        CControlPanelNavLink** ppLink);
    HRESULT AddLinkControlPanel(
        CPNAV_LIST list, HINSTANCE hInstance, UINT nNameResId, const WCHAR* pszApplet, const WCHAR* pszAppletPage,
        CControlPanelNavLink** ppLink);

    int GetCount();
    HRESULT GetAt(int i, CControlPanelNavLink** ppLink);

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

protected:
    virtual ~CControlPanelNavLinks();

private:
    HDPA _hdpaNavLinks;
    LONG _cRef;
};
