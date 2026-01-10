#include "pch.h"

#include "RectifyThemeCfgPage.h"

#include "CRectifyUtil.h"

DirectUI::IClassInfo* RectifyThemeCfgPage::Class = NULL;

RectifyThemeCfgPage::RectifyThemeCfgPage()
{
}

RectifyThemeCfgPage::~RectifyThemeCfgPage()
{
}

HRESULT RectifyThemeCfgPage::Create(Element* pParent, DWORD* pdwDeferCookie, Element** ppElement)
{
	return CreateAndInit<RectifyThemeCfgPage, int>(0, pParent, pdwDeferCookie, ppElement);
}

HRESULT RectifyThemeCfgPage::Register()
{
	HRESULT	hr = CElementWithSite::Register();
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::ClassInfo<RectifyThemeCfgPage, CElementWithSite>::RegisterGlobal(g_hinst, L"RectifyThemeCfgPage", nullptr, 0);
	}

	return hr;
}

IFACEMETHODIMP RectifyThemeCfgPage::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(RectifyThemeCfgPage, IFrameNotificationClient),
		{ 0 },
	};

	HRESULT hr = QISearch(this, qit, riid, ppv);
	if (FAILED(hr))
	{
		CElementWithSite::QueryInterface(riid, ppv);
	}

	return hr;
}

