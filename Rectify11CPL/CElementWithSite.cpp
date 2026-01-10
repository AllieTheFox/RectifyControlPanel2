#include "pch.h"

#include "CElementWithSite.h"

HRESULT CElementWithSite::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CElementWithSite, IObjectWithSite),
		{},
	};

	return QISearch(this, qit, riid, ppv);
}

ULONG CElementWithSite::AddRef()
{
	return 1;
}

ULONG CElementWithSite::Release()
{
	return 1;
}

HRESULT CElementWithSite::SetSite(IUnknown* punkSite)
{
	IUnknown_Set(&_punkSite, punkSite);
	return S_OK;
}

HRESULT CElementWithSite::GetSite(REFIID riid, void** ppvSite)
{
	*ppvSite = nullptr;
	return _punkSite ? _punkSite->QueryInterface(riid, ppvSite) : E_FAIL;
}

DirectUI::IClassInfo* CElementWithSite::Class;

HRESULT CElementWithSite::Create(DirectUI::Element* pParent, DWORD* pdwDeferCookie, DirectUI::Element** ppElement)
{
	UNREFERENCED_PARAMETER(pParent);
	UNREFERENCED_PARAMETER(pdwDeferCookie);
	UNREFERENCED_PARAMETER(ppElement);

	DUI_ASSERT("Cannot instantiate a CElementWithSite via parser. Must derive.");
	return E_NOTIMPL;
}

HRESULT CElementWithSite::Register()
{
	HRESULT hr = CElementWithIUnknown::Register();
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::ClassInfo<CElementWithSite, CElementWithIUnknown>::RegisterGlobal(g_hinst, L"CElementWithSite", 0, NULL);
	}

	return hr;
}

DEFINE_GUID(IID_IFrameManager, 0x31E4FA78, 0x02B4, 0x419F, 0x94, 0x30, 0x7B, 0x75, 0x85, 0x23, 0x7C, 0x77);

HRESULT CElementWithSite::GetPersistStream(
	int a2,
	unsigned long a3,
	IStream **ppStream)
{
	*ppStream = nullptr;

	IFrameManager *pfm;
	HRESULT hr = IUnknown_QueryService(_punkSite, IID_IFrameManager, IID_PPV_ARGS(&pfm));
	if (SUCCEEDED(hr))
	{
		WCHAR szName[MAX_PATH];
		if (GetAtomName(DirectUI::Element::GetID(), szName, ARRAYSIZE(szName)))
		{
			hr = pfm->GetPersistStream(a2, GetClassInfo()->GetName(), szName, a3, ppStream);
		}
		else
		{
			hr = ResultFromLastError();
		}

		pfm->Release();
	}

	return hr;
}

CElementWithSite::~CElementWithSite()
{
}