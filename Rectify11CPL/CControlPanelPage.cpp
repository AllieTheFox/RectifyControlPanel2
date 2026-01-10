#include "pch.h"

#include "CControlPanelPage.h"

#include <shdispid.h>

CControlPanelPage::CControlPanelPage()
    : CElementWithSite()
    , _dwProffered(0)
    , _dwEventToken(0)
    , _pDispatchView(nullptr)
    , _pPerftrackInfo(nullptr)
{
}

DEFINE_GUID(IID_IShellSearchTargetServices, 0xDDA3A58A, 0x43DA, 0x4A43, 0xA5, 0xF2, 0xF7, 0xAB, 0xF6, 0xE3, 0xC0, 0x26);

DEFINE_GUID(SID_SNavBar, 0x982A0000, 0x3F0E, 0x36B9, 0xDB, 0x43, 0x82, 0x1F, 0x13, 0x9D, 0xC5, 0xDE);
DEFINE_GUID(OID_OSearchControl, 0x03BFCEF0, 0xFCDD, 0x4CA9, 0xBE, 0xF4, 0x69, 0x10, 0xFA, 0x99, 0x0E, 0xA3);

HRESULT CControlPanelPage::LayoutInitialized()
{
    IServiceProvider* psp;
    if (!_dwProffered && SUCCEEDED(v_GetSearchTargetServices(IID_PPV_ARGS(&psp))))
    {
        IUnknown_ProfferService(_punkSite, IID_IShellSearchTargetServices, psp, &_dwProffered);

        IShellSearchTargetServices* psts;
        if (SUCCEEDED(psp->QueryInterface(IID_PPV_ARGS(&psts))))
        {
            WCHAR szPromptText[MAX_PATH];
            IObjectProvider* pop;
            if (SUCCEEDED(psts->GetPromptText(szPromptText, ARRAYSIZE(szPromptText)))
                && SUCCEEDED(IUnknown_QueryService(_punkSite, SID_SNavBar, IID_PPV_ARGS(&pop))))
            {
                IShellSearchControl* psc;
                if (SUCCEEDED(pop->QueryObject(OID_OSearchControl, IID_PPV_ARGS(&psc))))
                {
                    psc->SetCueAndTooltipText(szPromptText, nullptr);
                    psc->Release();
                }
                pop->Release();
            }
            psts->Release();
        }
        psp->Release();
    }
    return S_OK;
}

typedef BOOL (WINAPI *SHWindowsPolicy_t)(REFGUID rpolid);

BOOL SHWindowsPolicy(REFGUID rpolid)
{
    static SHWindowsPolicy_t fn = nullptr;
    if (!fn)
    {
        HMODULE h = LoadLibrary(L"shcore.dll");
        if (h)
            fn = (SHWindowsPolicy_t)GetProcAddress(h, MAKEINTRESOURCEA(190));
    }

    if (fn == nullptr)
        return E_FAIL;
    else
        return fn(rpolid);
}

typedef int (WINAPI *SHStringFromGUIDW_t)(REFGUID guid, WCHAR* lpszDest, int cchMax);
int SHStringFromGUIDW(REFGUID guid, WCHAR* lpszDest, int cchMax)
{
    static SHStringFromGUIDW_t fn = nullptr;
    if (!fn)
    {
        HMODULE h = GetModuleHandle(L"shlwapi.dll");
        if (h)
            fn = (SHStringFromGUIDW_t)GetProcAddress(h, MAKEINTRESOURCEA(24));
    }

    if (fn == nullptr)
        return E_FAIL;
    else
        return fn(guid, lpszDest, cchMax);
}

DEFINE_GUID(POLID_NoControlPanel, 0x8204C743, 0xCB99, 0x4578, 0xA7, 0xCC, 0x9D, 0xCC, 0xB9, 0xA3, 0x8C, 0x70);
DEFINE_GUID(CLSID_ControlPanelCategory, 0x26EE0668, 0xA00A, 0x44D7, 0x93, 0x71, 0xBE, 0xB0, 0x64, 0xC9, 0x86, 0x83);

HRESULT CControlPanelPage::Notify(const WCHAR* pszChangedProp)
{
    if (!_punkSite)
        return E_INVALIDARG;

    HRESULT hr = S_OK;

    if (!StrCmpC(pszChangedProp, L"SearchText") && !SHWindowsPolicy(POLID_NoControlPanel))
    {
        IPropertyBag* ppb;
        hr = IUnknown_QueryService(_punkSite, IID_IFrameManager, IID_PPV_ARGS(&ppb));
        if (SUCCEEDED(hr))
        {
            WCHAR szQuery[MAX_PATH];
            hr = PSPropertyBag_ReadStr(ppb, L"SearchText", szQuery, ARRAYSIZE(szQuery));
            if (SUCCEEDED(hr))
            {
                if (szQuery[0])
                {
                    WCHAR wszControlPanelGUID[GUIDSTRING_MAX];
                    hr = SHStringFromGUIDW(CLSID_ControlPanelCategory, wszControlPanelGUID, ARRAYSIZE(wszControlPanelGUID));
                    if (SUCCEEDED(hr))
                    {
                        WCHAR szControlPanelPath[42];
                        hr = StringCchPrintf(szControlPanelPath, (ARRAYSIZE(szControlPanelPath) - 1), L"::%s", wszControlPanelGUID);
                        if (SUCCEEDED(hr))
                        {
                            PIDLIST_ABSOLUTE pidlControlPanel;
                            hr = SHParseDisplayName(szControlPanelPath, 0, &pidlControlPanel, 0, 0);
                            if (SUCCEEDED(hr))
                            {
                                IShellBrowser* psb;
                                hr = IUnknown_QueryService(_punkSite, SID_STopLevelBrowser, IID_PPV_ARGS(&psb));
                                if (SUCCEEDED(hr))
                                {
                                    hr = psb->BrowseObject(pidlControlPanel, (SBSP_SAMEBROWSER | SBSP_ACTIVATE_NOFOCUS | SBSP_CREATENOHISTORY));
                                    psb->Release();
                                }
                                ILFree(pidlControlPanel);
                            }
                        }
                    }
                }
            }
            ppb->Release();
        }
    }

    return hr;
}

