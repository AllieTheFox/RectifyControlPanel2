#pragma once

class CElementWithIUnknown
    : public DirectUI::Element
    , public IUnknown
{
public:
    //~ Begin IUnknown Interface
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    //~ End IUnknown Interface

    static HRESULT Create(Element* pParent, DWORD* pdwDeferCookie, Element** ppElement);

    static DirectUI::IClassInfo* Class;
    DirectUI::IClassInfo* GetClassInfoW() override;
    static HRESULT Register();

    static IUnknown* GetUnknownFromElement(Element* pe);
};