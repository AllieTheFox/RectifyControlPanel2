#pragma once

typedef struct _SQM_STREAM_ENTRY
{
	DWORD dwType;
	union
	{
		WCHAR *pwszVal;
		DWORD dwVal;
	} val;

} SQM_STREAM_ENTRY, *PSQM_STREAM_ENTRY;

class CControlPanelNavLinkCommand
{
public:
	CControlPanelNavLinkCommand()
		: _cmdType(CPNAV_CMDTYPE_NOTIFY)
		, _nId(0)
		, _pszAppletOrCommand(NULL)
		, _pszAppletPageOrCommandParams(NULL)
		, _fUseNavPaneProvider(false)
		, _fLogRecentItems(false)
		, _sqmStreamInfo(0, 0, NULL)
	{
	}

	~CControlPanelNavLinkCommand()
	{
		CoTaskMemFree(_pszAppletOrCommand);
		CoTaskMemFree(_pszAppletPageOrCommandParams);
		_pszAppletOrCommand = NULL;
		_pszAppletPageOrCommandParams = NULL;

		delete[] _sqmStreamInfo.pSqmStreamEntries;
	}

	void SetType(CPNAV_CMDTYPE cmdType)
	{
		_cmdType = cmdType;
	}

	void SetId(int nId)
	{
		_nId = nId;
	}

	void SetSQMStream(DWORD dwDatapointId, DWORD cSqmStreamEntries, SQM_STREAM_ENTRY *pSqmStreamEntries)
	{
		_sqmStreamInfo.dwDatapointId = dwDatapointId;
		_sqmStreamInfo.cSqmStreamEntries = cSqmStreamEntries;
		_sqmStreamInfo.pSqmStreamEntries = pSqmStreamEntries;
	}

	HRESULT SetAppletOrCommand(LPCWSTR pszAppletOrCommand, LPCWSTR pszAppletPageOrCommandParams)
	{
		HRESULT hr = SHStrDup(pszAppletOrCommand, &_pszAppletOrCommand);
		if (SUCCEEDED(hr) && pszAppletPageOrCommandParams)
		{
			hr = SHStrDup(pszAppletPageOrCommandParams, &_pszAppletPageOrCommandParams);
		}

		return hr;
	}

	CControlPanelNavLinkCommand *Copy()
	{
		return 0; // TODO: Implement this
	}

	bool CanInvoke(IOpenControlPanel *pocp)
	{
		bool fInvoke = true;
		if (_cmdType == CPNAV_CMDTYPE_CONTROLPANEL)
		{
			if (pocp)
			{
				pocp->AddRef();
			}
			else if (FAILED(CoCreateInstance(CLSID_OpenControlPanel, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pocp))))
			{
				return false;
			}

			IControlPanelPrivate *pcpp;
			if (SUCCEEDED(pocp->QueryInterface(IID_PPV_ARGS(&pcpp))))
			{
				pcpp->AddEnumFlag(0x2000);
				pcpp->Release();
			}

			WCHAR szPath[MAX_PATH];
			fInvoke = SUCCEEDED(pocp->GetPath(_pszAppletOrCommand, szPath, ARRAYSIZE(szPath)));
			pocp->Release();
		}

		return fInvoke;
	}

	HRESULT Invoke(HWND, IUnknown *)
	{
		return E_NOTIMPL; // TODO: Implement this
	}

	void UseNavPaneProvider()
	{
		_fUseNavPaneProvider = true;
	}

	void LogRecentItems()
	{
		_fLogRecentItems = true;
	}

	struct SQM_STREAM_INFO
	{
		DWORD dwDatapointId;
		DWORD cSqmStreamEntries;
		SQM_STREAM_ENTRY *pSqmStreamEntries;
	};

private:
	CPNAV_CMDTYPE _cmdType;
	int _nId;
	LPWSTR _pszAppletOrCommand;
	LPWSTR _pszAppletPageOrCommandParams;
	bool _fUseNavPaneProvider;
	bool _fLogRecentItems;
	SQM_STREAM_INFO _sqmStreamInfo;
};
