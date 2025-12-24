#include "pch.h"

#include "NavigateButton.h"

DirectUI::IClassInfo *CNavigateButton::Class = nullptr;

HRESULT TranslateCplCanonicalName(LPCWSTR pszCanonicalName, LPWSTR pszNamespaceName, UINT cch)
{
	IOpenControlPanel *pocp;
	HRESULT hr = CoCreateInstance(CLSID_OpenControlPanel, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pocp));
	if (SUCCEEDED(hr))
	{
		hr = pocp->GetPath(pszCanonicalName, pszNamespaceName, cch);
		if (FAILED(hr))
		{
			hr = StringCchCopy(pszNamespaceName, cch, pszCanonicalName);
		}
		pocp->Release();
	}

	return hr;
}

HRESULT CNavigateButton::Create(DirectUI::Element *pParent, DWORD *pdwDeferCookie, DirectUI::Element **ppElement)
{
	HRESULT hr = CreateAndInit<CNavigateButton, UINT>(0, pParent, pdwDeferCookie, ppElement);
	if (SUCCEEDED(hr))
	{
		hr = (*ppElement)->SetActive(1);
	}

	return hr;
}

void CNavigateButton::OnEvent(DirectUI::Event *pEvent)
{
	if (pEvent->nStage == GMF_BUBBLED && pEvent->uidType == DirectUI::Button::Click)
	{
		DirectUI::Value *pvShellExecute;
		LPCWSTR pszShellExecute = GetShellExecute(&pvShellExecute);
		if (pszShellExecute && *pszShellExecute)
		{
			_Execute();
		}
		else
		{
			_Navigate();
		}
		pvShellExecute->Release();
	}

	DirectUI::Element::OnEvent(pEvent);
}

static const int vvimpNavigationTargetRootProp[] = { 5, -1 };
static DirectUI::PropertyInfoData dataimpNavigationTargetRootProp;
static const DirectUI::PropertyInfo impNavigationTargetRootProp =
{
  .pszName = L"navigationtargetroot",
  .fFlags = DirectUI::PF_Normal,
  .fGroups = DirectUI::PG_None,
  .pValidValues = vvimpNavigationTargetRootProp,
  .pEnumMaps = NULL,
  .DefaultProc = &DirectUI::Value::GetStringNull,
  .pData = &dataimpNavigationTargetRootProp
};

static const int vvimpNavigationTargetRelativeProp[] = { 5, -1 };
static DirectUI::PropertyInfoData dataimpNavigationTargetRelativeProp;
static const DirectUI::PropertyInfo impNavigationTargetRelativeProp =
{
  .pszName = L"navigationtargetrelative",
  .fFlags = DirectUI::PF_Normal,
  .fGroups = DirectUI::PG_None,
  .pValidValues = vvimpNavigationTargetRelativeProp,
  .pEnumMaps = NULL,
  .DefaultProc = &DirectUI::Value::GetStringNull,
  .pData = &dataimpNavigationTargetRelativeProp
};

static DirectUI::PropertyInfoData dataimpNoBackstackProp;
static const int vvimpNoBackstackProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impNoBackstackProp =
{
  .pszName = L"nobackstack",
  .fFlags = DirectUI::PF_Normal,
  .fGroups = DirectUI::PG_None,
  .pValidValues = vvimpNoBackstackProp,
  .pEnumMaps = NULL,
  .DefaultProc = &DirectUI::Value::GetStringNull,
  .pData = &dataimpNoBackstackProp
};

static DirectUI::PropertyInfoData dataimpShellExecuteProp;
static const int vvimpShellExecuteProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impShellExecuteProp =
{
  .pszName = L"ShellExecute",
  .fFlags = DirectUI::PF_Normal,
  .fGroups = DirectUI::PG_None,
  .pValidValues = vvimpShellExecuteProp,
  .pEnumMaps = NULL,
  .DefaultProc = (DefaultValueProcT)&svShellExecuteVerb,
  .pData = &dataimpShellExecuteProp
};

