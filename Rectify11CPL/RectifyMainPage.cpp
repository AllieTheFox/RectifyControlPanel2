#include "pch.h"

#include "RectifyMainPage.h"

#include "CRectifyUtil.h"
#include "ElevationManager.h"
#include "CControlPanelNavLinks.h"
#include "DuiUtil.h"

RectifyMainPage::RectifyMainPage()
	: _punkSite(nullptr)
	, _pcmbThemes(nullptr)
	, _pbtnHelp(nullptr)
	, _pchkMicaForEveryone(nullptr)
	, _pchkTabbed(nullptr)
	, _pbtnEnableAdmin(nullptr)
	, _pbtnRestartExplorer(nullptr)
	, _pbtnThemetoolInstall(nullptr)
	, _pchkWin11DefaultMenus(nullptr)
	, _pchkNilesoftSmall(nullptr)
	, _pchkNilesoftFull(nullptr)
	, _pchkClassic(nullptr)
	, _pchkClassicTransparent(nullptr)
	, _fAdmin(false)
	, _pRectifyUtil(nullptr)
	, _fInitializing(true)
{
}

RectifyMainPage::~RectifyMainPage()
{

}

HRESULT RectifyMainPage::Create(Element *pParent, DWORD *pdwDeferCookie, Element **ppElement)
{
	return CreateAndInit<RectifyMainPage, int>(0, pParent, pdwDeferCookie, ppElement);
}

