/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch101: POSIX ustar (tar) 512-byte header parse.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required).
 * No third-party tar source was copied.
 *
 * Surface:
 *   uint64_t gj_tar_octal(const char *p, size_t n);
 *     Parse up to n bytes of leading-space-tolerant octal digits.
 *     Stops at first non-digit (NUL, space, or other). Returns value
 *     (0 if empty / no digits).
 *   int gj_tar_checksum_ok(const unsigned char hdr[512]);
 *     Verify POSIX tar header checksum (chksum field treated as eight
 *     ASCII spaces during the sum). Returns 1 if ok, 0 otherwise.
 *   int gj_tar_header_parse(const unsigned char hdr[512],
 *                           char name_out[256], uint64_t *size,
 *                           int *typeflag);
 *     Parse name (ustar prefix + name when magic is "ustar"), size, and
 *     typeflag. Verifies checksum. Returns 0 on success, -1 on error
 *     (errno set). name_out is always NUL-terminated on success.
 *   __gj_tar_octal / __gj_tar_checksum_ok / __gj_tar_header_parse
 *   __libcgj_batch101_marker = "libcgj-batch101"
 *
 * USTAR layout (POSIX.1-1988 / IEEE Std 1003.1):
 *   name[100] mode[8] uid[8] gid[8] size[12] mtime[12] chksum[8]
 *   typeflag[1] linkname[100] magic[6] version[2] uname[32] gname[32]
 *   devmajor[8] devminor[8] prefix[155] pad[12]
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch101_marker[] = "libcgj-batch101";

/* Fixed header size and field offsets (POSIX ustar). */
#define B101_HDR_SZ     512u
#define B101_OFF_NAME   0u
#define B101_LEN_NAME   100u
#define B101_OFF_SIZE   124u
#define B101_LEN_SIZE   12u
#define B101_OFF_CHKSUM 148u
#define B101_LEN_CHKSUM 8u
#define B101_OFF_TYPE   156u
#define B101_OFF_MAGIC  257u
#define B101_OFF_PREFIX 345u
#define B101_LEN_PREFIX 155u
#define B101_NAME_CAP   256u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Field length without relying on libc: count non-NUL bytes up to cbMax.
 * Trailing padding may be space or NUL; we stop at the first NUL only so
 * full-length name/prefix (no terminator) is preserved.
 */
static size_t
b101_field_len(const unsigned char *pFld, size_t cbMax)
{
	size_t i;

	if (pFld == NULL) {
		return 0u;
	}
	for (i = 0u; i < cbMax; i++) {
		if (pFld[i] == 0u) {
			return i;
		}
	}
	return cbMax;
}

/* True if magic is "ustar" (POSIX or old GNU "ustar "). */
static int
b101_is_ustar(const unsigned char *pHdr)
{
	const unsigned char *pM = pHdr + B101_OFF_MAGIC;

	return pM[0] == (unsigned char)'u' && pM[1] == (unsigned char)'s' &&
	       pM[2] == (unsigned char)'t' && pM[3] == (unsigned char)'a' &&
	       pM[4] == (unsigned char)'r';
}

/* ---- public: octal field parse ----------------------------------------- */

/*
 * Parse an octal numeric field of width n.
 * Leading spaces and NULs are skipped; digits '0'..'7' accumulate.
 * Stops at the first non-digit. Empty field yields 0.
 * NULL p or n == 0 yields 0.
 */
uint64_t
gj_tar_octal(const char *p, size_t n)
{
	uint64_t u64Val = 0u;
	size_t i;

	if (p == NULL || n == 0u) {
		return 0u;
	}

	i = 0u;
	while (i < n && (p[i] == ' ' || p[i] == '\0')) {
		i++;
	}
	while (i < n) {
		unsigned char ch = (unsigned char)p[i];

		if (ch < (unsigned char)'0' || ch > (unsigned char)'7') {
			break;
		}
		u64Val = (u64Val << 3) | (uint64_t)(ch - (unsigned char)'0');
		i++;
	}
	return u64Val;
}

/* ---- public: checksum -------------------------------------------------- */

/*
 * POSIX: sum every header byte with the 8-byte chksum field replaced by
 * ASCII spaces (0x20). Compare to the octal value stored in chksum.
 * Returns 1 on match, 0 on failure / NULL.
 */
int
gj_tar_checksum_ok(const unsigned char hdr[512])
{
	unsigned uSum = 0u;
	unsigned uStored;
	size_t i;

	if (hdr == NULL) {
		return 0;
	}

	for (i = 0u; i < B101_HDR_SZ; i++) {
		if (i >= B101_OFF_CHKSUM && i < B101_OFF_CHKSUM + B101_LEN_CHKSUM) {
			uSum += (unsigned)' ';
		} else {
			uSum += (unsigned)hdr[i];
		}
	}

	uStored = (unsigned)gj_tar_octal((const char *)(hdr + B101_OFF_CHKSUM),
	                                 B101_LEN_CHKSUM);
	return uSum == uStored ? 1 : 0;
}

/* ---- public: header parse ---------------------------------------------- */

/*
 * Parse a 512-byte tar header into name / size / typeflag.
 * Requires a valid checksum. name_out capacity is 256 including NUL.
 * On ustar headers with a non-empty prefix, name is "prefix/name".
 */
int
gj_tar_header_parse(const unsigned char hdr[512], char name_out[256],
                    uint64_t *size, int *typeflag)
{
	size_t cbName;
	size_t cbPrefix;
	size_t cbOut;
	size_t i;
	int fUstar;

	if (hdr == NULL || name_out == NULL || size == NULL || typeflag == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (!gj_tar_checksum_ok(hdr)) {
		errno = EINVAL;
		return -1;
	}

	fUstar = b101_is_ustar(hdr);
	cbName = b101_field_len(hdr + B101_OFF_NAME, B101_LEN_NAME);
	cbPrefix = 0u;
	if (fUstar) {
		cbPrefix = b101_field_len(hdr + B101_OFF_PREFIX, B101_LEN_PREFIX);
	}

	/*
	 * Combined path must fit in name_out with a trailing NUL.
	 * Worst-case POSIX path is prefix(155) + '/' + name(100) = 256
	 * characters without NUL — that exceeds B101_NAME_CAP-1, so reject.
	 */
	if (cbPrefix > 0u) {
		cbOut = cbPrefix + 1u + cbName;
	} else {
		cbOut = cbName;
	}
	if (cbOut + 1u > B101_NAME_CAP) {
		errno = ENAMETOOLONG;
		return -1;
	}

	i = 0u;
	if (cbPrefix > 0u) {
		size_t j;

		for (j = 0u; j < cbPrefix; j++) {
			name_out[i++] = (char)hdr[B101_OFF_PREFIX + j];
		}
		name_out[i++] = '/';
	}
	{
		size_t j;

		for (j = 0u; j < cbName; j++) {
			name_out[i++] = (char)hdr[B101_OFF_NAME + j];
		}
	}
	name_out[i] = '\0';

	*size = gj_tar_octal((const char *)(hdr + B101_OFF_SIZE), B101_LEN_SIZE);
	*typeflag = (int)(unsigned char)hdr[B101_OFF_TYPE];
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

uint64_t __gj_tar_octal(const char *p, size_t n)
    __attribute__((alias("gj_tar_octal")));
int __gj_tar_checksum_ok(const unsigned char hdr[512])
    __attribute__((alias("gj_tar_checksum_ok")));
int __gj_tar_header_parse(const unsigned char hdr[512], char name_out[256],
                          uint64_t *size, int *typeflag)
    __attribute__((alias("gj_tar_header_parse")));
