#pragma once

#include "CControlPanelNavLinkCommand.h"

class CControlPanelNavLink
{
public:
    explicit CControlPanelNavLink(CPNAV_LIST list);
    ~CControlPanelNavLink();

    static HRESULT Create(CPNAV_LIST list, CControlPanelNavLink** ppLink);

    CPNAV_LIST GetList();

    HRESULT SetName(HINSTANCE hInstance, UINT nResId);
    HRESULT SetName(const WCHAR* pszName);
    WCHAR* GetName();

    HRESULT SetNameAcc(HINSTANCE hInstance, UINT nResId);
    HRESULT SetNameAcc(const WCHAR* pszNameAcc);
    WCHAR* GetNameAcc();

    HRESULT SetIcon(HINSTANCE hInstance, int nIconId);
    HRESULT SetIcon(HICON hIcon);
    HICON GetIcon();
    void SetSQMStream(DWORD dwDatapointId, DWORD cSqmStreamEntries, SQM_STREAM_ENTRY* pSqmStreamEntries);

    HRESULT SetCommandNotify(int nLinkId);
    HRESULT SetCommandShellEx(const WCHAR* pszCommand, const WCHAR* pszParams);
    HRESULT SetCommandControlPanel(const WCHAR* pszApplet, const WCHAR* pszAppletPage, bool fLogRecentItems);

    CControlPanelNavLinkCommand* GetCommand();

    void SetDisabled(bool fDisabled);
    bool GetDisabled();

private:
    CPNAV_LIST _list;
    WCHAR* _pszName;
    WCHAR* _pszNameAcc;
    HICON _hIcon;
    CControlPanelNavLinkCommand _cmd;
    bool _fDisabled;
};