void RectifyMainPage::OnEvent(DirectUI::Event* iev)
{
	if (iev->nStage != DirectUI::GMF_BUBBLED)
		return;

	if (!iev->fHandled)
		Element::OnEvent(iev);

	if (_fInitializing)
		return;

	if (iev->peTarget == _pbtnEnableAdmin)
	{
		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			IRectifyUtil* utility = ElevationManager::Initialize(_GetMainHwnd());
			if (utility != NULL)
			{
				// Destroy old class
				if (_pRectifyUtil != NULL)
				{
					_pRectifyUtil->Release();
				}

				_pRectifyUtil = utility;
				_fAdmin = TRUE;

				DWORD dwDefer = 0;
				StartDefer(&dwDefer);

				if (_pbtnEnableAdmin != nullptr)
				{
					_pbtnEnableAdmin->SetLayoutPos(-3);
					_pbtnEnableAdmin->SetVisible(FALSE);
				}
				if (_pbtnThemetoolInstall != nullptr)
				{
					_pbtnThemetoolInstall->SetEnabled(TRUE);
				}
				if (_pchkMicaForEveryone != nullptr)
				{
					_pchkMicaForEveryone->SetEnabled(TRUE);
				}
				if (_pchkMicaForEveryone != nullptr
					&& _pchkMicaForEveryone->GetCheckedState() != DirectUI::CSF_Unchecked
					&& _pchkTabbed != nullptr)
				{
					_pchkTabbed->SetEnabled(TRUE);
				}

				DirectUI::CCRadioButton* Options[] =
				{
					_pchkWin11DefaultMenus,
					_pchkNilesoftSmall,
					_pchkNilesoftFull,
					_pchkClassic,
					_pchkClassicTransparent
				};
				for (size_t i = 0; i < ARRAYSIZE(Options); i++)
				{
					if (Options[i] != nullptr)
					{
						Options[i]->SetEnabled(TRUE);
					}
				}
				EndDefer(dwDefer);
			}
		}
	}
	else if (iev->peTarget == _pbtnRestartExplorer)
	{
		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			CRectifyUtil::RestartExplorer();
			if (_pbtnRestartExplorer != nullptr)
			{
				// hide restart explorer button
				_pbtnRestartExplorer->SetLayoutPos(-3);
				_pbtnRestartExplorer->SetVisible(FALSE);
			}
		}
	}
	else if (iev->peTarget == _pcmbThemes)
	{
		if (iev->uidType == DirectUI::Combobox::SelectionChange)
		{
			ULONG apply_flags = 0;

			// load appy flags
			HKEY hkeyRectify11;
			if (RegCreateKey(HKEY_CURRENT_USER, Rectify11PrefsKey, &hkeyRectify11))
			{
				SHOW_ERROR("Failed to create Rectify11Prefs key");
				return;
			}

			DWORD size = 4;

			DWORD IgnoreBgVal = 0;
			DWORD IgnoreCursorsVal = 0;
			DWORD IgnoreIconsVal = 0;
			DWORD IgnoreColorsVal = 0;
			DWORD IgnoreSoundsVal = 0;
			DWORD IgnoreScreensaversVal = 0;

			RegQueryValueExW(hkeyRectify11, L"IgnoreBg", 0, NULL, (LPBYTE)&IgnoreBgVal, &size);
			RegQueryValueExW(hkeyRectify11, L"IgnoreCursors", 0, NULL, (LPBYTE)&IgnoreCursorsVal, &size);
			RegQueryValueExW(hkeyRectify11, L"IgnoreIcons", 0, NULL, (LPBYTE)&IgnoreIconsVal, &size);
			RegQueryValueExW(hkeyRectify11, L"IgnoreColors", 0, NULL, (LPBYTE)&IgnoreColorsVal, &size);
			RegQueryValueExW(hkeyRectify11, L"IgnoreSounds", 0, NULL, (LPBYTE)&IgnoreSoundsVal, &size);
			RegQueryValueExW(hkeyRectify11, L"IgnoreScreensavers", 0, NULL, (LPBYTE)&IgnoreScreensaversVal, &size);
			RegCloseKey(hkeyRectify11);

			if (IgnoreBgVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_BACKGROUND;
			}
			if (IgnoreCursorsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_CURSOR;
			}
			if (IgnoreIconsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_DESKTOP_ICONS;
			}
			if (IgnoreColorsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_COLOR;
			}
			if (IgnoreSoundsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_SOUND;
			}
			if (IgnoreSoundsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_SCREENSAVER;
			}

			// apply the theme
			int selection = _pcmbThemes->GetSelection();
			themetool_set_active(NULL, themes[selection], TRUE, apply_flags, 0);
			_UpdateThemeGraphic();

			// update mica
			if (_fAdmin)
			{
				BOOL hasMica = FALSE;
				BOOL hasTabbed = FALSE;
				_pRectifyUtil->GetMicaSettings(&hasMica, &hasTabbed);
				_pRectifyUtil->SetMicaForEveryoneEnabled(hasMica, hasTabbed);

				// update checkboxes in case we aren't using a mica theme anymore
				_pchkMicaForEveryone->SetCheckedState(hasMica ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
				_pchkTabbed->SetCheckedState(hasTabbed ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
			}
		}
	}
	else if (iev->peTarget == _pbtnHelp)
	{
		if (iev->uidType == DirectUI::Button::Click)
		{
			ShellExecute(nullptr, nullptr, TEXT("http://rectify11.net"), nullptr, nullptr, SW_SHOW);
		}
	}
	else if (iev->peTarget == _pchkMicaForEveryone)
	{
		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			DirectUI::CheckedStateFlags MicaEnabled2 = _pchkMicaForEveryone->GetCheckedState();
			DirectUI::CheckedStateFlags TabbedEnabled = _pchkTabbed->GetCheckedState();

			_pRectifyUtil->SetMicaForEveryoneEnabled(MicaEnabled2 == DirectUI::CSF_Checked ? TRUE : FALSE, TabbedEnabled ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);

			// Enable/disable the tabbed checkbox
			if (_pchkTabbed != nullptr)
			{
				_pchkTabbed->SetEnabled(MicaEnabled2 == DirectUI::CSF_Checked ? TRUE : FALSE);
			}
		}
	}
	else if (iev->peTarget == _pchkTabbed)
	{
		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			_pRectifyUtil->SetMicaForEveryoneEnabled(TRUE, _pchkTabbed->GetCheckedState() == DirectUI::CSF_Checked ? TRUE : FALSE);
		}
	}
	else if (iev->peTarget == _pbtnThemetoolInstall)
	{
		_pbtnThemetoolInstall->SetEnabled(FALSE);
		HRESULT hr = _pRectifyUtil->InstallThemeTool();
		if (FAILED(hr))
		{
			CHAR buffer[1024];
			std::string message = std::system_category().message(hr);

			snprintf(buffer, sizeof(buffer), "Failed to install SecureUxTheme. Error code is %x, which translates to %s.", hr, message.c_str());
			MessageBoxA(_GetMainHwnd(), buffer, "Error during SecureUxTheme install", MB_ICONERROR);
		}

		_UpdateThemetoolStatus();
		_pbtnThemetoolInstall->SetEnabled(TRUE);
	}
	// handle menu section
	if (iev->uidType == DirectUI::Button::Click && !wcscmp(iev->peTarget->GetClassInfoW()->GetName(), DirectUI::CCRadioButton::GetClassInfoPtr()->GetName()))
	{
		DirectUI::CCRadioButton* chkbox = (DirectUI::CCRadioButton*)iev->peTarget;
		if (chkbox->GetSelected())
		{
			HRESULT hr = E_ACTIVATIONDENIED_SHELLERROR;
			if (chkbox->GetID() == DirectUI::StrToID(L"Win11DefaultMenus"))
			{
				hr = _pRectifyUtil->SetCurrentMenuByIndex(Normal);
			}
			else if (chkbox->GetID() == DirectUI::StrToID(L"NilesoftSmall"))
			{
				hr = _pRectifyUtil->SetCurrentMenuByIndex(NilesoftSmall);
			}
			else if (chkbox->GetID() == DirectUI::StrToID(L"NilesoftFull"))
			{
				hr = _pRectifyUtil->SetCurrentMenuByIndex(NilesoftFull);
			}
			else if (chkbox->GetID() == DirectUI::StrToID(L"Classic"))
			{
				hr = _pRectifyUtil->SetCurrentMenuByIndex(Classic);
			}
			else if (chkbox->GetID() == DirectUI::StrToID(L"ClassicTransparent"))
			{
				hr = _pRectifyUtil->SetCurrentMenuByIndex(ClassicTransparent);
			}

			if (FAILED(hr))
			{
				WCHAR buffer[200];
				swprintf(buffer, 199, L"Failed to update menu settings. HRESULT is %x", hr);
				MessageBox(NULL, buffer, TEXT("ClassicTransparent_OnEvent"), MB_ICONERROR);
			}
			else
			{
				_ShowRestartExplorer();
			}
		}
	}
}

