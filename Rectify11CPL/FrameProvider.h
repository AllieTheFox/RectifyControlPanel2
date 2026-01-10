#include <propkey.h>

#include "CRectifyUtil.h"

class FrameProvider
	: public DirectUI::XProvider
	, public IDUIElementProviderInit
	, public IFrameNotificationClient
	, public IFrameShellViewClient
	, public IObjectWithSite
	, public IServiceProvider
{
public:
	FrameProvider(HINSTANCE hinst, const WCHAR* pszCommonResidToCreate);

	// == Begin IUnknown Impl ==
	IFACEMETHOD(QueryInterface)(_In_ REFIID riid, _Out_ LPVOID *ppv) override;
	IFACEMETHOD_(ULONG, AddRef)() override;
	IFACEMETHOD_(ULONG, Release)() override;
	// == End IUnknown Impl ==

	// == Begin IDUIElementProviderInit Impl ==
	IFACEMETHOD(SetResourceID)(UINT uID) override;
	IFACEMETHOD(OptionallyTakeInitialFocus)(BOOL *pbTookFocus) override;
	// == End IDUIElementProviderInit Impl ==

	// == Begin DirectUI::XProvider Impl ==
	IFACEMETHOD(CreateDUI)(DirectUI::IXElementCP *pcp, HWND *phwnd) override;
	// == End DirectUI::XProvider Impl ==

	// == Begin IFrameNotificationClient Impl ==
	IFACEMETHOD(LayoutInitialized)() override;
	IFACEMETHOD(Notify)(const WCHAR* pszChangedProp) override;
	IFACEMETHOD(OnNavigateAway)() override;
	IFACEMETHOD(OnInnerElementDestroyed)() override;
	// == End IFrameNotificationClient Impl ==

	// == Begin IFrameShellViewClient Impl ==
	IFACEMETHOD(OnSelectedItemChanged)() override;
	IFACEMETHOD(OnSelectionChanged)() override;
	IFACEMETHOD(OnContentsChanged)() override;
	IFACEMETHOD(OnFolderChanged)() override;
	// == End IFrameShellViewClient Impl ==

	// == Begin IObjectWithSite Impl ==
	IFACEMETHOD(SetSite)(IUnknown *punkSite) override;
	IFACEMETHOD(GetSite)(REFIID riid, void **ppv) override;
	// == End IObjectWithSite Impl ==

	// == Begin IServiceProvider Impl ==
	IFACEMETHOD(QueryService)(REFGUID guidService, REFIID riid, void **ppv) override;
	// == End IServiceProvider Impl ==

	HRESULT Init();

protected:
	~FrameProvider() override;

private:
	IUnknown *_punkSite;
	HINSTANCE _hinst;
	WCHAR _szResID[MAX_PATH];
	HRESULT _hrInit;
	DirectUI::XResourceProvider *_prprov;
};