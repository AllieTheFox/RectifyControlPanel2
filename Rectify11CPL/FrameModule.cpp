#include "pch.h"

#include "FrameModule.h"


HRESULT CFrameModule::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(CFrameModule, IFrameModule),
        {},
    };
    HRESULT hr = QISearch(this, qit, riid, ppv);
    if (FAILED(hr))
    {
        hr = CElementWithSite::QueryInterface(riid, ppv);
    }
    return hr;
}

DirectUI::IClassInfo* CFrameModule::Class = nullptr;

DirectUI::IClassInfo* CFrameModule::GetClassInfoW()
{
    return Class;
}

HRESULT CFrameModule::Create(Element* pParent, DWORD* pdwDeferCookie, Element** ppElement)
{
    UNREFERENCED_PARAMETER(pParent);
    UNREFERENCED_PARAMETER(pdwDeferCookie);
    UNREFERENCED_PARAMETER(ppElement);

    DUI_ASSERT("Cannot instantiate a CFrameModule via parser.Must derive.");
    return E_NOTIMPL;
}

static DirectUI::PropertyInfoData dataimpModuleIDProp;

static DirectUI::Value* WINAPI svDefault()
{
    static DirectUI::_StaticValuePtr value
    {
        /*_dType*/ (int)DirectUI::ValueType::String,
        /*_fWeakRef*/ FALSE,
        /*_cRef*/ -1,
        /*_ptr*/ (void*)L""
    };
    return reinterpret_cast<DirectUI::Value*>(&value);
}

static const int vvimpModuleIDProp[] = { 5, -1 };

static DirectUI::PropertyInfo impModuleIDProp =
{
    /*pszName*/ L"ModuleID",
    /*fFlags*/ DirectUI::PF_Normal,
    /*fGroups*/ DirectUI::PG_None,
    /*pValidValues*/ vvimpModuleIDProp,
    /*pEnumMaps*/ nullptr,
    /*DefaultProc*/ svDefault,
    /*pData*/ &dataimpModuleIDProp
};

const DirectUI::PropertyInfo* CFrameModule::ModuleIDProp = &impModuleIDProp;

HRESULT CFrameModule::Register()
{
    HRESULT hr = CElementWithSite::Register();
    if (SUCCEEDED(hr))
    {
        static const DirectUI::PropertyInfo* const s_rgProps[] =
        {
            ModuleIDProp
        };
        hr = DirectUI::ClassInfo<CFrameModule, CElementWithSite>::RegisterGlobal(g_hinst, L"FrameModule", s_rgProps, ARRAYSIZE(s_rgProps));
    }
    return hr;
}

HRESULT CFrameModule::SetInnerObject(IUnknown *punkInner)
{
    RemoveAll();

    HRESULT hr = E_FAIL;
    if (punkInner)
    {
        IWrappedDUIElement* pwde;
        hr = punkInner->QueryInterface(IID_PPV_ARGS(&pwde));
        if (SUCCEEDED(hr))
        {
            Element* pe;
            hr = pwde->GetElement((void**)&pe);
            if (SUCCEEDED(hr))
            {
                Add(pe);
            }
            pwde->Release();
        }
    }

    return hr;
}

HRESULT CFrameModule::GetModuleID(WCHAR** ppszModuleID)
{
    DirectUI::Value* pv = GetValue(ModuleIDProp, 2, nullptr);
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
        {},
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
    ElementWrapper* pew = new(std::nothrow) ElementWrapper(pe);
    if (pew)
    {
        hr = pew->QueryInterface(riid, ppv);
        pew->Release();
    }
    return hr;
}