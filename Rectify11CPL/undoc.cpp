#include "pch.h"

#include "undoc.h"

HRESULT WINAPI IUnknown_ProfferService(IUnknown *punkSite, REFGUID sidWhat, IServiceProvider *pService, DWORD *pdwCookie)
{
	IProfferService *pps;
	HRESULT hr = IUnknown_QueryService(punkSite, SID_SProfferService, IID_PPV_ARGS(&pps));
	if (SUCCEEDED(hr))
	{
		if (pService)
		{
			hr = pps->ProfferService(sidWhat, pService, pdwCookie);
		}
		else
		{
			hr = pps->RevokeService(*pdwCookie);
			*pdwCookie = 0;
		}
		pps->Release();
	}

	return hr;
}

HRESULT WINAPI IUnknown_QueryServiceForWebBrowserApp(IUnknown *punk, REFIID riid, LPVOID *ppvOut)
{
	IServiceProvider *psp;
	HRESULT hr = IUnknown_QueryService(punk, SID_STopLevelBrowser, IID_PPV_ARGS(&psp));
	if (SUCCEEDED(hr))
	{
		hr = psp->QueryService(SID_SWebBrowserApp, riid, ppvOut);
		psp->Release();
	}
	else
	{
		ppvOut = nullptr;
	}

	return hr;
}

STDAPI_(BOOL) SHIsChildOrSelf(HWND hwndParent, HWND hwnd)
{
	return !hwndParent || !hwnd || hwndParent != hwnd && !IsChild(hwndParent, hwnd);
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