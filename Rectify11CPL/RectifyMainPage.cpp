#include "pch.h"

#include "RectifyMainPage.h"

#include "CRectifyUtil.h"
#include "ElevationManager.h"
#include "CControlPanelNavLinks.h"
#include "DuiUtil.h"

DirectUI::IClassInfo *RectifyMainPage::Class = nullptr;

RectifyMainPage::RectifyMainPage()
	: _punkSite(nullptr)
	, _fAdmin(false)
	, _pRectifyUtil(nullptr)
	, _fInitializing(true)
{
}

RectifyMainPage::~RectifyMainPage()
{

}

HRESULT RectifyMainPage::Register()
{
	return DirectUI::ClassInfo<RectifyMainPage, CElementWithSite>::RegisterGlobal(g_hinst, L"RectifyMainPage", nullptr, 0);
}

HRESULT RectifyMainPage::Create(Element *pParent, DWORD *pdwDeferCookie, Element **ppElement)
{
	return CreateAndInit<RectifyMainPage, int>(0, pParent, pdwDeferCookie, ppElement);
}

DirectUI::IClassInfo *RectifyMainPage::GetClassInfo()
{
	return Class;
}

void RectifyMainPage::OnEvent(DirectUI::Event *iev)
{
	if (iev->nStage != DirectUI::GMF_BUBBLED)
		return;

	if (!iev->fHandled)
		Element::OnEvent(iev);

	if (_fInitializing)
		return;

	if (iev->peTarget->GetID() == DirectUI::StrToID(L"Link_EnableAdmin"))
	{
		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			IRectifyUtil *utility = ElevationManager::Initialize(_GetMainHwnd());
			DirectUI::TouchCheckBox *MicaForEveryoneCheckbox = (DirectUI::TouchCheckBox *)FindDescendent(DirectUI::StrToID(L"MicaChk"));
			DirectUI::TouchCheckBox *TabbedCheckbox = (DirectUI::TouchCheckBox *)FindDescendent(DirectUI::StrToID(L"TabChk"));
			DirectUI::TouchButton *ThemetoolInstall = (DirectUI::TouchButton *)FindDescendent(DirectUI::StrToID(L"ThemetoolInstall"));
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
				Element::StartDefer(&dwDefer);
				iev->peTarget->SetLayoutPos(-3);
				iev->peTarget->SetVisible(FALSE);
				ThemetoolInstall->SetEnabled(TRUE);

				MicaForEveryoneCheckbox->SetEnabled(TRUE);
				if (MicaForEveryoneCheckbox->GetCheckedState() != DirectUI::CSF_Unchecked)
					TabbedCheckbox->SetEnabled(TRUE);


				DirectUI::CCRadioButton *Win11DefaultMenus = (DirectUI::CCRadioButton *)FindDescendent(DirectUI::StrToID(L"Win11DefaultMenus"));
				DirectUI::CCRadioButton *NilesoftSmall = (DirectUI::CCRadioButton *)FindDescendent(DirectUI::StrToID(L"NilesoftSmall"));
				DirectUI::CCRadioButton *NilesoftFull = (DirectUI::CCRadioButton *)FindDescendent(DirectUI::StrToID(L"NilesoftFull"));
				DirectUI::CCRadioButton *Classic = (DirectUI::CCRadioButton *)FindDescendent(DirectUI::StrToID(L"Classic"));
				DirectUI::CCRadioButton *ClassicTransparent = (DirectUI::CCRadioButton *)FindDescendent(DirectUI::StrToID(L"ClassicTransparent"));

				DirectUI::CCRadioButton *Options[] =
				{
					Win11DefaultMenus,
					NilesoftSmall,
					NilesoftFull,
					Classic,
					ClassicTransparent
				};

				for (size_t i = 0; i < 5; i++)
				{
					Options[i]->SetEnabled(TRUE);
				}

				Element::EndDefer(dwDefer);
			}
		}
	}
	else if (iev->peTarget->GetID() == DirectUI::StrToID(L"BtnRestartExplorer"))
	{
		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			CRectifyUtil::RestartExplorer();

			// hide restart explorer button
			iev->peTarget->SetLayoutPos(-3);
			iev->peTarget->SetVisible(FALSE);
		}
	}
	else if (iev->peTarget->GetID() == DirectUI::StrToID(L"ThemeCmb"))
	{
		if (iev->uidType == DirectUI::Combobox::SelectionChange)
		{
			DirectUI::TouchCheckBox *MicaForEveryoneCheckbox = (DirectUI::TouchCheckBox *)FindDescendent(DirectUI::StrToID(L"MicaChk"));
			DirectUI::TouchCheckBox *TabbedCheckbox = (DirectUI::TouchCheckBox *)FindDescendent(DirectUI::StrToID(L"TabChk"));
			int selection = ((DirectUI::Combobox *)iev->peTarget)->GetSelection();

			ULONG apply_flags = 0;

			// load appy flags
			HKEY Rectify11;
			if (RegCreateKey(HKEY_CURRENT_USER, Rectify11PrefsKey, &Rectify11))
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

			RegQueryValueExW(Rectify11, L"IgnoreBg", 0, NULL, (LPBYTE)&IgnoreBgVal, &size);
			RegQueryValueExW(Rectify11, L"IgnoreCursors", 0, NULL, (LPBYTE)&IgnoreCursorsVal, &size);
			RegQueryValueExW(Rectify11, L"IgnoreIcons", 0, NULL, (LPBYTE)&IgnoreIconsVal, &size);
			RegQueryValueExW(Rectify11, L"IgnoreColors", 0, NULL, (LPBYTE)&IgnoreColorsVal, &size);
			RegQueryValueExW(Rectify11, L"IgnoreSounds", 0, NULL, (LPBYTE)&IgnoreSoundsVal, &size);
			RegQueryValueExW(Rectify11, L"IgnoreScreensavers", 0, NULL, (LPBYTE)&IgnoreScreensaversVal, &size);
			RegCloseKey(Rectify11);

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
				MicaForEveryoneCheckbox->SetCheckedState(hasMica ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
				TabbedCheckbox->SetCheckedState(hasTabbed ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
			}
		}
	}
	else if (iev->peTarget->GetID() == DirectUI::StrToID(L"buttonHelp"))
	{
		if (iev->uidType == DirectUI::Button::Click)
		{
			ShellExecute(0, 0, TEXT("http://rectify11.net"), 0, 0, SW_SHOW);
		}
	}
	else if (iev->peTarget->GetID() == DirectUI::StrToID(L"MicaChk"))
	{
		DirectUI::TouchCheckBox *MicaForEveryoneCheckbox = (DirectUI::TouchCheckBox *)iev->peTarget;
		DirectUI::TouchCheckBox *TabbedCheckbox = (DirectUI::TouchCheckBox *)GetRoot()->FindDescendent(DirectUI::StrToID(L"TabChk"));
		DirectUI::Combobox *ThemeCombo = (DirectUI::Combobox *)GetRoot()->FindDescendent(DirectUI::StrToID(L"ThemeCmb"));
		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			DirectUI::CheckedStateFlags MicaEnabled2 = MicaForEveryoneCheckbox->GetCheckedState();
			DirectUI::CheckedStateFlags TabbedEnabled = TabbedCheckbox->GetCheckedState();

			_pRectifyUtil->SetMicaForEveryoneEnabled(MicaEnabled2 == DirectUI::CSF_Checked ? TRUE : FALSE, TabbedEnabled ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);

			// Enable/disable the tabbed checkbox
			if (TabbedCheckbox != NULL)
			{
				TabbedCheckbox->SetEnabled(MicaEnabled2 == DirectUI::CSF_Checked ? TRUE : FALSE);
			}
		}
	}
	else if (iev->peTarget->GetID() == DirectUI::StrToID(L"TabChk"))
	{
		DirectUI::TouchCheckBox *TabbedCheckbox = (DirectUI::TouchCheckBox *)iev->peTarget;
		DirectUI::Combobox *ThemeCombo = (DirectUI::Combobox *)FindDescendent(DirectUI::StrToID(L"ThemeCmb"));

		if (iev->uidType == DirectUI::TouchButton::Click)
		{
			_pRectifyUtil->SetMicaForEveryoneEnabled(TRUE, TabbedCheckbox->GetCheckedState() == DirectUI::CSF_Checked ? TRUE : FALSE);
		}
	}
	else if (iev->peTarget->GetID() == DirectUI::StrToID(L"ThemetoolInstall"))
	{
		iev->peTarget->SetEnabled(FALSE);
		HRESULT hr = _pRectifyUtil->InstallThemeTool();
		if (FAILED(hr))
		{
			CHAR buffer[1024];
			std::string message = std::system_category().message(hr);

			snprintf(buffer, sizeof(buffer), "Failed to install SecureUxTheme. Error code is %x, which translates to %s.", hr, message.c_str());
			MessageBoxA(_GetMainHwnd(), buffer, "Error during SecureUxTheme install", MB_ICONERROR);
		}

		_UpdateThemetoolStatus();
		iev->peTarget->SetEnabled(TRUE);
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

void RectifyMainPage::_ShowRestartExplorer()
{
	DirectUI::TouchButton *BtnRestartExplorer = (DirectUI::TouchButton *)FindDescendent(DirectUI::StrToID(L"BtnRestartExplorer"));
	BtnRestartExplorer->SetLayoutPos(0);
	BtnRestartExplorer->SetVisible(TRUE);
}

void RectifyMainPage::_UpdateThemeGraphic()
{
	const WCHAR* id = IsDarkTheme() ? MAKEINTRESOURCE(IDB_DARKPREVIEW) : MAKEINTRESOURCE(IDB_LIGHTPREVIEW);
	HBITMAP bmp = (HBITMAP)LoadImage(g_hinst, id, IMAGE_BITMAP, 256, 256, 0);
	if (bmp == NULL)
	{
		return;
	}
	DirectUI::Value *bitmap = DirectUI::Value::CreateGraphic(bmp, 3, 0xffffffff, false, false, false);
	Element *PreviewElement = FindDescendent(DirectUI::StrToID(L"ThemePreview"));
	if (PreviewElement != NULL)
	{
		PreviewElement->SetValue(Element::ContentProp, 1, bitmap);
	}
	bitmap->Release();
}

DEFINE_GUID(SID_PerLayoutPropertyBag, 0xA46E5C25, 0xC09C, 0x4CA8, 0x9A, 0x53, 0x49, 0xCF, 0x7F, 0x86, 0x55, 0x25);

HRESULT RectifyMainPage::_InitNavLinks()
{
	auto pLinks = new(std::nothrow) CControlPanelNavLinks();
	if (!pLinks)
		return E_OUTOFMEMORY;

	CControlPanelNavLink* pLink;
	HRESULT hr = pLinks->AddLinkControlPanel(CPNAV_LIST_TASKS, g_hinst, IDS_UPDATE, L"Rectify11.SettingsCPL", L"pageThemePref", &pLink);
	if (SUCCEEDED(hr))
	{
		hr = pLinks->AddLinkShellEx(CPNAV_LIST_TASKS, g_hinst, IDS_UNINSTALL, L"C:\\Windows\\Rectify11\\Uninstall.exe", L"", &pLink);
		if (SUCCEEDED(hr))
		{
			hr = pLinks->AddLinkControlPanel(CPNAV_LIST_SEEALSO, g_hinst, IDS_SYSINFO, L"Microsoft.System", L"", &pLink);
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
		}
	}

	pLinks->Release();
	return hr;
}

void RectifyMainPage::_UpdateThemetoolStatus()
{
	Element *status = (DirectUI::TouchButton *)FindDescendent(DirectUI::StrToID(L"ThemetoolStatus"));

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
		else {
			if (FAILED(LoadStringW(g_hinst, IDS_OUTDATED, buffer1, 1023)))
			{
				wcscpy_s(buffer1, L"OUTDATED STRING MISSING");
			}

			statusText += buffer1;

			status->SetForegroundStdColor(138); // yellow
		}
	}
	else {
		if (FAILED(LoadStringW(g_hinst, IDS_NOTINSTALLED, buffer1, 1023)))
		{
			wcscpy_s(buffer1, L"NOT INSTALLED STRING MISSING");
		}

		statusText += buffer1;

		status->SetForegroundStdColor(113); // red
	}

	status->SetContentString(statusText.c_str());
}