DirectUI::IClassInfo* RectifyMainPage::Class = nullptr;

DirectUI::IClassInfo* RectifyMainPage::GetClassInfo()
{
	return Class;
}

HRESULT RectifyMainPage::Register()
{
	return DirectUI::ClassInfo<RectifyMainPage, CElementWithSite>::RegisterGlobal(g_hinst, L"RectifyMainPage", nullptr, 0);
}

void RectifyMainPage::_UpdateThemeGraphic()
{
	const WCHAR* id = IsDarkTheme() ? MAKEINTRESOURCE(IDB_DARKPREVIEW) : MAKEINTRESOURCE(IDB_LIGHTPREVIEW);
	HBITMAP bmp = (HBITMAP)LoadImage(g_hinst, id, IMAGE_BITMAP, 256, 256, 0);
	if (bmp == NULL)
	{
		return;
	}
	DirectUI::Value* bitmap = DirectUI::Value::CreateGraphic(bmp, 3, 0xffffffff, false, false, false);
	Element* PreviewElement = FindDescendent(DirectUI::StrToID(L"ThemePreview"));
	if (PreviewElement != nullptr)
	{
		PreviewElement->SetValue(Element::ContentProp, 1, bitmap);
	}
	bitmap->Release();
}

void RectifyMainPage::_ShowRestartExplorer()
{
	_pbtnRestartExplorer->SetLayoutPos(0);
	_pbtnRestartExplorer->SetVisible(TRUE);
}

DEFINE_GUID(SID_PerLayoutPropertyBag, 0xA46E5C25, 0xC09C, 0x4CA8, 0x9A, 0x53, 0x49, 0xCF, 0x7F, 0x86, 0x55, 0x25);

void RectifyMainPage::OnDestroy()
{
	if (_pRectifyUtil != NULL)
	{
		_pRectifyUtil->Release();
		_pRectifyUtil = NULL;
	}

	Element::OnDestroy();
}

IFACEMETHODIMP RectifyMainPage::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(RectifyMainPage, IFrameNotificationClient),
		{},
	};
	HRESULT hr = QISearch(this, qit, riid, ppv);
	if (FAILED(hr))
	{
		hr = CElementWithSite::QueryInterface(riid, ppv);
	}
	return hr;
}

