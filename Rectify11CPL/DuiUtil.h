#pragma once

void CALLBACK DUI_SetSiteOnUnknown(IUnknown* punk, LPARAM lparam);
void CALLBACK DUI_SendSelectionChangedToUnknown(IUnknown* punk, LPARAM lparam);
void CALLBACK DUI_SendSelectedItemChangedToUnknown(IUnknown* punk, LPARAM lparam);
void CALLBACK DUI_SendNotificationToUnknown(IUnknown* punk, LPARAM lparam);
void CALLBACK DUI_SendNavigateAwayToUnknown(IUnknown* punk, LPARAM lparam);
void CALLBACK DUI_SendInitializationToUnknown(IUnknown* punk, LPARAM lparam);
void CALLBACK DUI_SendFolderChangedToUnknown(IUnknown* punk, LPARAM lparam);
void CALLBACK DUI_SendContentsChangedToUnknown(IUnknown* punk, LPARAM lparam);

typedef void (CALLBACK *PFNELEMENTCALLBACK)(IUnknown* punk, LPARAM lParam);
void DUI_WalkIUnknownElements(DirectUI::Element* pe, PFNELEMENTCALLBACK pfn, LPARAM lparam);

HRESULT WINAPI DUIFramework_InitDUI();
HRESULT WINAPI DUIFramework_UninitDUI();
HRESULT WINAPI DUIFramework_SetFocusByFocusIndicator(DirectUI::Element* peStart);

#define GetDUIPropIndirect(get, prop, index)								\
	*ppv = DirectUI::Element::GetValue(&imp##prop##Prop, index, nullptr);	\
	return (*ppv)->get();

#define GetDUIProp(type, get, prop, index)												\
	DirectUI::Value* pv = DirectUI::Value::GetValue(imp##prop##Prop, index, nullptr);	\
	type v = pv->get();																	\
	pv->Release();																		\
	return v;																			\

#define SetDUIProp(createFn, pProp, iIndex)										\
    DirectUI::Value* pv = DirectUI::Value::createFn;							\
	HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;										\
	if (SUCCEEDED(hr))															\
	{																			\
		hr = DirectUI::Element::SetValue(&imp##pProp##Prop, iIndex, pv);		\
		if (SUCCEEDED(hr))														\
		{																		\
			pv->Release();														\
			return hr;															\
		}																		\
	}																			\