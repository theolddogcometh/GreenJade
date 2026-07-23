/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch155: DHCP option scan (RFC 2132).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party DHCP source was copied.
 *
 * Surface (unique symbols):
 *   int gj_dhcp_find_option(const unsigned char *opts, size_t len,
 *                           unsigned code, const unsigned char **val,
 *                           size_t *vlen);
 *     Walk opts[0..len) as an RFC 2132 option list (after the magic
 *     cookie). On match: *val → option value, *vlen = length, return 0.
 *     Returns -1 if not found, truncated TLV, NULL args, or code is
 *     Pad (0) / End (255) (those have no value payload).
 *   int gj_dhcp_msg_type(const unsigned char *opts, size_t len,
 *                        unsigned *type);
 *     Locate option 53 (DHCP Message Type). Requires length == 1.
 *     On success *type = the single value octet; return 0. Else -1.
 *   __gj_dhcp_find_option / __gj_dhcp_msg_type  (aliases)
 *   __libcgj_batch155_marker = "libcgj-batch155"
 *
 * RFC 2132 encoding (options field):
 *   Pad  (code 0)   : single octet, no length
 *   End  (code 255) : single octet, no length — stop scan
 *   other           : code (1) + len (1) + value[len]
 *
 * Common codes (informative; scanner is code-agnostic):
 *   53 = DHCP Message Type (1 octet: 1 DISCOVER … 8 INFORM)
 *   50 = Requested IP Address, 51 = Lease Time, 54 = Server ID, …
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch155_marker[] = "libcgj-batch155";

/* RFC 2132 special single-octet options. */
#define B155_OPT_PAD  0u
#define B155_OPT_END  255u
/* DHCP Message Type (RFC 2132 §9.6). */
#define B155_OPT_MSG  53u

/*
 * gj_dhcp_find_option — linear scan of a DHCP options blob for code.
 * Stops at End (255). Skips Pad (0). Rejects truncated length/value.
 */
int
gj_dhcp_find_option(const unsigned char *opts, size_t len, unsigned code,
                    const unsigned char **val, size_t *vlen)
{
	size_t i;

	if (opts == NULL || val == NULL || vlen == NULL) {
		return -1;
	}
	/* Pad/End have no value field — not searchable as TLV. */
	if (code == B155_OPT_PAD || code == B155_OPT_END || code > 255u) {
		return -1;
	}

	i = 0u;
	while (i < len) {
		unsigned c = (unsigned)opts[i];

		if (c == B155_OPT_END) {
			break;
		}
		if (c == B155_OPT_PAD) {
			i++;
			continue;
		}

		/* Need code + length octet. */
		if (i + 1u >= len) {
			return -1;
		}
		{
			unsigned oLen = (unsigned)opts[i + 1u];
			size_t need = 2u + (size_t)oLen;

			if (i + need > len) {
				return -1; /* truncated value */
			}
			if (c == code) {
				*val = opts + i + 2u;
				*vlen = (size_t)oLen;
				return 0;
			}
			i += need;
		}
	}

	return -1; /* not found */
}

int __gj_dhcp_find_option(const unsigned char *opts, size_t len,
                          unsigned code, const unsigned char **val,
                          size_t *vlen)
    __attribute__((alias("gj_dhcp_find_option")));

/*
 * gj_dhcp_msg_type — extract DHCP Message Type (option 53, length 1).
 */
int
gj_dhcp_msg_type(const unsigned char *opts, size_t len, unsigned *type)
{
	const unsigned char *pVal = NULL;
	size_t cbVal = 0u;

	if (type == NULL) {
		return -1;
	}
	if (gj_dhcp_find_option(opts, len, B155_OPT_MSG, &pVal, &cbVal) != 0) {
		return -1;
	}
	if (cbVal != 1u || pVal == NULL) {
		return -1;
	}
	*type = (unsigned)pVal[0];
	return 0;
}

int __gj_dhcp_msg_type(const unsigned char *opts, size_t len, unsigned *type)
    __attribute__((alias("gj_dhcp_msg_type")));
