#pragma once

typedef struct _SQM_STREAM_ENTRY
{
    DWORD dwType;

    union
    {
        WCHAR* pwszVal;
        DWORD dwVal;
    } val;
} SQM_STREAM_ENTRY, *PSQM_STREAM_ENTRY;

class CControlPanelNavLinkCommand final
{
public:
    CControlPanelNavLinkCommand();
    ~CControlPanelNavLinkCommand();

    void SetType(CPNAV_CMDTYPE cmdType);
    void SetId(int nId);
    void SetSQMStream(DWORD dwDatapointId, DWORD cSqmStreamEntries, SQM_STREAM_ENTRY* pSqmStreamEntries);
    HRESULT SetAppletOrCommand(const WCHAR* pszAppletOrCommand, const WCHAR* pszAppletPageOrCommandParams);

    // I could not find any of the following 3 methods in any CPL page that ships with Windows.
    CControlPanelNavLinkCommand* Copy();
    bool CanInvoke(IOpenControlPanel*);
    HRESULT Invoke(HWND, IUnknown*);
    void UseNavPaneProvider();
    void LogRecentItems();

    struct SQM_STREAM_INFO
    {
        DWORD dwDatapointId;
        DWORD cSqmStreamEntries;
        SQM_STREAM_ENTRY* pSqmStreamEntries;
    };

private:
    CPNAV_CMDTYPE _cmdType;
    int _nId;
    WCHAR* _pszAppletOrCommand;
    WCHAR* _pszAppletPageOrCommandParams;
    bool _fUseNavPaneProvider;
    bool _fLogRecentItems;
    SQM_STREAM_INFO _sqmStreamInfo;
};
