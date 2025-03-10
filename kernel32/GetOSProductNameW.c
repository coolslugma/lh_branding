static void
break_to_debugger(DWORD error)
{
	if ((debug != 0) && (error == debug))
		DbgBreakPoint();

	if (LastErrorValue != error)
		LastErrorValue = error;
}

/*
 * kernel32.dll
 * 0x39583  440  GetOSProductNameW
 *
 * CURRENTLY A STUB
 */
int
GetOSProductNameW(LPCWSTR sb, uint length, OS_PRODUCTNAME productName)
{
	NTSTATUS status;
	UNICODE_STRING buf;

	if (length == 0) {
		status = STATUS_INVALID_PARAMETER;
	} else {
		status = RtlGetOSProductName(&buf, productName);
	}

	error = RtlNtStatusToDosError(status);
	break_to_debugger(error);

	if (status < 0)
		return 0;
	else
		return 1;
}