IFACEMETHODIMP RectifyMainPage::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CControlPanelPage, IFrameNotificationClient),
		{ 0 },
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

	HRESULT hr = IUnknown_GetSite(static_cast<IObjectWithSite *>(this), IID_PPV_ARGS(&_punkSite));
	if (SUCCEEDED(hr))
	{
		DUI_WalkIUnknownElements(this, (PFNELEMENTCALLBACK)DUI_SetSiteOnUnknown, (LPARAM)_punkSite);
	}

	hr = themetool_init();
	if (hr != S_OK && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED))
	{
#ifdef ERROR_MESSAGES
		MessageBox(NULL, TEXT("Failed to initialize SecureUXTheme ThemeTool. Theme information will not be loaded. This may be due to the lack of the ThemeDll.dll in C:\\Windows\\Rectify11\\RectifyControlPanel"), TEXT("FrameProvider::LayoutInitialized"), MB_ICONERROR);
#endif
	}

	Element *root = GetRoot();
	_pRectifyUtil = (IRectifyUtil *)new CRectifyUtil();
	_InitNavLinks();

	DirectUI::Combobox *ThemeCombo = (DirectUI::Combobox *)root->FindDescendent(DirectUI::StrToID(L"ThemeCmb"));
	DirectUI::Button *HelpButton = (DirectUI::Button *)root->FindDescendent(DirectUI::StrToID(L"buttonHelp"));
	DirectUI::TouchCheckBox *MicaForEveryoneCheckbox = (DirectUI::TouchCheckBox *)root->FindDescendent(DirectUI::StrToID(L"MicaChk"));
	DirectUI::TouchCheckBox *TabbedCheckbox = (DirectUI::TouchCheckBox *)root->FindDescendent(DirectUI::StrToID(L"TabChk"));
	Element *version = root->FindDescendent(DirectUI::StrToID(L"RectifyVersion"));
	DirectUI::TouchButton *enableAdmin = (DirectUI::TouchButton *)root->FindDescendent(DirectUI::StrToID(L"Link_EnableAdmin"));
	DirectUI::TouchButton *BtnRestartExplorer = (DirectUI::TouchButton *)root->FindDescendent(DirectUI::StrToID(L"BtnRestartExplorer"));
	DirectUI::TouchButton *ThemetoolInstall = (DirectUI::TouchButton *)root->FindDescendent(DirectUI::StrToID(L"ThemetoolInstall"));

	DirectUI::CCRadioButton *Win11DefaultMenus = (DirectUI::CCRadioButton *)root->FindDescendent(DirectUI::StrToID(L"Win11DefaultMenus"));
	DirectUI::CCRadioButton *NilesoftSmall = (DirectUI::CCRadioButton *)root->FindDescendent(DirectUI::StrToID(L"NilesoftSmall"));
	DirectUI::CCRadioButton *NilesoftFull = (DirectUI::CCRadioButton *)root->FindDescendent(DirectUI::StrToID(L"NilesoftFull"));
	DirectUI::CCRadioButton *Classic = (DirectUI::CCRadioButton *)root->FindDescendent(DirectUI::StrToID(L"Classic"));
	DirectUI::CCRadioButton *ClassicTransparent = (DirectUI::CCRadioButton *)root->FindDescendent(DirectUI::StrToID(L"ClassicTransparent"));

	DirectUI::CCRadioButton *Options[] = { Win11DefaultMenus, NilesoftSmall, NilesoftFull, Classic, ClassicTransparent };

	if (ThemeCombo != NULL)
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
						ThemeCombo->AddString(nameBuffer.c_str());
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
							ThemeCombo->SetSelection(k);
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

		if (version != NULL)
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

	if (MicaForEveryoneCheckbox != NULL)
	{
		MicaForEveryoneCheckbox->SetToggleOnClick(true);
		BOOL MicaEnabled;
		BOOL TabbedEnabled;
		_pRectifyUtil->GetMicaSettings(&MicaEnabled, &TabbedEnabled);

		MicaForEveryoneCheckbox->SetCheckedState(MicaEnabled ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);

		if (!MicaEnabled && TabbedCheckbox != NULL)
		{
			TabbedCheckbox->SetEnabled(FALSE);
		}
	}

	if (TabbedCheckbox != NULL)
	{
		BOOL MicaEnabled;
		BOOL TabbedEnabled;
		_pRectifyUtil->GetMicaSettings(&MicaEnabled, &TabbedEnabled);

		TabbedCheckbox->SetToggleOnClick(true);
		TabbedCheckbox->SetCheckedState(MicaEnabled ? DirectUI::CSF_Checked : DirectUI::CSF_Unchecked);
	}

	if (BtnRestartExplorer != NULL)
	{
		BtnRestartExplorer->SetLayoutPos(-3);
		BtnRestartExplorer->SetVisible(FALSE);
	}

	DWORD menuIndex;

	for (size_t i = 0; i < 5; i++)
	{
		if (!_fAdmin)
			Options[i]->SetEnabled(FALSE);
		else
			Options[i]->SetEnabled(TRUE);
	}
	if (SUCCEEDED(_pRectifyUtil->GetCurrentMenuIndex(&menuIndex)))
	{
		Options[menuIndex]->SetSelected(true);
	}

	if (_fAdmin)
	{
		enableAdmin->SetLayoutPos(-3);
		enableAdmin->SetVisible(FALSE);
		ThemetoolInstall->SetEnabled(TRUE);
	}
	else
	{
		MicaForEveryoneCheckbox->SetEnabled(FALSE);
		TabbedCheckbox->SetEnabled(FALSE);
		ThemetoolInstall->SetEnabled(FALSE);
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

void RectifyMainPage::OnDestroy()
{
	if (_pRectifyUtil != NULL)
	{
		_pRectifyUtil->Release();
		_pRectifyUtil = NULL;
	}

	Element::OnDestroy();
}