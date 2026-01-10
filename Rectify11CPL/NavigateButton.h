#pragma once

#include "CElementWithSite.h"

class CNavigateButton : public CElementWithSite
{
public:
    static HRESULT Create(DirectUI::Element* pParent, DWORD* pdwDeferCookie, DirectUI::Element** ppElement);

    void OnEvent(DirectUI::Event* pEvent) override;

    static DirectUI::IClassInfo* Class;
    DirectUI::IClassInfo* GetClassInfoW() override;
    static HRESULT Register();

    static const DirectUI::PropertyInfo* NavigationTargetRootProp;
    static const DirectUI::PropertyInfo* NavigationTargetRelativeProp;
    static const DirectUI::PropertyInfo* NoBackstackProp;
    static const DirectUI::PropertyInfo* ShellExecuteProp;
    static const DirectUI::PropertyInfo* ShellExecuteVerbProp;
    static const DirectUI::PropertyInfo* ShellExecuteParamsProp;

    //~ Begin IUnknown Interface
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    //~ End IUnknown Interface

    HRESULT SetNavigationTarget(const WCHAR* pszTargetRoot, const WCHAR* pszTargetRelative);

    static void Navigate(const WCHAR* pszTargetNameRoot, const WCHAR* pszTargetNameRelative, IUnknown* punkSite, bool bIsRedirect);
    static void NavigateBack(IUnknown* punkSite);

    HRESULT SetNavigationTargetRoot(const WCHAR* v);
    HRESULT SetNavigationTargetRelative(const WCHAR* v);

    const WCHAR* GetShellExecute(DirectUI::Value** ppv);
    HRESULT SetShellExecute(const WCHAR* v);

    const WCHAR* GetShellExecuteVerb(DirectUI::Value** ppv);
    HRESULT SetShellExecuteVerb(const WCHAR* v);

    const WCHAR* GetShellExecuteParams(DirectUI::Value** ppv);
    HRESULT SetShellExecuteParams(const WCHAR* v);

private:
    void _Navigate();
    void _Execute();

    static HRESULT GetCplRoot(IShellBrowser* psb, ITEMIDLIST_ABSOLUTE** ppidl);
};