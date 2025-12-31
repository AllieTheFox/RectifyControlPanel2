#include "pch.h"

#include "CControlPanelNavLinkCommand.h"

CControlPanelNavLinkCommand::CControlPanelNavLinkCommand()
    : _cmdType(CPNAV_CMDTYPE_NOTIFY)
    , _nId(0)
    , _pszAppletOrCommand(nullptr)
    , _pszAppletPageOrCommandParams(nullptr)
    , _fUseNavPaneProvider(false)
    , _fLogRecentItems(false)
    , _sqmStreamInfo(0, 0, nullptr)
{
}

CControlPanelNavLinkCommand::~CControlPanelNavLinkCommand()
{
    CoTaskMemFree(_pszAppletOrCommand);
    CoTaskMemFree(_pszAppletPageOrCommandParams);
    _pszAppletOrCommand = nullptr;
    _pszAppletPageOrCommandParams = nullptr;

    delete[] _sqmStreamInfo.pSqmStreamEntries;
}

void CControlPanelNavLinkCommand::SetType(CPNAV_CMDTYPE cmdType)
{
    _cmdType = cmdType;
}

void CControlPanelNavLinkCommand::SetId(int nId)
{
    _nId = nId;
}

void CControlPanelNavLinkCommand::SetSQMStream(DWORD dwDatapointId, DWORD cSqmStreamEntries, SQM_STREAM_ENTRY* pSqmStreamEntries)
{
    _sqmStreamInfo.dwDatapointId = dwDatapointId;
    _sqmStreamInfo.cSqmStreamEntries = cSqmStreamEntries;
    _sqmStreamInfo.pSqmStreamEntries = pSqmStreamEntries;
}

HRESULT CControlPanelNavLinkCommand::SetAppletOrCommand(const WCHAR* pszAppletOrCommand, const WCHAR* pszAppletPageOrCommandParams)
{
    HRESULT hr = SHStrDupW(pszAppletOrCommand, &_pszAppletOrCommand);
    if (SUCCEEDED(hr) && pszAppletPageOrCommandParams)
    {
        hr = SHStrDup(pszAppletPageOrCommandParams, &_pszAppletPageOrCommandParams);
    }

    return hr;
}

void CControlPanelNavLinkCommand::UseNavPaneProvider()
{
    _fUseNavPaneProvider = true;
}

void CControlPanelNavLinkCommand::LogRecentItems()
{
    _fLogRecentItems = true;
}
