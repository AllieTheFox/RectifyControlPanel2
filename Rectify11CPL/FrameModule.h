#pragma once

#include "NavigateButton.h"
#include "Windows.h"

MIDL_INTERFACE("60CE96BB-B6F9-42DC-B84E-5E5D9C370A6F")
IFrameModule : IUnknown
{
    STDMETHOD(CreateInnerObject)(IUnknown **ppunkInner) PURE;
    STDMETHOD(SetInnerObject)(IUnknown *punkInner) PURE;
    STDMETHOD(GetModuleID)(LPWSTR *ppszModuleID) PURE;
};

class CFrameModule
    : public CElementWithSite
    , public IFrameModule
{
public:
	// == Begin IUnknown Interface ==
    IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	// == End IUnknown Interface ==

    static HRESULT Create(DirectUI::Element *pParent, DWORD *pdwDeferCookie, DirectUI::Element **ppElement);
    static DirectUI::IClassInfo *Class;
    DirectUI::IClassInfo *GetClassInfo() override { return Class; }

    static HRESULT Register();
    static const DirectUI::PropertyInfo *ModuleIDProp;

	// == Begin IFrameModule Interface ==
    IFACEMETHODIMP CreateInnerObject(IUnknown **ppunkInner) PURE;
    IFACEMETHODIMP SetInnerObject(IUnknown *punkInner);
    IFACEMETHODIMP GetModuleID(LPWSTR *ppszModuleID);
	// == End IFrameModule Interface ==
};

MIDL_INTERFACE("b8c7036c-8d15-456d-84f5-82fb5b1ae6a5")
IWrappedDUIElement : IUnknown
{
    STDMETHOD(GetElement)(void**) PURE;
};

class ElementWrapper final : public IWrappedDUIElement
{
public:
    ElementWrapper(DirectUI::Element* pe);
    ElementWrapper(const ElementWrapper& other) = delete;

    //~ Begin IUnknown Interface
    STDMETHODIMP QueryInterface(REFIID, void**);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
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