#pragma once

#include "CControlPanelNavLinkCommand.h"

class CControlPanelNavLink // should be 88 bytes (0x58) in size
{
public:
	CControlPanelNavLink(CPNAV_LIST list);
	~CControlPanelNavLink();

	static HRESULT Create(CPNAV_LIST list, CControlPanelNavLink **ppLink);

	CPNAV_LIST GetList()
	{
		return _list;
	}

	HRESULT SetName(HINSTANCE hInstance, UINT nResId);
	HRESULT SetName(LPCWSTR pszName);
	LPWSTR GetName() { return _pszName; }

	HRESULT SetNameAcc(HINSTANCE hInstance, UINT nResId);
	HRESULT SetNameAcc(LPCWSTR pszNameAcc);

	LPWSTR GetNameAcc()
	{
		return !_pszName ? _pszNameAcc : _pszName;
	}

	HRESULT SetIcon(HINSTANCE hInstance, int nIconId);
	HRESULT SetIcon(HICON hIcon);

	HICON GetIcon()
	{
		return CopyIcon(_hIcon);
	}

	void SetSQMStream(DWORD dwDatapointId, DWORD cSqmStreamEntries, SQM_STREAM_ENTRY *pSqmStreamEntries)
	{
		_cmd.SetSQMStream(dwDatapointId, cSqmStreamEntries, pSqmStreamEntries);
	}

	HRESULT SetCommandNotify(int nLinkId)
	{
		if (nLinkId <= 0)
			return E_INVALIDARG;

		_cmd.SetType(CPNAV_CMDTYPE_NOTIFY);
		_cmd.SetId(nLinkId);
		return S_OK;
	}

	HRESULT SetCommandShellEx(LPCWSTR pszCommand, LPCWSTR pszParams)
	{
		_cmd.SetType(CPNAV_CMDTYPE_SHELLEX);
		return _cmd.SetAppletOrCommand(pszCommand, pszParams);
	}

	HRESULT SetCommandControlPanel(LPCWSTR pszApplet, LPCWSTR pszAppletPage, bool fLogRecentItems)
	{
		_cmd.SetType(CPNAV_CMDTYPE_CONTROLPANEL);

		if (fLogRecentItems)
			_cmd.LogRecentItems();

		return _cmd.SetAppletOrCommand(pszApplet, pszAppletPage);
	}

	CControlPanelNavLinkCommand *GetCommand()
	{
		return _cmd.Copy();
	}

	void SetDisabled(bool fDisabled)
	{
		_fDisabled = fDisabled;
	}

	bool GetDisabled()
	{
		return _fDisabled;
	}

private:
	CPNAV_LIST _list;
	LPWSTR _pszName;
	LPWSTR _pszNameAcc;
	HICON _hIcon;
	CControlPanelNavLinkCommand _cmd;
	bool _fDisabled;
};