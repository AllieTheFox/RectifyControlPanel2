#include "pch.h"

#include "NavigateButton.h"

DirectUI::IClassInfo* CNavigateButton::Class = nullptr;

HRESULT TranslateCplCanonicalName(const WCHAR* pszCanonicalName, WCHAR* pszNamespaceName, UINT cch)
{
    IOpenControlPanel* pocp;
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

HRESULT CNavigateButton::Create(DirectUI::Element* pParent, DWORD* pdwDeferCookie, DirectUI::Element** ppElement)
{
    HRESULT hr = CreateAndInit<CNavigateButton, UINT>(0, pParent, pdwDeferCookie, ppElement);
    if (SUCCEEDED(hr))
    {
        hr = (*ppElement)->SetActive(1);
    }

    return hr;
}

void CNavigateButton::OnEvent(DirectUI::Event* pEvent)
{
    if (pEvent->nStage == DirectUI::GMF_BUBBLED && pEvent->uidType == DirectUI::Button::Click)
    {
        DirectUI::Value* pvShellExecute;
        const WCHAR* pszShellExecute = GetShellExecute(&pvShellExecute);
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

DirectUI::IClassInfo* CNavigateButton::GetClassInfoW()
{
    return CElementWithSite::GetClassInfoW();
}

static const int vvimpNavigationTargetRootProp[] = { 5, -1 };
static DirectUI::PropertyInfoData dataimpNavigationTargetRootProp;
static const DirectUI::PropertyInfo impNavigationTargetRootProp =
{
    .pszName = L"navigationtargetroot",
    .fFlags = DirectUI::PF_Normal,
    .fGroups = DirectUI::PG_None,
    .pValidValues = vvimpNavigationTargetRootProp,
    .pEnumMaps = nullptr,
    .DefaultProc = &DirectUI::Value::GetStringNull,
    .pData = &dataimpNavigationTargetRootProp
};

const DirectUI::PropertyInfo* CNavigateButton::NavigationTargetRootProp = &impNavigationTargetRootProp;

static const int vvimpNavigationTargetRelativeProp[] = { 5, -1 };
static DirectUI::PropertyInfoData dataimpNavigationTargetRelativeProp;
static const DirectUI::PropertyInfo impNavigationTargetRelativeProp =
{
    .pszName = L"navigationtargetrelative",
    .fFlags = DirectUI::PF_Normal,
    .fGroups = DirectUI::PG_None,
    .pValidValues = vvimpNavigationTargetRelativeProp,
    .pEnumMaps = nullptr,
    .DefaultProc = &DirectUI::Value::GetStringNull,
    .pData = &dataimpNavigationTargetRelativeProp
};

const DirectUI::PropertyInfo* CNavigateButton::NavigationTargetRelativeProp = &impNavigationTargetRelativeProp;

static DirectUI::PropertyInfoData dataimpNoBackstackProp;
static const int vvimpNoBackstackProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impNoBackstackProp =
{
    .pszName = L"nobackstack",
    .fFlags = DirectUI::PF_Normal,
    .fGroups = DirectUI::PG_None,
    .pValidValues = vvimpNoBackstackProp,
    .pEnumMaps = nullptr,
    .DefaultProc = &DirectUI::Value::GetStringNull,
    .pData = &dataimpNoBackstackProp
};

const DirectUI::PropertyInfo* CNavigateButton::NoBackstackProp = &impNoBackstackProp;

static DirectUI::_StaticValuePtr* svShellExecute()
{
    static DirectUI::_StaticValuePtr value =
    {
        /*_dType*/ (int)DirectUI::ValueType::String,
        /*_fWeakRef*/ FALSE,
        /*_cRef*/ -1,
        /*_ptr*/ (void*)L""
    };
    return &value;
}

static DirectUI::PropertyInfoData dataimpShellExecuteProp;
static const int vvimpShellExecuteProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impShellExecuteProp =
{
    .pszName = L"ShellExecute",
    .fFlags = DirectUI::PF_Normal,
    .fGroups = DirectUI::PG_None,
    .pValidValues = vvimpShellExecuteProp,
    .pEnumMaps = nullptr,
    .DefaultProc = (DefaultValueProcT)&svShellExecute,
    .pData = &dataimpShellExecuteProp
};

const DirectUI::PropertyInfo* CNavigateButton::ShellExecuteProp = &impShellExecuteProp;

static DirectUI::_StaticValuePtr* svShellExecuteVerb()
{
    static DirectUI::_StaticValuePtr value =
    {
        /*_dType*/ (int)DirectUI::ValueType::String,
        /*_fWeakRef*/ FALSE,
        /*_cRef*/ -1,
        /*_ptr*/ (void*)L""
    };
    return &value;
}

static DirectUI::PropertyInfoData dataimpShellExecuteVerbProp;
static const int vvimpShellExecuteVerbProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impShellExecuteVerbProp =
{
    .pszName = L"ShellExecuteVerb",
    .fFlags = DirectUI::PF_Normal,
    .fGroups = DirectUI::PG_None,
    .pValidValues = vvimpShellExecuteVerbProp,
    .pEnumMaps = nullptr,
    .DefaultProc = (DefaultValueProcT)&svShellExecuteVerb,
    .pData = &dataimpShellExecuteVerbProp
};

const DirectUI::PropertyInfo* CNavigateButton::ShellExecuteVerbProp = &impShellExecuteVerbProp;

static DirectUI::_StaticValuePtr* svShellExecuteParams()
{
    static DirectUI::_StaticValuePtr value =
    {
        /*_dType*/ (int)DirectUI::ValueType::String,
        /*_fWeakRef*/ FALSE,
        /*_cRef*/ -1,
        /*_ptr*/ (void*)L""
    };
    return &value;
}

static DirectUI::PropertyInfoData dataimpShellExecuteParamsProp;
static const int vvimpShellExecuteParamsProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impShellExecuteParamsProp =
{
    .pszName = L"ShellExecuteParams",
    .fFlags = DirectUI::PF_Normal,
    .fGroups = DirectUI::PG_None,
    .pValidValues = vvimpShellExecuteParamsProp,
    .pEnumMaps = nullptr,
    .DefaultProc = (DefaultValueProcT)&svShellExecuteParams,
    .pData = &dataimpShellExecuteParamsProp
};

const DirectUI::PropertyInfo* CNavigateButton::ShellExecuteParamsProp = &impShellExecuteParamsProp;

HRESULT CNavigateButton::Register()
{
    HRESULT hr = CElementWithSite::Register();
    if (SUCCEEDED(hr))
    {
        static const DirectUI::PropertyInfo* const s_rgProperties[] =
        {
            NavigationTargetRootProp,
            NavigationTargetRelativeProp,
            NoBackstackProp,
            ShellExecuteProp,
            ShellExecuteVerbProp,
            ShellExecuteParamsProp
        };
        hr = DirectUI::ClassInfo<CNavigateButton, CElementWithSite>::RegisterGlobal(g_hinst, L"NavigateButton", s_rgProperties, ARRAYSIZE(s_rgProperties));
    }
    return hr;
}

MIDL_INTERFACE("52b14a6a-58f1-45bd-b00a-dce7403d951e")
IRootAndRelativeParsingFolder : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetRoot(ITEMIDLIST_ABSOLUTE**) = 0;
};

HRESULT CNavigateButton::GetCplRoot(IShellBrowser* psb, ITEMIDLIST_ABSOLUTE** ppidl)
{
    IShellView* psv;
    HRESULT hr = psb->QueryActiveShellView(&psv);
    if (SUCCEEDED(hr))
    {
        IFolderView* pfv;
        hr = psv->QueryInterface(IID_PPV_ARGS(&pfv));
        if (SUCCEEDED(hr))
        {
            IRootAndRelativeParsingFolder* pCpl;
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

__inline HRESULT SHILCombine(PCIDLIST_ABSOLUTE pidl1, PCUIDLIST_RELATIVE pidl2, PIDLIST_ABSOLUTE* ppidl)
{
    *ppidl = ILCombine(pidl1, pidl2);
    return *ppidl ? S_OK : E_OUTOFMEMORY;
}

HRESULT SHILClone(PCUIDLIST_RELATIVE pidl, PIDLIST_RELATIVE* ppidlOut)
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

void CNavigateButton::Navigate(const WCHAR* pszTargetNameRoot, const WCHAR* pszTargetNameRelative, IUnknown* punkSite, bool bIsRedirect)
{
    HRESULT hr;
    IShellBrowser* psb;
    if (SUCCEEDED(IUnknown_QueryService(punkSite, SID_STopLevelBrowser, IID_PPV_ARGS(&psb))))
    {
        ITEMIDLIST_ABSOLUTE* pidlFolder = nullptr;

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

        ITEMIDLIST_ABSOLUTE* pidl = nullptr;
        if (hr >= 0)
        {
            bool bHaveRelative = pszTargetNameRelative && *pszTargetNameRelative;
            if (bHaveRelative)
            {
                IShellFolder* psfRoot = nullptr;
                hr = SHBindToObject(nullptr, pidlFolder, nullptr, IID_PPV_ARGS(&psfRoot));
                if (hr >= 0)
                {
                    PIDLIST_RELATIVE pidlRel;
                    hr = psfRoot->ParseDisplayName(nullptr, nullptr, (WCHAR*)pszTargetNameRelative, nullptr, &pidlRel, nullptr);
                    if (hr >= 0)
                    {
                        hr = SHILCombine(pidlFolder, pidlRel, &pidl);
                        ILFree(pidlRel);
                    }
                    psfRoot->Release();
                }
            }
            else
            {
                ITEMIDLIST_RELATIVE* pidlTemp = nullptr;
                hr = SHILClone(pidlFolder, &pidlTemp);
                pidl = (ITEMIDLIST_ABSOLUTE*)pidlTemp;
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
    DirectUI::Value* pvShellExecute;
    const WCHAR* pszShellExecute = GetShellExecute(&pvShellExecute);
    if (pszShellExecute && *pszShellExecute)
    {
        IShellBrowser* psb;
        HWND hwndOwner = nullptr;
        if (SUCCEEDED(IUnknown_QueryService(_punkSite, SID_STopLevelBrowser, IID_PPV_ARGS(&psb))))
        {
            psb->GetWindow(&hwndOwner);
            psb->Release();
        }

        DirectUI::Value* pvShellExecuteVerb;
        const WCHAR* pszShellExecuteVerb = GetShellExecuteVerb(&pvShellExecuteVerb);

        DirectUI::Value* pvShellExecuteParams;
        const WCHAR* pszShellExecuteParams = GetShellExecuteParams(&pvShellExecuteParams);

        SHELLEXECUTEINFOW sei = {};
        sei.lpFile = pszShellExecute;
        sei.cbSize = sizeof(sei);
        sei.hwnd = hwndOwner;
        sei.nShow = SW_SHOW;
        sei.lpVerb = pszShellExecuteVerb;
        sei.lpParameters = pszShellExecuteParams;
        sei.fMask = SEE_MASK_IDLIST | 0x00000008 | SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_LOG_USAGE;
        ShellExecuteExW(&sei);

        pvShellExecuteVerb->Release();
        pvShellExecuteParams->Release();
    }

    pvShellExecute->Release();
}

HRESULT CNavigateButton::QueryInterface(const IID& riid, void** ppv)
{
    return CElementWithSite::QueryInterface(riid, ppv);
}

ULONG CNavigateButton::AddRef()
{
    return CElementWithSite::AddRef();
}

ULONG CNavigateButton::Release()
{
    return CElementWithSite::Release();
}

// TODO: Find typed version of this function
HRESULT CNavigateButton::SetNavigationTarget(const WCHAR* pszTargetRoot, const WCHAR* pszTargetRelative)
{
    HRESULT hr = SetNavigationTargetRoot(pszTargetRoot);
    if (SUCCEEDED(hr))
    {
        hr = SetNavigationTargetRelative(pszTargetRelative);
    }

    return hr;
}

void CNavigateButton::NavigateBack(IUnknown* punkSite)
{
    IShellBrowser* psb;
    if (SUCCEEDED(IUnknown_QueryService(punkSite, SID_STopLevelBrowser, IID_PPV_ARGS(&psb))))
    {
        IWebBrowserApp* pWBA;
        if (FAILED(psb->BrowseObject(nullptr, SBSP_NAVIGATEBACK))
            && SUCCEEDED(IUnknown_QueryServiceForWebBrowserApp(punkSite, IID_PPV_ARGS(&pWBA))))
        {
            pWBA->Quit();
            pWBA->Release();
        }
        psb->Release();
    }
}

HRESULT CNavigateButton::SetNavigationTargetRoot(const WCHAR* v)
{
    DirectUI::Value* pv = DirectUI::Value::CreateString(v, nullptr);
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = DirectUI::Element::SetValue(NavigationTargetRootProp, 1, pv);
        pv->Release();
    }

    return hr;
}

HRESULT CNavigateButton::SetNavigationTargetRelative(const WCHAR* v)
{
    DirectUI::Value* pv = DirectUI::Value::CreateString(v, nullptr);
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = DirectUI::Element::SetValue(NavigationTargetRelativeProp, 1, pv);
        pv->Release();
    }

    return hr;
}

HRESULT CNavigateButton::SetShellExecute(const WCHAR* v)
{
    DirectUI::Value* pv = DirectUI::Value::CreateString(v, nullptr);
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = DirectUI::Element::SetValue(ShellExecuteProp, 1, pv);
        pv->Release();
    }

    return hr;
}

HRESULT CNavigateButton::SetShellExecuteVerb(const WCHAR* v)
{
    DirectUI::Value* pv = DirectUI::Value::CreateString(v, nullptr);
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = DirectUI::Element::SetValue(ShellExecuteVerbProp, 1, pv);
        pv->Release();
    }

    return hr;
}

HRESULT CNavigateButton::SetShellExecuteParams(const WCHAR* v)
{
    DirectUI::Value* pv = DirectUI::Value::CreateString(v, nullptr);
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = DirectUI::Element::SetValue(ShellExecuteParamsProp, 1, pv);
        pv->Release();
    }

    return hr;
}

void CNavigateButton::_Navigate()
{
    DirectUI::Value* pvRoot = DirectUI::Element::GetValue(NavigationTargetRootProp, 1, nullptr);
    const WCHAR* pszTargetNameRoot = DirectUI::Value::GetUnset() ? nullptr : pvRoot->GetString();

    DirectUI::Value* pvRelative = DirectUI::Element::GetValue(NavigationTargetRelativeProp, 1, nullptr);
    const WCHAR* pszTargetNameRelative = pvRelative == DirectUI::Value::GetUnset() ? nullptr : pvRelative->GetString();

    DirectUI::Value* pvNoBackstack = DirectUI::Element::GetValue(NoBackstackProp, 1, nullptr);
    const WCHAR* pszNoBackstack = pvNoBackstack == DirectUI::Value::GetUnset() ? nullptr : pvNoBackstack->GetString();

    bool bNoBackstack = pszNoBackstack && !StrCmpICW(pszNoBackstack, L"true");
    Navigate(pszTargetNameRoot, pszTargetNameRelative, _punkSite, bNoBackstack);

    pvRoot->Release();
    pvRelative->Release();
    pvNoBackstack->Release();
}

const WCHAR* CNavigateButton::GetShellExecute(DirectUI::Value** ppv)
{
    *ppv = DirectUI::Element::GetValue(ShellExecuteProp, 2, nullptr);
    return (*ppv)->GetString();
}

const WCHAR* CNavigateButton::GetShellExecuteVerb(DirectUI::Value** ppv)
{
    *ppv = DirectUI::Element::GetValue(ShellExecuteVerbProp, 2, nullptr);
    return (*ppv)->GetString();
}

const WCHAR* CNavigateButton::GetShellExecuteParams(DirectUI::Value** ppv)
{
    *ppv = DirectUI::Element::GetValue(ShellExecuteParamsProp, 2, nullptr);
    return (*ppv)->GetString();
}