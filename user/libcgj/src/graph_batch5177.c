/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5177: ACPI byte-sum checksum predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_checksum_ok_u(const uint8_t *p, size_t n);
 *     - Return 1 if the 8-bit sum of n octets at p is zero (ACPI
 *       table checksum convention); else 0. NULL with n > 0 → 0;
 *       NULL with n == 0 → 1 (empty sum is zero).
 *   uint32_t __gj_acpi_checksum_ok_u  (alias)
 *   __libcgj_batch5177_marker = "libcgj-batch5177"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_checksum_ok_u surface only; no multi-def. Distinct
 * from gj_ip_checksum / gj_tar_checksum_ok. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5177_marker[] = "libcgj-batch5177";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Sum octets mod 256; ACPI requires the total (including the checksum
 * field itself) to be zero. p non-NULL when n > 0.
 */
static uint32_t
b5177_checksum_ok(const uint8_t *p, size_t n)
{
	size_t i;
	uint8_t u8Sum;

	u8Sum = 0u;
	for (i = 0u; i < n; i++) {
		u8Sum = (uint8_t)(u8Sum + p[i]);
	}
	if (u8Sum == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_checksum_ok_u - 1 if ACPI 8-bit checksum of [p, p+n) is 0.
 *
 * p: table/buffer start (NULL allowed only when n == 0)
 * n: octet count
 *
 * Soft check only; does not allocate or set errno. No parent wires.
 */
uint32_t
gj_acpi_checksum_ok_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL) {
		if (n == 0u) {
			return 1u;
		}
		return 0u;
	}
	return b5177_checksum_ok(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_checksum_ok_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_acpi_checksum_ok_u")));
