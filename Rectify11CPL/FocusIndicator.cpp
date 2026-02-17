#include "pch.h"

#include "FocusIndicator.h"

DirectUI::IClassInfo* CFocusIndicator::Class = nullptr;

DirectUI::IClassInfo* CFocusIndicator::GetClassInfoW()
{
    return Class;
}

static DirectUI::PropertyInfoData dataimpFirstTabTargetProp;
static const int vvimpFirstTabTargetProp[] = { 14, -1 };
static DirectUI::PropertyInfo impFirstTabTargetProp =
{
    /*pszName*/ L"firsttabtarget",
    /*fFlags*/ DirectUI::PF_Normal,
    /*fGroups*/ DirectUI::PG_None,
    /*pValidValues*/ vvimpFirstTabTargetProp,
    /*pEnumMaps*/ nullptr,
    /*DefaultProc*/ &DirectUI::Value::GetAtomZero,
    /*pData*/ &dataimpFirstTabTargetProp
};

const DirectUI::PropertyInfo* CFocusIndicator::FirstTabTargetProp = &impFirstTabTargetProp;

ATOM CFocusIndicator::GetTargetId()
{
    DirectUI::Value* pv = GetValue(FirstTabTargetProp, 2, nullptr);
    ATOM v = pv->GetAtom();
    pv->Release();
    return v;
}

HRESULT CFocusIndicator::Register()
{
    const DirectUI::PropertyInfo* s_rgProperties[] =
    {
        FirstTabTargetProp
    };
    return DirectUI::ClassInfo<CFocusIndicator, Element>::RegisterGlobal(g_hinst, L"FocusIndicator", s_rgProperties, ARRAYSIZE(s_rgProperties));
}

HRESULT CFocusIndicator::Create(Element* pParent, DWORD* pdwDeferCookie, Element** ppElement)
{
    HRESULT hr = DirectUI::CreateAndInit<CFocusIndicator, int>(0, pParent, pdwDeferCookie, ppElement);
    if (SUCCEEDED(hr))
    {
        hr = (*ppElement)->SetActive(0);
    }
    return hr;
}
