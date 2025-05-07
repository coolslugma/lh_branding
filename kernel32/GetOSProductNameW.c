#include <ntifs.h>

#include "lh_branding.h"

typedef ULONG DWORD;

/*
 * @implemented
 */
BOOL
WINAPI
GetOSProductNameW(OUT LPWSTR ProductName,
                  IN ULONG ProductNameLength,
                  IN DWORD Flags)
{
    NTSTATUS Status;
    DWORD Error;
    UNICODE_STRING ProductNameOut;

    if (!ProductNameLength)
        Status = STATUS_BUFFER_TOO_SMALL;
    else {
        ProductNameOut.Buffer = ProductName;
        ProductNameOut.MaximumLength = (USHORT)(ProductNameLength * sizeof(WCHAR));
        Status = RtlGetOSProductName(&ProductNameOut, Flags);
    }

    Error = RtlNtStatusToDosError(Status);
    SetLastError(Error);

    return NT_SUCCESS(Status);
}