static DirectUI::PropertyInfoData dataimpShellExecuteVerbProp;
static const int vvimpShellExecuteVerbProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impShellExecuteVerbProp =
{
  .pszName = L"ShellExecuteVerb",
  .fFlags = DirectUI::PF_Normal,
  .fGroups = DirectUI::PG_None,
  .pValidValues = vvimpShellExecuteVerbProp,
  .pEnumMaps = NULL,
  .DefaultProc = (DefaultValueProcT)&svShellExecuteVerb,
  .pData = &dataimpShellExecuteVerbProp
};

static DirectUI::PropertyInfoData dataimpShellExecuteParamsProp;
static const int vvimpShellExecuteParamsProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impShellExecuteParamsProp =
{
  .pszName = L"ShellExecuteParams",
  .fFlags = DirectUI::PF_Normal,
  .fGroups = DirectUI::PG_None,
  .pValidValues = vvimpShellExecuteParamsProp,
  .pEnumMaps = NULL,
  .DefaultProc = (DefaultValueProcT)&svShellExecuteVerb,
  .pData = &dataimpShellExecuteParamsProp
};

HRESULT CNavigateButton::Register()
{
	HRESULT hr = CElementWithSite::Register();
	if (SUCCEEDED(hr))
	{
		static const DirectUI::PropertyInfo *const s_rgProperties[] =
		{
		  &impNavigationTargetRootProp,
		  &impNavigationTargetRelativeProp,
		  &impNoBackstackProp,
		  &impShellExecuteProp,
		  &impShellExecuteVerbProp,
		  &impShellExecuteParamsProp
		};

		hr = DirectUI::ClassInfo<CNavigateButton, CElementWithSite>::RegisterGlobal(g_hinst, L"NavigateButton", s_rgProperties, ARRAYSIZE(s_rgProperties));
	}

	return hr;
}

MIDL_INTERFACE("52B14A6A-58F1-45BD-B00A-DCE7403D951E")
IRootAndRelativeParsingFolder : IUnknown
{
	STDMETHOD(GetRoot)(PIDLIST_ABSOLUTE *) PURE;
};

HRESULT CNavigateButton::GetCplRoot(IShellBrowser *psb, ITEMIDLIST_ABSOLUTE **ppidl)
{
	IShellView *psv;
	HRESULT hr = psb->QueryActiveShellView(&psv);
	if (SUCCEEDED(hr))
	{
		IFolderView *pfv;
		hr = psv->QueryInterface(IID_PPV_ARGS(&pfv));
		if (SUCCEEDED(hr))
		{
			IRootAndRelativeParsingFolder *pCpl;
			hr = pfv->GetFolder(IID_PPV_ARGS(&pCpl));
			if (SUCCEEDED(hr))
			{
				hr = pCpl->GetRoot(ppidl);
				pCpl->Release();
			}
			pfv->Release();
		}
		psv->Release();
	}

	return hr;
}

__inline HRESULT SHILCombine(PCIDLIST_ABSOLUTE pidl1, PCUIDLIST_RELATIVE pidl2, PIDLIST_ABSOLUTE *ppidl)
{
	*ppidl = ILCombine(pidl1, pidl2);
	return *ppidl ? S_OK : E_OUTOFMEMORY;
}

HRESULT SHILClone(PCUIDLIST_RELATIVE pidl, PIDLIST_RELATIVE *ppidlOut)
{
	if (pidl)
	{
		*ppidlOut = ILClone(pidl);
		return *ppidlOut ? S_OK : E_OUTOFMEMORY;
	}
	else
	{
		*ppidlOut = nullptr;
		return E_INVALIDARG;
	}
}