HRESULT RectifyMainPage::LayoutInitialized()
{
	CControlPanelPage::LayoutInitialized();

	HRESULT hr = IUnknown_GetSite(static_cast<IObjectWithSite*>(this), IID_PPV_ARGS(&_punkSite));
	if (SUCCEEDED(hr))
	{
		DUI_WalkIUnknownElements(this, DUI_SetSiteOnUnknown, reinterpret_cast<LPARAM>(_punkSite));
	}

	hr = themetool_init();
	if (hr != S_OK && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED))
	{
#ifdef ERROR_MESSAGES
		MessageBox(NULL, TEXT("Failed to initialize SecureUXTheme ThemeTool. Theme information will not be loaded. This may be due to the lack of the ThemeDll.dll in C:\\Windows\\Rectify11\\RectifyControlPanel"), TEXT("FrameProvider::LayoutInitialized"), MB_ICONERROR);
#endif
	}

	_pRectifyUtil = (IRectifyUtil*)new CRectifyUtil();
	_InitNavLinks();

	_pcmbThemes = static_cast<DirectUI::Combobox*>(FindDescendent(DirectUI::StrToID(L"ThemeCmb")));
	_pbtnHelp = static_cast<DirectUI::Button*>(FindDescendent(DirectUI::StrToID(L"buttonHelp")));
	_pchkMicaForEveryone = static_cast<DirectUI::TouchCheckBox*>(FindDescendent(DirectUI::StrToID(L"MicaChk")));
	_pchkTabbed = static_cast<DirectUI::TouchCheckBox*>(FindDescendent(DirectUI::StrToID(L"TabChk")));

	Element* version = FindDescendent(DirectUI::StrToID(L"RectifyVersion"));
	_pbtnEnableAdmin = static_cast<DirectUI::TouchButton*>(FindDescendent(DirectUI::StrToID(L"Link_EnableAdmin")));

	_pbtnRestartExplorer = static_cast<DirectUI::TouchButton*>(FindDescendent(DirectUI::StrToID(L"BtnRestartExplorer")));
	_pbtnThemetoolInstall = static_cast<DirectUI::TouchButton*>(FindDescendent(DirectUI::StrToID(L"ThemetoolInstall")));

	_pchkWin11DefaultMenus = static_cast<DirectUI::CCRadioButton*>(FindDescendent(DirectUI::StrToID(L"Win11DefaultMenus")));
	_pchkNilesoftSmall = static_cast<DirectUI::CCRadioButton*>(FindDescendent(DirectUI::StrToID(L"NilesoftSmall")));
	_pchkNilesoftFull = static_cast<DirectUI::CCRadioButton*>(FindDescendent(DirectUI::StrToID(L"NilesoftFull")));
	_pchkClassic = static_cast<DirectUI::CCRadioButton*>(FindDescendent(DirectUI::StrToID(L"Classic")));
	_pchkClassicTransparent = static_cast<DirectUI::CCRadioButton*>(FindDescendent(DirectUI::StrToID(L"ClassicTransparent")));

	DirectUI::CCRadioButton* Options[] =
	{
		_pchkWin11DefaultMenus,
		_pchkNilesoftSmall,
		_pchkNilesoftFull,
		_pchkClassic,
		_pchkClassicTransparent
	};

	if (_pcmbThemes != nullptr)
	{
		WCHAR value[255] = { 0 };
		PVOID pvData = value;
		DWORD size = sizeof(value);
		RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", L"DllName", RRF_RT_REG_SZ, 0, pvData, &size);
		std::wstring msstylePath = std::wstring((const WCHAR*)pvData);
		int k = 0;
		ULONG themeCount = 0;
		if (SUCCEEDED(themetool_get_theme_count(&themeCount)))
		{
			for (ULONG i = 0; i < themeCount; i++)
			{
				ITheme *theme = NULL;
				if (SUCCEEDED(themetool_get_theme(i, &theme)))
				{
					std::wstring nameBuffer = std::wstring(255, '\0');
					theme->GetDisplayName(nameBuffer);

					if (nameBuffer.starts_with(L"Rectify11"))
					{
						_pcmbThemes->AddString(nameBuffer.c_str());
						std::wstring pathBuff = std::wstring();
						theme->GetVisualStyle(pathBuff);
						std::wstring msstylePath = std::wstring((const WCHAR*)pvData);

						std::wstring msstylePathClean = msstylePath;

						const size_t last_slash_idx = msstylePathClean.find_last_of(L"\\/");
						if (std::string::npos != last_slash_idx)
						{
							msstylePathClean.erase(0, last_slash_idx + 1);
						}

						// Remove extension if present.
						const size_t period_idx = msstylePathClean.rfind('.');
						if (std::string::npos != period_idx)
						{
							msstylePathClean.erase(period_idx);
						}

						ThemesMap[k] = msstylePathClean;
						if (pathBuff == msstylePath)
						{
							_pcmbThemes->SetSelection(k);
						}
						themes.push_back(i);
						k++;
					}
				}
				themetool_theme_release(theme);
			}
		}
		else
		{
#ifdef ERROR_MESSAGES
			MessageBox(NULL, TEXT("Failed to count the amount of themes"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
#endif // 0
		}

		if (version != nullptr)
		{
			WCHAR value[255] = { 0 };
			PVOID pvData = value;
			DWORD size = sizeof(value);
			LONG result = RegGetValue(HKEY_LOCAL_MACHINE, L"Software\\Rectify11", L"Version", RRF_RT_REG_SZ, 0, pvData, &size);
			std::wstring vstr = std::wstring(L"");
			if (result == 0)
			{
				WCHAR versionstr[1024];
				if (FAILED(LoadStringW(g_hinst, IDS_VERSION, versionstr, 1023)))
				{
					wcscpy_s(versionstr, L"[VERSION STRING MISSING]: ");
				}
				vstr += versionstr;
				vstr += L" ";
				vstr += value;
			}
			else
			{
				WCHAR versionstr[1024];
				WCHAR notapplicable[1024];
				if (FAILED(LoadStringW(g_hinst, IDS_VERSION, versionstr, 1023)))
				{
					wcscpy_s(versionstr, L"[VERSION STRING MISSING]: ");
				}
				if (FAILED(LoadStringW(g_hinst, IDS_NA, notapplicable, 1023)))
				{
					wcscpy_s(notapplicable, L"[N/A STRING MISSING]: ");
				}

				vstr += versionstr;
				vstr += L" ";
				vstr += notapplicable;
			}
			version->SetContentString(vstr.c_str());
		}
	}

	if (_pchkMicaForEveryone != nullptr)
	{
		_pchkMicaForEveryone->SetToggleOnClick(true);
		BOOL MicaEnabled;
		BOOL TabbedEnabled;
		_pRectifyUtil->GetMicaSettings(&MicaEnabled, &TabbedEnabled);

		_pchkMicaForEveryone->SetCheckedState(MicaEnabled ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);

		if (!MicaEnabled && _pchkTabbed != nullptr)
		{
			_pchkTabbed->SetEnabled(FALSE);
		}
	}

	if (_pchkTabbed != nullptr)
	{
		BOOL MicaEnabled;
		BOOL TabbedEnabled;
		_pRectifyUtil->GetMicaSettings(&MicaEnabled, &TabbedEnabled);

		_pchkTabbed->SetToggleOnClick(true);
		_pchkTabbed->SetCheckedState(MicaEnabled ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	}

	if (_pbtnRestartExplorer != nullptr)
	{
		_pbtnRestartExplorer->SetLayoutPos(-3);
		_pbtnRestartExplorer->SetVisible(FALSE);
	}

	DWORD menuIndex;

	for (size_t i = 0; i < 5; i++)
	{
		if (Options[i] == nullptr)
			continue;
		if (!_fAdmin)
		{
			Options[i]->SetEnabled(FALSE);
		}
		else
		{
			Options[i]->SetEnabled(TRUE);
		}
	}
	if (SUCCEEDED(_pRectifyUtil->GetCurrentMenuIndex(&menuIndex)) && Options[menuIndex] != nullptr)
	{
		Options[menuIndex]->SetSelected(true);
	}

	if (_fAdmin)
	{
		if (_pbtnEnableAdmin != nullptr && _pbtnThemetoolInstall != nullptr)
		{
			_pbtnEnableAdmin->SetLayoutPos(-3);
			_pbtnEnableAdmin->SetVisible(FALSE);
			_pbtnThemetoolInstall->SetEnabled(TRUE);
		}
	}
	else if (_pchkMicaForEveryone && _pchkTabbed && _pbtnThemetoolInstall)
	{
		_pchkMicaForEveryone->SetEnabled(FALSE);
		_pchkTabbed->SetEnabled(FALSE);
		_pbtnThemetoolInstall->SetEnabled(FALSE);
	}

	_UpdateThemetoolStatus();

	_UpdateThemeGraphic();
	_fInitializing = false;

	return S_OK;
}

HRESULT RectifyMainPage::OnInnerElementDestroyed()
{
	return S_OK;
}

HRESULT RectifyMainPage::_InitNavLinks()
{
	CControlPanelNavLinks* pLinks = new(std::nothrow) CControlPanelNavLinks();
	if (!pLinks)
		return E_OUTOFMEMORY;

	CControlPanelNavLink* pLink;
	HRESULT hr = pLinks->AddLinkControlPanel(CPNAV_LIST_TASKS, g_hinst, IDS_UPDATE, L"Rectify11.SettingsCPL", L"pageThemePref", &pLink);
	if (SUCCEEDED(hr))
	{
		hr = pLinks->AddLinkShellEx(CPNAV_LIST_TASKS, g_hinst, IDS_UNINSTALL, L"C:\\Windows\\Rectify11\\Uninstall.exe", L"", &pLink);
	}
	if (SUCCEEDED(hr))
	{
		hr = pLinks->AddLinkControlPanel(CPNAV_LIST_SEEALSO, g_hinst, IDS_SYSINFO, L"Microsoft.System", L"", &pLink);
	}
	if (SUCCEEDED(hr))
	{
		IPropertyBag* ppb;
		hr = IUnknown_QueryService(_punkSite, SID_PerLayoutPropertyBag, IID_PPV_ARGS(&ppb));
		if (SUCCEEDED(hr))
		{
			hr = PSPropertyBag_WriteUnknown(ppb, L"ControlPanelNavLinks", pLinks);
			ppb->Release();
		}
	}
	pLinks->Release();
	return hr;
}

void RectifyMainPage::_UpdateThemetoolStatus()
{
	Element* status = (DirectUI::TouchButton*)FindDescendent(DirectUI::StrToID(L"ThemetoolStatus"));
	if (status != nullptr)
	{
		ULONG flags = secureuxtheme_get_state_flags();
		wstring statusText;

		WCHAR buffer1[1024];
		if (FAILED(LoadStringW(g_hinst, IDS_THEMETOOLSTATUS, buffer1, 1023)))
		{
			wcscpy_s(buffer1, L"[SECURE UX STATUS STRING MISSING]: ");
		}
		statusText += buffer1;

		if (flags & SECUREUXTHEME_STATE_INSTALLED)
		{
			if (flags & SECUREUXTHEME_STATE_CURRENT)
			{
				if (FAILED(LoadStringW(g_hinst, IDS_OK, buffer1, 1023)))
				{
					wcscpy_s(buffer1, L"OK STRING MISSING");
				}
				statusText += buffer1;
				status->SetForegroundStdColor(44); // forest green
			}
			else
			{
				if (FAILED(LoadStringW(g_hinst, IDS_OUTDATED, buffer1, 1023)))
				{
					wcscpy_s(buffer1, L"OUTDATED STRING MISSING");
				}
				statusText += buffer1;
				status->SetForegroundStdColor(138); // yellow
			}
		}
		else
		{
			if (FAILED(LoadStringW(g_hinst, IDS_NOTINSTALLED, buffer1, 1023)))
			{
				wcscpy_s(buffer1, L"NOT INSTALLED STRING MISSING");
			}
			statusText += buffer1;
			status->SetForegroundStdColor(113); // red
		}
		status->SetContentString(statusText.c_str());
	}
}

HWND RectifyMainPage::_GetMainHwnd()
{
	GUID SID_STopLevelBrowser = {};
	IID IID_IFrameManager = {};
	HRESULT hr = CLSIDFromString(L"{4c96be40-915c-11cf-99d3-00aa004ae837}", &SID_STopLevelBrowser);
	HWND result = NULL;
	if (SUCCEEDED(hr))
	{
		hr = CLSIDFromString(L"{31e4fa78-02b4-419f-9430-7b7585237c77}", &IID_IFrameManager);
		if (SUCCEEDED(hr))
		{
			IShellBrowser *browser = NULL;
			if (SUCCEEDED(IUnknown_QueryService(_punkSite, SID_STopLevelBrowser, IID_IShellBrowser, (LPVOID *)&browser)))
			{
				browser->GetWindow(&result);
				browser->Release();
			}
		}
	}

	return result;
}
