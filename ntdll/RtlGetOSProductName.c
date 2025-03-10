/*
 * Flags to replicate different versions of the function:
 * EARLY_LH: Longhorn builds 3683-4020
 * PIGLATIN: Longhorn builds 4029-4042 (main.030905-1800)
 * LH_2004: Longhorn builds 4042 (Lab06_n.030909-1709) - 4093
 *
 * You can only define one out of the three available.
 */
//#define EARLY_LH	1
//#define PIGLATIN	1
#define LH_2004		1
#if !defined(EARLY_LH) || !defined(PIGLATIN) || !defined(LH_2004)
#error "You need to specify a specific Longhorn variant."
#endif

/*
 * ntdll.dll
 * 0x5b678  1182  RtlGetOSProductName
 *
 * CURRENTLY A STUB
 */