void CNavigateButton::Navigate(LPCWSTR pszTargetNameRoot, LPCWSTR pszTargetNameRelative, IUnknown *punkSite, bool bIsRedirect)
{
	HRESULT hr;
	IShellBrowser *psb;
	if (SUCCEEDED(IUnknown_QueryService(punkSite, SID_STopLevelBrowser, IID_PPV_ARGS(&psb))))
	{
		PIDLIST_ABSOLUTE pidlFolder = nullptr;

		bool bHaveRoot = pszTargetNameRoot && *pszTargetNameRoot;
		if (bHaveRoot)
		{
			WCHAR wszNamespaceName[MAX_PATH];
			hr = TranslateCplCanonicalName(pszTargetNameRoot, wszNamespaceName, ARRAYSIZE(wszNamespaceName));
			if (hr >= 0)
			{
				hr = SHParseDisplayName(wszNamespaceName, nullptr, &pidlFolder, 0, nullptr);
			}
		}
		else
		{
			hr = GetCplRoot(psb, &pidlFolder);
		}

		PIDLIST_ABSOLUTE pidl = nullptr;
		if (hr >= 0)
		{
			bool bHaveRelative = pszTargetNameRelative && *pszTargetNameRelative;
			if (bHaveRelative)
			{
				IShellFolder *psfRoot = nullptr;
				hr = SHBindToObject(nullptr, pidlFolder, nullptr, IID_PPV_ARGS(&psfRoot));
				if (hr >= 0)
				{
					PIDLIST_RELATIVE pidlRel;
					hr = psfRoot->ParseDisplayName(nullptr, nullptr, (LPWSTR)pszTargetNameRelative, nullptr, &pidlRel, nullptr);
					if (hr >= 0)
					{
						hr = SHILCombine(pidlFolder, pidlRel, (ITEMIDLIST_ABSOLUTE **)&pidl);
						ILFree(pidlRel);
					}
					psfRoot->Release();
				}
			}
			else
			{
				ITEMIDLIST_RELATIVE *pidlTemp = nullptr;
				hr = SHILClone(pidlFolder, &pidlTemp);
				pidl = (ITEMIDLIST_ABSOLUTE *)pidlTemp;
			}
		}

		ILFree(pidlFolder);

		if (SUCCEEDED(hr))
		{
			DWORD dwFlags = SBSP_SAMEBROWSER;
			if (bIsRedirect)
				dwFlags |= SBSP_REDIRECT;
			psb->BrowseObject(pidl, dwFlags);
			ILFree(pidl);
		}

		psb->Release();
	}
}

void CNavigateButton::_Execute()
{
	DirectUI::Value *pvShellExecute;
	LPCWSTR pszShellExecute = GetShellExecute(&pvShellExecute);
	if (pszShellExecute && *pszShellExecute)
	{
		IShellBrowser *psb;
		HWND hwndOwner = nullptr;
		if (SUCCEEDED(IUnknown_QueryService(_punkSite, SID_STopLevelBrowser, IID_PPV_ARGS(&psb))))
		{
			psb->GetWindow(&hwndOwner);
			psb->Release();
		}

		DirectUI::Value *pvShellExecuteVerb;
		LPCWSTR pszShellExecuteVerb = GetShellExecuteVerb(&pvShellExecuteVerb);

		DirectUI::Value *pvShellExecuteParams;
		LPCWSTR pszShellExecuteParams = GetShellExecuteParams(&pvShellExecuteParams);

		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpFile = pszShellExecute;
		sei.hwnd = hwndOwner;
		sei.nShow = SW_SHOW;
		sei.lpVerb = pszShellExecuteVerb;
		sei.lpParameters = pszShellExecuteParams;
		sei.fMask = (SEE_MASK_IDLIST | 0x00000008 | SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_LOG_USAGE);
		ShellExecuteEx(&sei);

		pvShellExecuteVerb->Release();
		pvShellExecuteParams->Release();
	}

	pvShellExecute->Release();
}

// TODO: Find typed version of this function
HRESULT CNavigateButton::SetNavigationTarget(LPCWSTR pszTargetRoot, LPCWSTR pszTargetRelative)
{
	HRESULT hr = SetNavigationTargetRoot(pszTargetRoot);
	if (SUCCEEDED(hr))
	{
		hr = SetNavigationTargetRelative(pszTargetRelative);
	}

	return hr;
}

