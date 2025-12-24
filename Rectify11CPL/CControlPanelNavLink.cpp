#include "pch.h"

#include "CControlPanelNavLink.h"

CControlPanelNavLink::CControlPanelNavLink(CPNAV_LIST list)
	: _list(list)
	, _pszName(nullptr)
	, _pszNameAcc(nullptr)
	, _hIcon(nullptr)
	, _fDisabled(false)
{
}

CControlPanelNavLink::~CControlPanelNavLink()
{
	CoTaskMemFree(_pszName);
	CoTaskMemFree(_pszNameAcc);
	DestroyIcon(_hIcon);
}

HRESULT CControlPanelNavLink::Create(CPNAV_LIST list, CControlPanelNavLink **ppLink)
{
	*ppLink = new (std::nothrow) CControlPanelNavLink(list);
	return *ppLink != nullptr ? S_OK : E_OUTOFMEMORY;
}

HRESULT CControlPanelNavLink::SetName(HINSTANCE hInstance, UINT nResId)
{
	WCHAR szName[MAX_PATH];
	HRESULT hr = LoadString(hInstance, nResId, szName, ARRAYSIZE(szName)) != 0 ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		hr = SetName(szName);
	}

	return hr;
}

HRESULT CControlPanelNavLink::SetName(LPCWSTR pszName)
{
	return SHStrDup(pszName, &_pszName);
}

HRESULT CControlPanelNavLink::SetNameAcc(HINSTANCE hInstance, UINT nResId)
{
	return E_NOTIMPL;
}

HRESULT CControlPanelNavLink::SetNameAcc(LPCWSTR pszNameAcc)
{
	return SHStrDup(pszNameAcc, &_pszNameAcc);
}

HRESULT CControlPanelNavLink::SetIcon(HICON hIcon)
{
	_hIcon = hIcon;
	return _hIcon ? S_OK : E_FAIL;
}

HRESULT CControlPanelNavLink::SetIcon(HINSTANCE hInstance, int nIconId)
{
	// If hInstance is non-null, try to load the icon from the specified module first.
	if (hInstance)
	{
		_hIcon = (HICON)LoadImage(
			hInstance, MAKEINTRESOURCE(nIconId), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		return _hIcon != nullptr ? S_OK : E_FAIL;
	}

	// Otherwise, load the icon from the stock icons.
	SHSTOCKICONINFO sii = {};
	sii.cbSize = sizeof(sii);
	HRESULT hr = SHGetStockIconInfo((SHSTOCKICONID)nIconId, SHGSI_SMALLICON | SHGSI_ICON, &sii);
	if (SUCCEEDED(hr))
	{
		_hIcon = sii.hIcon;
	}
	return hr;
}
