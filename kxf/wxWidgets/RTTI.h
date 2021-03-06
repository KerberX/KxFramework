#pragma once
#include <wx/object.h>
#include <wx/xti.h>

#define	KxWxRTTI_ImplementClassDynamic(Name, CClassName, Base1CClassName)														\
wxClassInfo CClassName::ms_classInfo(wxT(#Name), &Base1CClassName::ms_classInfo, nullptr, (int)sizeof(CClassName), nullptr);			\
wxClassInfo* CClassName::GetClassInfo() const																					\
{																																\
	return &CClassName::ms_classInfo;																							\
}

#define	KxWxRTTI_ImplementClassDynamic2(Name, CClassName, Base1CClassName, Base2CClassName)															\
wxClassInfo CClassName::ms_classInfo(wxT(#Name), &Base1CClassName::ms_classInfo, &Base2CClassName::ms_classInfo, (int)sizeof(CClassName), nullptr);	\
wxClassInfo* CClassName::GetClassInfo() const																										\
{																																					\
	return &CClassName::ms_classInfo;																												\
}