void CNavigateButton::NavigateBack(IUnknown *punkSite)
{
	IShellBrowser *psb;
	if (SUCCEEDED(IUnknown_QueryService(punkSite, SID_STopLevelBrowser, IID_PPV_ARGS(&psb))))
	{
		IWebBrowserApp *pWBA;
		if (FAILED(psb->BrowseObject(0, SBSP_NAVIGATEBACK))
			&& SUCCEEDED(IUnknown_QueryServiceForWebBrowserApp(punkSite, IID_PPV_ARGS(&pWBA))))
		{
			pWBA->Quit();
			pWBA->Release();
		}
		psb->Release();
	}
}

HRESULT CNavigateButton::SetNavigationTargetRoot(LPCWSTR v)
{
	DirectUI::Value *pv = DirectUI::Value::CreateString(v, 0);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::Element::SetValue(&impNavigationTargetRootProp, 1, pv);
		pv->Release();
	}

	return hr;
}

HRESULT CNavigateButton::SetNavigationTargetRelative(LPCWSTR v)
{
	DirectUI::Value *pv = DirectUI::Value::CreateString(v, 0);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::Element::SetValue(&impNavigationTargetRelativeProp, 1, pv);
		pv->Release();
	}

	return hr;
}

HRESULT CNavigateButton::SetShellExecute(LPCWSTR v)
{
	DirectUI::Value *pv = DirectUI::Value::CreateString(v, 0);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::Element::SetValue(&impShellExecuteProp, 1, pv);
		pv->Release();
	}

	return hr;
}

HRESULT CNavigateButton::SetShellExecuteVerb(LPCWSTR v)
{
	DirectUI::Value *pv = DirectUI::Value::CreateString(v, 0);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::Element::SetValue(&impShellExecuteVerbProp, 1, pv);
		pv->Release();
	}

	return hr;
}

HRESULT CNavigateButton::SetShellExecuteParams(LPCWSTR v)
{
	DirectUI::Value *pv = DirectUI::Value::CreateString(v, 0);
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::Element::SetValue(&impShellExecuteParamsProp, 1, pv);
		pv->Release();
	}

	return hr;
}

void CNavigateButton::_Navigate()
{
	DirectUI::Value *pvRoot = DirectUI::Element::GetValue(&impNavigationTargetRootProp, 1, nullptr);
	LPCWSTR pszTargetNameRoot = DirectUI::Value::GetUnset() ? nullptr : pvRoot->GetString();

	DirectUI::Value *pvRelative = DirectUI::Element::GetValue(&impNavigationTargetRelativeProp, 1, nullptr);
	LPCWSTR pszTargetNameRelative = pvRelative == DirectUI::Value::GetUnset() ? nullptr : pvRelative->GetString();

	DirectUI::Value *pvNoBackstack = DirectUI::Element::GetValue(&impNoBackstackProp, 1, nullptr);
	LPCWSTR pszNoBackstack = pvNoBackstack == DirectUI::Value::GetUnset() ? nullptr : pvNoBackstack->GetString();

	bool bNoBackstack = pszNoBackstack && !StrCmpICW(pszNoBackstack, L"true");
	Navigate(pszTargetNameRoot, pszTargetNameRelative, _punkSite, bNoBackstack);

	pvRoot->Release();
	pvRelative->Release();
	pvNoBackstack->Release();
}

LPCWSTR CNavigateButton::GetShellExecute(DirectUI::Value **ppv)
{
	*ppv = DirectUI::Element::GetValue(&impShellExecuteProp, 2, nullptr);
	return (*ppv)->GetString();
}

LPCWSTR CNavigateButton::GetShellExecuteVerb(DirectUI::Value **ppv)
{
	*ppv = DirectUI::Element::GetValue(&impShellExecuteVerbProp, 2, nullptr);
	return (*ppv)->GetString();
}

LPCWSTR CNavigateButton::GetShellExecuteParams(DirectUI::Value **ppv)
{
	*ppv = DirectUI::Element::GetValue(&impShellExecuteParamsProp, 2, nullptr);
	return (*ppv)->GetString();
}
