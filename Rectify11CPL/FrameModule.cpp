#include "pch.h"

#include "FrameModule.h"

DirectUI::IClassInfo *CFrameModule::Class = nullptr;

HRESULT CFrameModule::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(CFrameModule, IFrameModule),
        { 0 },
    };

    HRESULT hr = QISearch(this, qit, riid, ppv);
    if (FAILED(hr))
    {
        hr = CElementWithSite::QueryInterface(riid, ppv);
    }

    return hr;
}

HRESULT CFrameModule::Create(DirectUI::Element *pParent, DWORD *pdwDeferCookie, DirectUI::Element **ppElement)
{
	UNREFERENCED_PARAMETER(pParent);
	UNREFERENCED_PARAMETER(pdwDeferCookie);
	UNREFERENCED_PARAMETER(ppElement);

	DUI_ASSERT("Cannot instantiate a CFrameModule via parser.Must derive.");

	return E_NOTIMPL;
}

static DirectUI::PropertyInfoData dataimpModuleIDProp;
static const int vvimpModuleIDProp[] = { 5, -1 };
static const DirectUI::PropertyInfo impModuleIDProp =
{
  .pszName = L"ModuleID",
  .fFlags = DirectUI::PF_Normal,
  .fGroups = DirectUI::PG_None,
  .pValidValues = vvimpModuleIDProp,
  .pEnumMaps = nullptr,
  .DefaultProc = (DefaultValueProcT)&svShellExecuteVerb,
  .pData = &dataimpModuleIDProp
};

HRESULT CFrameModule::Register()
{
    HRESULT hr = CElementWithSite::Register();
    if (SUCCEEDED(hr))
    {
        static const DirectUI::PropertyInfo const *s_rgProps[] =
        {
            &impModuleIDProp
        };

        hr = ClassInfo<CFrameModule, CElementWithSite>::RegisterGlobal(g_hinst, L"FrameModule", s_rgProps, ARRAYSIZE(s_rgProps));
    }

    return hr;
}

HRESULT CFrameModule::SetInnerObject(IUnknown *punkInner)
{
    DirectUI::Element::RemoveAll();

    HRESULT hr = E_FAIL;
    if (punkInner)
    {
        IWrappedDUIElement *pwde;
        hr = punkInner->QueryInterface(IID_PPV_ARGS(&pwde));
        if (SUCCEEDED(hr))
        {
            DirectUI::Element *pe;
            hr = pwde->GetElement((void **)&pe);
            if (SUCCEEDED(hr))
            {
                DirectUI::Element::Add(pe);
            }
            pwde->Release();
        }
    }

    return hr;
}

HRESULT CFrameModule::GetModuleID(LPWSTR *ppszModuleID)
{
    DirectUI::Value *pv = DirectUI::Element::GetValue(&impModuleIDProp, 2, nullptr);
    HRESULT hr = SHStrDup(pv->GetString(), ppszModuleID);
    pv->Release();
    return hr;
}

ElementWrapper::ElementWrapper(DirectUI::Element* pe)
    : _cRef(1)
    , _pe(pe)
{
}

HRESULT ElementWrapper::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(ElementWrapper, IWrappedDUIElement),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

ULONG ElementWrapper::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG ElementWrapper::Release()
{
    ULONG cRef = InterlockedDecrement(&_cRef);
    if (!cRef)
    {
        delete this;
    }
    return cRef;
}

HRESULT ElementWrapper::GetElement(void** ppElement)
{
    *ppElement = _pe;
    return S_OK;
}

ElementWrapper::~ElementWrapper()
{
    _pe->DestroyAll(false);
    _pe->Destroy(false);
}

HRESULT CreateWrapperForElement(DirectUI::Element* pe, REFIID riid, void** ppv)
{
    *ppv = nullptr;

    HRESULT hr = E_OUTOFMEMORY;
    ElementWrapper* pew = new (std::nothrow) ElementWrapper(pe);
    if (pew)
    {
        hr = pew->QueryInterface(riid, ppv);
        pew->Release();
    }
    return hr;
}

template <
    typename TElement,
    int nCreate,
    int nActive,
    bool bUseShellBorderLayout
>
HRESULT FrameModule_CreateImpl(DirectUI::Element* pParent, DWORD* pdwDeferCookie, DirectUI::Element** ppElement)
{
    *ppElement = nullptr;

    TElement* pT = DirectUI::HNew<TElement>();
    HRESULT hr = pT ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pT->Initialize(nCreate, pParent, pdwDeferCookie);
    }

    if (SUCCEEDED(hr))
    {
        hr = pT->SetActive(nActive);
    }

    if constexpr (bUseShellBorderLayout)
    {
        if (SUCCEEDED(hr))
        {
            DirectUI::Layout* pLayout;
            hr = DirectUI::ShellBorderLayout::Create(&pLayout);
            if (SUCCEEDED(hr))
            {
                hr = pT->SetLayout(pLayout);
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppElement = pT;
        pT = nullptr;
    }

    if (pT)
    {
        pT->Destroy(false);
    }

    return hr;
}
