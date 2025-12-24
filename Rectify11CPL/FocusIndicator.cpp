#include "pch.h"

#include "FocusIndicator.h"

IClassInfo* CFocusIndicator::Class = nullptr;

IClassInfo* CFocusIndicator::GetClassInfo()
{
	return Class;
}	

static const int vvimpFirstTabTargetProp[] = { 14, -1 };
static DirectUI::PropertyInfoData dataimpFirstTabTargetProp;
static const DirectUI::PropertyInfo impFirstTabTargetProp =
{
  .pszName = L"firsttabtarget",
  .fFlags = 0x2,
  .fGroups = 0x0,
  .pValidValues = vvimpFirstTabTargetProp,
  .pEnumMaps = NULL,
  .DefaultProc = &DirectUI::Value::GetAtomZero,
  .pData = &dataimpFirstTabTargetProp
};

ATOM CFocusIndicator::GetTargetId()
{
	DirectUI::Value* Value = DirectUI::Element::GetValue(&impFirstTabTargetProp, 2, 0);
	ATOM Atom = Value->GetAtom();
	Value->Release();
	return Atom;
}

HRESULT CFocusIndicator::Register()
{
	constexpr const DirectUI::PropertyInfo *pProps[] =
	{
		&impFirstTabTargetProp
	};

	return DirectUI::ClassInfo<CFocusIndicator, DirectUI::Element>::RegisterGlobal(g_hinst, L"FocusIndicator", pProps, ARRAYSIZE(pProps));
}

HRESULT CFocusIndicator::Create(DirectUI::Element *pParent, DWORD *pdwDeferCookie, DirectUI::Element **ppElement)
{
	HRESULT hr = DirectUI::CreateAndInit<CFocusIndicator, UINT>(0, pParent, pdwDeferCookie, ppElement);
	if (SUCCEEDED(hr))
	{
		hr = (*ppElement)->SetActive(0);
	}

	return hr;
}
