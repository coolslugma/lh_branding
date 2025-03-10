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
	ULONG error;
	LPCWSTR buf[2];

	if (length == 0) {
		status = -0x3fffffdd;
	} else {
		buf[0] = (LPCWSTR)((uint)(ushort)((short)length * 2) << 0x10);
		buf[1] = sb;
		status = RtlGetOSProductName((PUNICODE_STRING)buf, productName);
	}

	error = RtlNtStatusToDosError(status);
	break_to_debugger(error);

	if (status < 0)
		return 0;
	else
		return 1;
}
