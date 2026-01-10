#pragma once

#include "CElementWithSite.h"

class CNavigateButton : public CElementWithSite
{
public:
	static HRESULT Create(DirectUI::Element *pParent, DWORD *pdwDeferCookie, DirectUI::Element **ppElement);

	void OnEvent(DirectUI::Event *pEvent) override;

	static DirectUI::IClassInfo *Class;
	DirectUI::IClassInfo *GetClassInfo() override { return Class; }
	static HRESULT Register();

	static const DirectUI::PropertyInfo *NavigationTargetRootProp;
	static const DirectUI::PropertyInfo *NavigationTargetRelativeProp;
	static const DirectUI::PropertyInfo *NoBackstackProp;
	static const DirectUI::PropertyInfo *ShellExecuteProp;
	static const DirectUI::PropertyInfo *ShellExecuteVerbProp;
	static const DirectUI::PropertyInfo *ShellExecuteParamsProp;

	// == Begin IUnknown Interface ==
	//IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
	//IFACEMETHODIMP_(ULONG) AddRef();
	//IFACEMETHODIMP_(ULONG) Release();
	// == End IUnknown Interface ==

	HRESULT SetNavigationTarget(LPCWSTR pszTargetRoot, LPCWSTR pszTargetRelative);

	static void Navigate(LPCWSTR pszTargetNameRoot, LPCWSTR pszTargetNameRelative, IUnknown *punkSite, bool bIsRedirect);
	static void NavigateBack(IUnknown *punkSite);

	HRESULT SetNavigationTargetRoot(LPCWSTR v);
	HRESULT SetNavigationTargetRelative(LPCWSTR v);

	LPCWSTR GetShellExecute(DirectUI::Value **ppv);
	HRESULT SetShellExecute(LPCWSTR v);

	LPCWSTR GetShellExecuteVerb(DirectUI::Value **ppv);
	HRESULT SetShellExecuteVerb(LPCWSTR v);

	LPCWSTR GetShellExecuteParams(DirectUI::Value **ppv);
	HRESULT SetShellExecuteParams(LPCWSTR v);

private:
	void _Navigate();
	void _Execute();

	static HRESULT GetCplRoot(IShellBrowser *psb, ITEMIDLIST_ABSOLUTE **ppidl);
};

static DirectUI::_StaticValuePtr *svShellExecuteVerb()
{
	static DirectUI::_StaticValuePtr value =
	{
		/*_dType*/ (int)DirectUI::ValueType::String,
		/*_fWeakRef*/ FALSE,
		/*_cRef*/ -1,
		/*_ptr*/ (void *)L""
	};

	return &value;
}