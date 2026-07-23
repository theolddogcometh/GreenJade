/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch345: DHCP options magic cookie (RFC 2131).
 *
 * Surface (unique symbols):
 *   int gj_dhcp_check_magic(const uint8_t *opts4);
 *     — Return 1 if opts4[0..3] is the DHCP magic cookie
 *       99.130.83.99 (decimal octets; wire 0x63 0x82 0x53 0x63),
 *       else 0. NULL opts4 returns 0.
 *   void gj_dhcp_write_magic(uint8_t *opts4);
 *     — Write the four magic-cookie octets at opts4. NULL is a no-op.
 *   __gj_dhcp_check_magic / __gj_dhcp_write_magic  (aliases)
 *   __libcgj_batch345_marker = "libcgj-batch345"
 *
 * RFC 2131 §3: the first four octets of the options field are the
 * magic cookie identifying the options as vendor extensions (BOOTP)
 * / DHCP. Distinct from gj_dhcp_find_option (batch155), which walks
 * the TLV list after the cookie.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch345_marker[] = "libcgj-batch345";

/* RFC 2131 magic cookie: 99, 130, 83, 99 (decimal). */
#define B345_MAGIC0  99u
#define B345_MAGIC1  130u
#define B345_MAGIC2  83u
#define B345_MAGIC3  99u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dhcp_check_magic — true if the four octets are the DHCP cookie.
 *
 * opts4: pointer to at least four readable option-field leading bytes
 *        (typically &dhcp_msg.options[0]). NULL → 0.
 *
 * Returns 1 on exact match, 0 otherwise.
 */
int
gj_dhcp_check_magic(const uint8_t *opts4)
{
	if (opts4 == NULL) {
		return 0;
	}
	if (opts4[0] != (uint8_t)B345_MAGIC0 ||
	    opts4[1] != (uint8_t)B345_MAGIC1 ||
	    opts4[2] != (uint8_t)B345_MAGIC2 ||
	    opts4[3] != (uint8_t)B345_MAGIC3) {
		return 0;
	}
	return 1;
}

/*
 * gj_dhcp_write_magic — store the RFC 2131 magic cookie at opts4[0..3].
 *
 * opts4: writable four-byte destination; NULL is a no-op.
 */
void
gj_dhcp_write_magic(uint8_t *opts4)
{
	if (opts4 == NULL) {
		return;
	}
	opts4[0] = (uint8_t)B345_MAGIC0;
	opts4[1] = (uint8_t)B345_MAGIC1;
	opts4[2] = (uint8_t)B345_MAGIC2;
	opts4[3] = (uint8_t)B345_MAGIC3;
}

int __gj_dhcp_check_magic(const uint8_t *opts4)
    __attribute__((alias("gj_dhcp_check_magic")));

void __gj_dhcp_write_magic(uint8_t *opts4)
    __attribute__((alias("gj_dhcp_write_magic")));
