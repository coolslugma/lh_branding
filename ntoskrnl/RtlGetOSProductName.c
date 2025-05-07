#include <ntifs.h>

#include "lh_branding.h"

VOID NTAPI
AppendConditionally(PUNICODE_STRING Destination, BOOLEAN Condition, LPCWSTR Suffix)
{
    NTSTATUS Status;

    if (Condition) {
        Status = RtlAppendUnicodeToString(Destination, L" ");
        if (!NT_SUCCESS(Status))
		    return;
    }

    RtlAppendUnicodeToString(Destination, Suffix);
}

/*
 * @implemented
 *
 * ntoskrnl.exe
 * 0x16ed38  1157  RtlGetOSProductName
 *
 * ntdll.dll
 * 0x5b678  1182  RtlGetOSProductName
 */
NTSTATUS NTAPI
RtlGetOSProductName(OUT PUNICODE_STRING Buffer, IN ULONG Flags)
{
    NTSTATUS Status;
    LPWSTR ProductName, ProductType;
    OSVERSIONINFOW VersionInfo;

    if (!Flags)
        return STATUS_INVALID_PARAMETER;

    // BUG: This is absolutely horrendous..why would MS do this?
    // They check the second byte of the buffer for a NUL char, instead
    // of its size, and if it is, it reports that the buffer is too
    // small...
    if (Buffer->Buffer[1] == L'\0')
        return STATUS_BUFFER_TOO_SMALL;

    Buffer->Buffer[0] = L'\0';
    Buffer->Length = 0;

    VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    Status = RtlGetVersion(&VersionInfo);
    if (Status < 0)
        return Status;

    // Company information.
    if (Flags & OS_PRODUCTNAME_COMPANY)
        AppendConditionally(Buffer, FALSE, L"Microsoft");

    // Generic operating system family.
    if (Flags & OS_PRODUCTNAME_FAMILY_GENERIC)
        AppendConditionally(Buffer, TRUE, L"Windows");

    // Specific operating system family.
    else if (Flags & OS_PRODUCTNAME_FAMILY_SPECIFIC) {
        ProductName = (LPWSTR)L"Longhorn";
        AppendConditionally(Buffer, TRUE, ProductName);
    }

    // Product type (aka edition).
    if (Flags & OS_PRODUCTNAME_TYPE) {
        ProductType = NULL;
        if (!(VersionInfo.dwMajorVersion & 0x40)) {
			if (VersionInfo.dwMajorVersion & 0x200)
			    ProductType = (LPWSTR)L"Home Edition";
			else if (((CHAR *)&VersionInfo.dwMajorVersion)[2] == 1)
			    ProductType = (LPWSTR)L"Professional";
			else if (VersionInfo.dwMajorVersion & 0x400)
			    ProductType = (LPWSTR)L"Web Server";
			else if (VersionInfo.dwMajorVersion & 0x21)
			    ProductType = (LPWSTR)L"Small Business Server";
			else if ((CHAR)VersionInfo.dwMajorVersion < 0)
			    ProductType = (LPWSTR)L"Datacenter Server";
			else if (VersionInfo.dwMajorVersion & 0x02)
			    ProductType = (LPWSTR)L"Enterprise Server";
            else
                ProductType = (LPWSTR)L"Standard Server";
        }
		else
            ProductType = (LPWSTR)L"Embedded";

        AppendConditionally(Buffer, TRUE, ProductType);
    }

    if (Flags & OS_PRODUCTNAME_VERSION)
        AppendConditionally(Buffer, TRUE, L"Version 2003");

    if (Flags & OS_PRODUCTNAME_COPYRIGHT)
        AppendConditionally(Buffer, TRUE, L"Copyright © 1985-2003 Microsoft Corporation");

    return STATUS_SUCCESS;
}
