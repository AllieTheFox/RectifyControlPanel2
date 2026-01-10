#pragma once

class CFocusIndicator : public DirectUI::Element
{
public:
    static HRESULT Create(Element* pParent, DWORD* pdwDeferCookie, Element** ppElement);

    static DirectUI::IClassInfo* Class;
    DirectUI::IClassInfo* GetClassInfoW() override;
    static HRESULT Register();

    static const DirectUI::PropertyInfo* FirstTabTargetProp;
    ATOM GetTargetId();
};