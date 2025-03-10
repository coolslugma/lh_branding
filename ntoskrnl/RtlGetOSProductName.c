/*
 * This file is a part of the lh_branding project.
 */

static void
append_str(PUNICODE_STRING dest, char str, PCWSTR src)
{
	if ((str != '\0') && (RtlAppendUnicodeToString(dest, L" ") < 0))
		return;

	RtlAppendUnicodeToString(dest, src);
}

/*
 * ntoskrnl.exe
 * 0x16ed38  1157  RtlGetOSProductName
 *
 * CURRENTLY A STUB
 */
int
RtlGetOSProductName(PUNICODE_STRING buf, OS_PRODUCTNAME productName)
{
	dest = buf;
	if (productName == 0)
		return 
	if (buf->MaximumLength == 0)
		return STATUS_INVALID_PARAMETER;

	*buf->Buffer = L'\0';
	buf->Length = 0;
}
