#pragma once

#include "Rectify11CPL.h"

// TODO: Clean up this file, it is a mess of copypasted IDA pseudocode

int g_id;
HMODULE g_hmod;
HANDLE g_hActCtx;

HINSTANCE g_hinstCC = NULL;

BOOL NT5_ActivateActCtx(HANDLE hActCtx, ULONG_PTR *pulCookie)
{
	*pulCookie = NULL;
	return ActivateActCtx(hActCtx, pulCookie);
}

BOOL NT5_DeactivateActCtx(ULONG_PTR ulCookie)
{
	return DeactivateActCtx(0, ulCookie);
}

void NT5_ReleaseActCtx(void *a1)
{
	ReleaseActCtx(a1);
}

BOOL SHFusionInitializeFromModuleID(HMODULE hMod, int id);

BOOL SHActivateContext(ULONG_PTR *pulCookie)
{
	*pulCookie = 0;

	if (g_hActCtx != INVALID_HANDLE_VALUE)
		return NT5_ActivateActCtx(g_hActCtx, pulCookie);

	if (!g_hmod)
		return TRUE;
	if (g_hmod == INVALID_HANDLE_VALUE)
		return FALSE;

	SHFusionInitializeFromModuleID(g_hmod, g_id);

	return g_hActCtx == INVALID_HANDLE_VALUE || NT5_ActivateActCtx(g_hActCtx, pulCookie);
}

void SHDeactivateContext(ULONG_PTR ulCookie)
{
	if (ulCookie != 0)
	{
		NT5_DeactivateActCtx(ulCookie);
	}
}

HMODULE SHFusionLoadLibrary(const WCHAR* lpLibFileName)
{
	ULONG_PTR ulCookie = 0;
	HMODULE hMod = (HMODULE)SHActivateContext(&ulCookie);
	if (hMod)
	{
		hMod = LoadLibraryEx(lpLibFileName, 0, 0x4000);
		SHDeactivateContext(ulCookie);
	}
	return hMod;
}

BOOL DelayLoadCC()
{
	if (!g_hinstCC)
	{
		g_hinstCC = (HMODULE)SHFusionLoadLibrary(L"comctl32.dll");
		if (!g_hinstCC)
		{
			g_hinstCC = LoadLibraryW(L"comctl32.dll");
		}
	}
	return g_hinstCC != 0;
}

BOOL __stdcall SHFusionInitializeFromModuleID(HMODULE hMod, int id)
{
	if (g_hActCtx == INVALID_HANDLE_VALUE)
	{
		ACTCTXW pActCtx;
		memset(&pActCtx, 0, sizeof(pActCtx));
		if (!hMod)
			hMod = GetModuleHandleW(0);

		WCHAR szPath[MAX_PATH];
		GetModuleFileNameW(hMod, szPath, ARRAYSIZE(szPath));
		pActCtx.lpSource = szPath;
		pActCtx.lpResourceName = MAKEINTRESOURCE(id);
		pActCtx.cbSize = sizeof(pActCtx);
		pActCtx.dwFlags = 136;
		pActCtx.hModule = hMod;

		HANDLE v3 = CreateActCtxW(&pActCtx);
		if (v3 != INVALID_HANDLE_VALUE)
		{
			if (InterlockedCompareExchangePointer((PVOID *)&g_hActCtx, v3, INVALID_HANDLE_VALUE) != INVALID_HANDLE_VALUE)
				ReleaseActCtx(v3);
			DelayLoadCC();
		}
	}

	return g_hActCtx != INVALID_HANDLE_VALUE;
}