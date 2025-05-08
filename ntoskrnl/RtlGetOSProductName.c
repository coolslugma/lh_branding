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

NTSTATUS NTAPI
CheckInstalledValue(PHANDLE KeyHandle, CHAR Access, ACCESS_MASK DesiredAccess)
{
	NTSTATUS Status;
	HANDLE Handle = NULL;
	UNICODE_STRING ValueName;
	ULONG ResultLength;
	CHAR Buffer[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + 256];
	PKEY_VALUE_PARTIAL_INFORMATION KeyValue = (PKEY_VALUE_PARTIAL_INFORMATION) Buffer;
	ACCESS_MASK AccessMask = (ACCESS_MASK)((Access << 8) | DesiredAccess);
	BOOLEAN Found = FALSE;

	Status = ZwOpenKey(KeyHandle, AccessMask, (POBJECT_ATTRIBUTES)&DesiredAccess);
	if (NT_SUCCESS(Status)) {
		RtlInitUnicodeString(&ValueName, L"Installed");
		Status = ZwQueryValueKey(*KeyHandle, &ValueName, KeyValuePartialInformation, KeyValue, sizeof(Buffer), &ResultLength);
		if (NT_SUCCESS(Status) && KeyValue->DataLength > 0)
			Found = TRUE;

		ZwClose(*KeyHandle);
	}

	return NT_SUCCESS(Found);
}

BOOLEAN NTAPI
IsFreestyle(VOID)
{
	NTSTATUS Status;
	HANDLE KeyHandle = NULL;
	CHAR Access = 0;
	OBJECT_ATTRIBUTES ObjAttr;
	UNICODE_STRING Path;

	RtlInitUnicodeString(&Path, L"\\Registry\\Machine\\System\\WPA\\MediaCenter");
	InitializeObjectAttributes(&ObjAttr, &Path, OBJ_CASE_INSENSITIVE, NULL, NULL);

	Status = CheckInstalledValue(&KeyHandle, Access, (ACCESS_MASK)&ObjAttr);
	if (NT_SUCCESS(Status))
		return TRUE;
	else
		return FALSE;
}

BOOLEAN NTAPI
IsTabletPC(VOID)
{
	NTSTATUS Status;
	HANDLE KeyHandle = NULL;
	CHAR Access = 0;
	OBJECT_ATTRIBUTES ObjAttr;
	UNICODE_STRING Path;

	RtlInitUnicodeString(&Path, L"\\Registry\\Machine\\System\\WPA\\TabletPC");
	InitializeObjectAttributes(&ObjAttr, &Path, OBJ_CASE_INSENSITIVE, NULL, NULL);

	Status = CheckInstalledValue(&KeyHandle, Access, (ACCESS_MASK)&ObjAttr);
	if (NT_SUCCESS(Status))
		return TRUE;
	else
		return FALSE;
}

BOOLEAN NTAPI
IsApplianceServer(VOID)
{
	NTSTATUS Status;
	HANDLE KeyHandle = NULL;
	CHAR Access = 0;
	OBJECT_ATTRIBUTES ObjAttr;
	UNICODE_STRING Path;

	RtlInitUnicodeString(&Path, L"\\Registry\\Machine\\System\\WPA\\ApplianceServer");
	InitializeObjectAttributes(&ObjAttr, &Path, OBJ_CASE_INSENSITIVE, NULL, NULL);

	Status = CheckInstalledValue(&KeyHandle, Access, (ACCESS_MASK)&ObjAttr);
	if (NT_SUCCESS(Status))
		return TRUE;
	else
		return FALSE;
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
		if (((CHAR *)&VersionInfo.dwMajorVersion)[2] != 1)
			ProductName = (LPWSTR)L"Longhorn Server";
        AppendConditionally(Buffer, TRUE, ProductName);
    }

    // Product type (aka edition).
    if (Flags & OS_PRODUCTNAME_TYPE) {
        ProductType = NULL;
        if (!(VersionInfo.dwMajorVersion & 0x40)) {
			if (IsFreestyle())
				ProductType = (LPWSTR)L"Media Center Edition";
			else if (IsTabletPC())
				ProductType = (LPWSTR)L"Tablet PC Edition";
			else if (VersionInfo.dwMajorVersion & 0x200)
			    ProductType = (LPWSTR)L"Home Edition";
			else if (((CHAR *)&VersionInfo.dwMajorVersion)[2] == 1)
			    ProductType = (LPWSTR)L"Professional";
			else if (IsApplianceServer())
				ProductType = (LPWSTR)L"Appliance Server";
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
