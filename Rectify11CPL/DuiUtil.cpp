#include "pch.h"

#include "DuiUtil.h"

HRESULT CALLBACK DUI_SetSiteOnUnknown(IUnknown *punk, IUnknown *punkSite)
{
    return IUnknown_SetSite(punk, punkSite);
}

void CALLBACK DUI_SendSelectionChangedToUnknown(IUnknown *punk, LPARAM lparam)
{
    IFrameShellViewClient *pfsvc;
    if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pfsvc))))
    {
        pfsvc->OnSelectionChanged();
        pfsvc->Release();
    }
}

void CALLBACK DUI_SendSelectedItemChangedToUnknown(IUnknown *punk, LPARAM lparam)
{
    IFrameShellViewClient *pfsvc;
    if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pfsvc))))
    {
        pfsvc->OnSelectedItemChanged();
        pfsvc->Release();
    }
}

void CALLBACK DUI_SendNotificationToUnknown(IUnknown *punk, LPARAM lparam)
{
    IFrameNotificationClient *pfnc;
    if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pfnc))))
    {
        pfnc->Notify((LPCWSTR)lparam);
        pfnc->Release();
    }
}

void CALLBACK DUI_SendNavigateAwayToUnknown(IUnknown *punk, LPARAM lparam)
{
    IFrameNotificationClient *pfnc;
    if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pfnc))))
    {
        pfnc->OnNavigateAway();
        pfnc->Release();
    }
}

void CALLBACK DUI_SendInitializationToUnknown(IUnknown *punk, LPARAM lparam)
{
    IFrameNotificationClient *pfnc;
    if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pfnc))))
    {
        pfnc->LayoutInitialized();
        pfnc->Release();
    }
}

void CALLBACK DUI_SendFolderChangedToUnknown(IUnknown *punk, LPARAM lparam)
{
    IFrameShellViewClient *pfsvc;
    if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pfsvc))))
    {
        pfsvc->OnFolderChanged();
        pfsvc->Release();
    }
}

void CALLBACK DUI_SendContentsChangedToUnknown(IUnknown *punk, LPARAM lparam)
{
    IFrameShellViewClient *pfsvc;
    if (SUCCEEDED(punk->QueryInterface(IID_PPV_ARGS(&pfsvc))))
    {
        pfsvc->OnContentsChanged();
        pfsvc->Release();
    }
}

void DUI_WalkIUnknownElements(DirectUI::Element *pe, PFNELEMENTCALLBACK pfn, LPARAM lparam)
{
    if (pe)
    {
        DirectUI::Value *pvChildren;
        DirectUI::DynamicArray<DirectUI::Element *> *pel = pe->GetChildren(&pvChildren);
        if (pel)
        {
            for (UINT i = 0; i < pel->GetSize(); ++i)
            {
                DirectUI::Element *peChild = pel->GetItem(i);
                IUnknown *punk = CElementWithIUnknown::GetUnknownFromElement(peChild);
                if (punk)
                {
                    pfn(punk, lparam);
                }

                DUI_WalkIUnknownElements(peChild, pfn, lparam);
            }

            pvChildren->Release();
        }
    }
}

HRESULT WINAPI DUIFramework_InitDUI()
{
    HRESULT hr = InitProcessPriv(DUI_VERSION, g_hinst, true, true, true);
    if (SUCCEEDED(hr))
    {
        hr = InitThread(DirectUI::TSM_IMMERSIVE);
        if (FAILED(hr))
        {
            UnInitProcessPriv(g_hinst);
        }
    }

    return hr;
}

HRESULT WINAPI DUIFramework_UninitDUI()
{
    DirectUI::UnInitThread();
    DirectUI::UnInitProcessPriv(g_hinst);
    return S_OK;
}

#include "FocusIndicator.h"

HRESULT DUIFramework_SetFocusByFocusIndicator(DirectUI::Element *peStart)
{
    HRESULT hr = E_FAIL;
    CFocusIndicator *pfi = (CFocusIndicator *)peStart->FindDescendent(StrToID(L"FocusIndicator"));
    if (pfi)
    {
        DirectUI::Element *peFocus = peStart->FindDescendent(pfi->GetTargetId());
        if (peFocus)
        {
            hr = S_OK;

            DirectUI::NavReference nr;
            nr.Init(peFocus, nullptr);

            DirectUI::Element *peElement = peFocus->GetAdjacent(nullptr, 3, &nr, 0x3);
            if (peElement)
                peFocus = peElement;
            peFocus->SetKeyFocus();
        }
    }

    return hr;
}
