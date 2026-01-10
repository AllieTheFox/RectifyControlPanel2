#pragma once

#include "CElementWithSite.h"
#include "undoc.h"

class CControlPanelPage
    : public CElementWithSite
    , public IFrameNotificationClient
{
public:
    CControlPanelPage();

    //~ Begin IFrameNotificationClient Interface
    STDMETHODIMP LayoutInitialized() override;
    STDMETHODIMP Notify(const WCHAR* pszChangedProp) override;
    STDMETHODIMP OnNavigateAway() override;
    STDMETHODIMP OnInnerElementDestroyed() override;
    //~ End IFrameNotificationClient Interface

    virtual void LogCPLPerfTrackStopEvent();

protected:
    HRESULT RegisterWindowReadyEvent();
    HRESULT UnRegisterWindowReadyEvent();
    virtual HRESULT v_GetSearchTargetServices(REFIID riid, void** ppv);

private:
    DWORD _dwProffered;
    DWORD _dwEventToken;
    IDispatch* _pDispatchView;
    IDispatch* _pPerftrackInfo;
};

class CControlPanelPerfTrackInfo final : public IDispatch
{
public:
    static HRESULT Create(CControlPanelPage* pCControlPanelPage, IDispatch** ppDisp);

    //~ Begin IUnknown Interface
    STDMETHODIMP QueryInterface(REFIID, void**) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    //~ End IUnknown Interface

    //~ Begin IDispatch Interface
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;
    STDMETHODIMP GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo) override;
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid) override;
    STDMETHODIMP Invoke(
        DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdp, VARIANT* pvarResult, EXCEPINFO* pxi,
        UINT* puArgErr) override;
    //~ End IDispatch Interface

private:
    explicit CControlPanelPerfTrackInfo(CControlPanelPage* pCControlPanelPage);

    CControlPanelPage* _pPage;
    ULONG _cRef;
};