void RectifyThemeCfgPage::OnEvent(DirectUI::Event* iev)
{
	if (iev->nStage != DirectUI::GMF_ROUTED)
		return;
	if (!iev->fHandled)
		Element::OnEvent(iev);
	if (initializing) return;

	if (iev->uidType == DirectUI::TouchButton::Click)
	{
		if (iev->peTarget->GetID() == DirectUI::StrToID((LPCWSTR)L"SaveThemePreferences"))
		{
			DirectUI::TouchCheckBox* IgnoreBg = (DirectUI::TouchCheckBox*)FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreBg"));
			DirectUI::TouchCheckBox* IgnoreCursors = (DirectUI::TouchCheckBox*)FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreCursors"));
			DirectUI::TouchCheckBox* IgnoreIcons = (DirectUI::TouchCheckBox*)FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreIcons"));
			DirectUI::TouchCheckBox* IgnoreColors = (DirectUI::TouchCheckBox*)FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreColors"));
			DirectUI::TouchCheckBox* IgnoreSounds = (DirectUI::TouchCheckBox*)FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreSounds"));
			DirectUI::TouchCheckBox* IgnoreScreensavers = (DirectUI::TouchCheckBox*)FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreScreensavers"));

			DWORD IgnoreBgVal = IgnoreBg->GetCheckedState() != DirectUI::CSF_Unchecked;
			DWORD IgnoreCursorsVal = IgnoreCursors->GetCheckedState() != DirectUI::CSF_Unchecked;
			DWORD IgnoreIconsVal = IgnoreIcons->GetCheckedState() != DirectUI::CSF_Unchecked;
			DWORD IgnoreColorsVal = IgnoreColors->GetCheckedState() != DirectUI::CSF_Unchecked;
			DWORD IgnoreSoundsVal = IgnoreSounds->GetCheckedState() != DirectUI::CSF_Unchecked;
			DWORD IgnoreScreensaversVal = IgnoreScreensavers->GetCheckedState() != DirectUI::CSF_Unchecked;

			HKEY Rectify11;
			if (RegCreateKey(HKEY_CURRENT_USER, Rectify11PrefsKey, &Rectify11))
			{
				SHOW_ERROR("Failed to create rectify11 key");
				return;
			}

			DWORD size = 4;

			RegSetKeyValueW(Rectify11, NULL, L"IgnoreBg", REG_DWORD, (LPCVOID)&IgnoreBgVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreCursors", REG_DWORD, (LPCVOID)&IgnoreCursorsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreIcons", REG_DWORD, (LPCVOID)&IgnoreIconsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreColors", REG_DWORD, (LPCVOID)&IgnoreColorsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreSounds", REG_DWORD, (LPCVOID)&IgnoreSoundsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreScreensavers", REG_DWORD, (LPCVOID)&IgnoreScreensaversVal, sizeof(DWORD));
			RegCloseKey(Rectify11);
			GoBack();
		}
		else if (iev->peTarget->GetID() == DirectUI::StrToID((LPCWSTR)L"IgnoreThemePreferences"))
		{
			GoBack();
		}
	}
}

void RectifyThemeCfgPage::GoBack()
{
	GUID SID_STopLevelBrowser = {};
	HRESULT hr = CLSIDFromString(L"{4c96be40-915c-11cf-99d3-00aa004ae837}", (LPCLSID)&SID_STopLevelBrowser);
	if (SUCCEEDED(hr))
	{
		IShellBrowser* browser = NULL;
		hr = IUnknown_QueryService(_punkSite, SID_STopLevelBrowser, IID_IShellBrowser, (LPVOID*)&browser);
		if (SUCCEEDED(hr))
		{
			hr = browser->BrowseObject(NULL, SBSP_PARENT | SBSP_SAMEBROWSER);
			browser->Release();
		}
	}

	if (FAILED(hr))
	{
		std::string message = std::system_category().message(hr);
		CHAR buffer[1024];
		sprintf_s(buffer, "BrowseObject() failed with %d (%s)", hr, message.c_str());
		MessageBoxA(NULL, buffer, "Rectify Theme Configuration Page Element", MB_ICONERROR);
	}
}

HRESULT RectifyThemeCfgPage::LayoutInitialized()
{
	Element* root = GetRoot();
	DirectUI::TouchCheckBox* IgnoreBg = (DirectUI::TouchCheckBox*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreBg"));
	DirectUI::TouchCheckBox* IgnoreCursors = (DirectUI::TouchCheckBox*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreCursors"));
	DirectUI::TouchCheckBox* IgnoreIcons = (DirectUI::TouchCheckBox*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreIcons"));
	DirectUI::TouchCheckBox* IgnoreColors = (DirectUI::TouchCheckBox*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreColors"));
	DirectUI::TouchCheckBox* IgnoreSounds = (DirectUI::TouchCheckBox*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreSounds"));
	DirectUI::TouchCheckBox* IgnoreScreensavers = (DirectUI::TouchCheckBox*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreScreensavers"));

	DirectUI::TouchButton* SaveThemePreferences = (DirectUI::TouchButton*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"SaveThemePreferences"));
	DirectUI::TouchButton* IgnoreThemePreferences = (DirectUI::TouchButton*)root->FindDescendent(DirectUI::StrToID((LPCWSTR)L"IgnoreThemePreferences"));

	HKEY Rectify11;
	if (RegCreateKey(HKEY_CURRENT_USER, Rectify11PrefsKey, &Rectify11))
	{
		SHOW_ERROR("Failed to create rectify11 key");
		return E_FAIL;
	}

	DWORD size = 4;

	DWORD IgnoreBgVal = 0;
	DWORD IgnoreCursorsVal = 0;
	DWORD IgnoreIconsVal = 0;
	DWORD IgnoreColorsVal = 0;
	DWORD IgnoreSoundsVal = 0;
	DWORD IgnoreScreensaversVal = 0;

	RegQueryValueExW(Rectify11, L"IgnoreBg", 0, NULL, (LPBYTE)&IgnoreBgVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreCursors", 0, NULL, (LPBYTE)&IgnoreCursorsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreIcons", 0, NULL, (LPBYTE)&IgnoreIconsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreColors", 0, NULL, (LPBYTE)&IgnoreColorsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreSounds", 0, NULL, (LPBYTE)&IgnoreSoundsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreScreensavers", 0, NULL, (LPBYTE)&IgnoreScreensaversVal, &size);
	RegCloseKey(Rectify11);


	IgnoreBg->SetToggleOnClick(true);
	IgnoreCursors->SetToggleOnClick(true);
	IgnoreIcons->SetToggleOnClick(true);
	IgnoreColors->SetToggleOnClick(true);
	IgnoreSounds->SetToggleOnClick(true);
	IgnoreScreensavers->SetToggleOnClick(true);

	IgnoreBg->SetCheckedState(IgnoreBgVal ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	IgnoreCursors->SetCheckedState(IgnoreCursorsVal ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	IgnoreIcons->SetCheckedState(IgnoreIconsVal ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	IgnoreColors->SetCheckedState(IgnoreColorsVal ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	IgnoreSounds->SetCheckedState(IgnoreSoundsVal ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	IgnoreScreensavers->SetCheckedState(IgnoreScreensaversVal ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	initializing = false;

	return S_OK;
}