HRESULT CControlPanelPage::OnNavigateAway()
{
    if (_dwProffered)
    {
        IUnknown_ProfferService(_punkSite, IID_IShellSearchTargetServices, nullptr, &_dwProffered);
    }
    return S_OK;
}

HRESULT CControlPanelPage::OnInnerElementDestroyed()
{
    return S_OK;
}

void CControlPanelPage::LogCPLPerfTrackStopEvent()
{
}

HRESULT CControlPanelPage::RegisterWindowReadyEvent()
{
    HRESULT hr = CControlPanelPerfTrackInfo::Create(this, &_pPerftrackInfo);
    if (SUCCEEDED(hr))
    {
        IFrameManager* pfm;
        hr = IUnknown_QueryService(_punkSite, IID_IFrameManager, IID_PPV_ARGS(&pfm));
        if (SUCCEEDED(hr))
        {
            IShellView* psv;
            hr = pfm->GetShellView(&psv);
            if (SUCCEEDED(hr))
            {
                hr = psv->GetItemObject(0, IID_PPV_ARGS(&_pDispatchView));
                if (SUCCEEDED(hr))
                {
                    hr = ConnectToConnectionPoint(_pPerftrackInfo, DIID_DShellFolderViewEvents, TRUE, _pDispatchView, &_dwEventToken, 0);
                }
                psv->Release();
            }
            pfm->Release();
        }
    }
    return hr;
}

HRESULT CControlPanelPage::UnRegisterWindowReadyEvent()
{
    HRESULT hr = S_OK;
    if (_dwEventToken)
    {
        hr = ConnectToConnectionPoint(_pPerftrackInfo, DIID_DShellFolderViewEvents, FALSE, _pDispatchView, &_dwEventToken, nullptr);
        _dwEventToken = 0;
    }
    SafeRelease(&_pDispatchView);
    SafeRelease(&_pPerftrackInfo);
    return hr;
}

DEFINE_GUID(CLSID_CPLSearchTargetServices, 0x8E69E5B8, 0x893B, 0x4E67, 0xBF, 0xA2, 0x45, 0xC6, 0x7D, 0x60, 0xAC, 0x3A);

HRESULT CControlPanelPage::v_GetSearchTargetServices(REFIID riid, void** ppv)
{
    return CoCreateInstance(
        CLSID_CPLSearchTargetServices, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, riid, ppv);
}

HRESULT CControlPanelPerfTrackInfo::Create(CControlPanelPage* pCControlPanelPage, IDispatch** ppDisp)
{
    HRESULT hr = E_OUTOFMEMORY;
    CControlPanelPerfTrackInfo* pCControlPanelPerfTrackInfo = new(std::nothrow) CControlPanelPerfTrackInfo(pCControlPanelPage);
    if (pCControlPanelPerfTrackInfo)
    {
        hr = pCControlPanelPerfTrackInfo->QueryInterface(IID_PPV_ARGS(ppDisp));
        pCControlPanelPerfTrackInfo->Release();
    }
    return hr;
}

HRESULT CControlPanelPerfTrackInfo::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(CControlPanelPerfTrackInfo, IDispatch),
        QITABENT(CControlPanelPerfTrackInfo, IUnknown),
        {},
    };
    return QISearch(this, qit, riid, ppv);
}

ULONG CControlPanelPerfTrackInfo::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG CControlPanelPerfTrackInfo::Release()
{
    ULONG cRef = InterlockedDecrement(&_cRef);
    if (!cRef)
    {
        delete this;
    }
    return cRef;
}

HRESULT CControlPanelPerfTrackInfo::GetTypeInfoCount(UINT* pctinfo)
{
    return E_NOTIMPL;
}

HRESULT CControlPanelPerfTrackInfo::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
{
    return E_NOTIMPL;
}

HRESULT CControlPanelPerfTrackInfo::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid)
{
    return E_NOTIMPL;
}

HRESULT CControlPanelPerfTrackInfo::Invoke(
    DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdp, VARIANT* pvarResult, EXCEPINFO* pxi,
    UINT* puArgErr)
{
    HRESULT hr = S_OK;
    if (dispid == DISPID_EXPLORERWINDOWREADY)
    {
        _pPage->LogCPLPerfTrackStopEvent();
    }
    else
    {
        hr = DISP_E_MEMBERNOTFOUND;
    }

    if (pvarResult)
    {
        pvarResult->vt = VT_BOOL;
        pvarResult->boolVal = FAILED(hr) ? VARIANT_FALSE : VARIANT_TRUE;
    }

    return hr;
}

CControlPanelPerfTrackInfo::CControlPanelPerfTrackInfo(CControlPanelPage* pCControlPanelPage)
    : _pPage(pCControlPanelPage)
    , _cRef(1)
{
}