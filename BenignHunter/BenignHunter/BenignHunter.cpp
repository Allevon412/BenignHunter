// BenignHunter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <winternl.h>
#include <stdio.h>

volatile DWORD g_NTDLLSTR1 = 0x46414163;	// 'ldtn' ^ 0x2A25350D = 0x6C64746E ^ 0x2A25350D
volatile DWORD g_NTDLLSTR2 = 0x4643Eb76;	// 'ld.l' ^ 0x2A27C51A = 0x6C642E6C ^ 0x2A27C51A

UINT32 CRC32BA(IN LPCSTR String) {

	UINT32      uMask = 0x00,
		uHash = 0xFFFFEFFF;
	INT         i = 0x00;

	while (String[i] != 0) {

		uHash = uHash ^ (UINT32)String[i];

		for (int ii = 0; ii < 8; ii++) {

			uMask = -1 * (uHash & 1);
			uHash = (uHash >> 1) ^ (0xEDB88320 & uMask);
		}

		i++;
	}

	return ~uHash;
}

#define HASH(STR)(CRC32BA(STR))

int main()
{
#if defined(_WIN64)
#define SEARCH_BYTES 0x8b4c
#else
#define SEARCH_BYTES 0x00b8
#endif

	PLDR_DATA_TABLE_ENTRY		pDataTableEntry = NULL;
	PIMAGE_NT_HEADERS		    pImgNtHdrs = NULL;
	PIMAGE_EXPORT_DIRECTORY		pExportDirectory = NULL;
	ULONG_PTR			        uNtdllBase = NULL;
	PDWORD				        pdwFunctionNameArray = NULL;
	PDWORD				        pdwFunctionAddressArray = NULL;
	PWORD				        pwFunctionOrdinalArray = NULL;

	// Fetch ntdll.dll base address
	// not working for some reason going to use simpler method.
	uNtdllBase = (ULONG_PTR)GetModuleHandleA("Ntdll.dll");

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)uNtdllBase;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)uNtdllBase + dosHeader->e_lfanew);
	DWORD exportDirectoryRVA = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)uNtdllBase + exportDirectoryRVA);

	if (!uNtdllBase)
		return FALSE;

	pdwFunctionNameArray = (PDWORD)(uNtdllBase + pExportDirectory->AddressOfNames);
	pdwFunctionAddressArray = (PDWORD)(uNtdllBase + pExportDirectory->AddressOfFunctions);
	pwFunctionOrdinalArray = (PWORD)(uNtdllBase + pExportDirectory->AddressOfNameOrdinals);

	// Store Zw* syscalls addresses
	for (int i = 0; i < pExportDirectory->NumberOfNames; i++) {

		CHAR* pFunctionName = (CHAR*)(uNtdllBase + pdwFunctionNameArray[i]);

		if (*(unsigned short*)pFunctionName == 'wZ') {
			ULONG_PTR uAddress = (ULONG_PTR)(uNtdllBase + pdwFunctionAddressArray[pwFunctionOrdinalArray[i]]);
			WORD Bytes = *(unsigned short*)uAddress;
			if ((Bytes & SEARCH_BYTES) == SEARCH_BYTES) // if the sys call is not hooked. mov r10, rcx in x64 or x32
			{
				SHORT SSN = 0;
				UINT32 u32Hash = HASH(pFunctionName);
				printf("Syscall Name: %s\n", pFunctionName);
				printf("Syscall Hash: 0x%08X\n", u32Hash);
#if defined(_WIN64)
				SSN = *(unsigned short*)((PBYTE)uAddress + 4);
#else
				SSN = *(unsigned short*)((PBYTE)uAddress + 1);
#endif
				printf("SSN %d\n", SSN);
			}
		}
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
