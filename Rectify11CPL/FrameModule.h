#pragma once

#include "CElementWithSite.h"

MIDL_INTERFACE("60ce96bb-b6f9-42dc-b84e-5e5d9c370a6f")
IFrameModule : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CreateInnerObject(IUnknown** ppunkInner) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetInnerObject(IUnknown* punkInner) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetModuleID(WCHAR** ppszModuleID) = 0;
};

class CFrameModule
    : public CElementWithSite
    , public IFrameModule
{
public:
    //~ Begin IUnknown Interface
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    //~ End IUnknown Interface

    static HRESULT Create(Element* pParent, DWORD* pdwDeferCookie, Element** ppElement);
    static DirectUI::IClassInfo* Class;
    DirectUI::IClassInfo* GetClassInfoW() override;

    static HRESULT Register();
    static const DirectUI::PropertyInfo* ModuleIDProp;

    //~ Begin IFrameModule Interface
    STDMETHODIMP CreateInnerObject(IUnknown** ppunkInner) override PURE;
    STDMETHODIMP SetInnerObject(IUnknown* punkInner) override;
    STDMETHODIMP GetModuleID(WCHAR** ppszModuleID) override;
    //~ End IFrameModule Interface
};

MIDL_INTERFACE("b8c7036c-8d15-456d-84f5-82fb5b1ae6a5")
IWrappedDUIElement : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetElement(void**) = 0;
};

class ElementWrapper final : public IWrappedDUIElement
{
public:
    ElementWrapper(DirectUI::Element* pe);
    ElementWrapper(const ElementWrapper& other) = delete;

    //~ Begin IUnknown Interface
    STDMETHODIMP QueryInterface(REFIID, void**) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    //~ End IUnknown Interface

    //~ Begin IWrappedDUIElement Interface
    STDMETHODIMP GetElement(void** ppElement) override;
    //~ End IWrappedDUIElement Interface

protected:
    ~ElementWrapper();

private:
    LONG _cRef;
    DirectUI::Element* _pe;
};

HRESULT CreateWrapperForElement(DirectUI::Element* pe, REFIID riid, void** ppv);

template <typename TInnerClass, const WCHAR* ModuleID>
HRESULT FrameModule_CreateInnerObjectImpl(IUnknown** ppunkInner)
{
    DWORD dwCookie;
    DirectUI::Element* pe;
    HRESULT hr = TInnerClass::Create(0, &dwCookie, &pe);
    if (SUCCEEDED(hr))
    {
        pe->SetID(ModuleID);
        pe->SetLayoutPos(4);
        pe->EndDefer(dwCookie);
        hr = CreateWrapperForElement(pe, IID_PPV_ARGS(ppunkInner));
        if (FAILED(hr))
        {
            pe->Destroy();
        }
    }
    return hr;
}


template <typename TElement, int nCreate, int nActive, bool bShellLayout>
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

    if (bShellLayout && SUCCEEDED(hr))
    {
        DirectUI::Layout* pLayout;
        hr = DirectUI::ShellBorderLayout::Create(&pLayout);
        if (SUCCEEDED(hr))
        {
            hr = pT->SetLayout(pLayout);
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