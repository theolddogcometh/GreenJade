/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5176: ACPI four-char signature ASCII probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_acpi_is_ascii_sig_u(uint32_t sig);
 *     - Return 1 if every byte of the little-endian four-char
 *       signature is an ASCII printable (0x20..0x7E); else 0.
 *   uint32_t __gj_acpi_is_ascii_sig_u  (alias)
 *   __libcgj_batch5176_marker = "libcgj-batch5176"
 *
 * Exclusive continuum CREATE-ONLY (5171-5180: ACPI/table unique).
 * Unique gj_acpi_is_ascii_sig_u surface only; no multi-def. Distinct
 * from gj_ascii_isprint_u2 (single code unit). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5176_marker[] = "libcgj-batch5176";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Probe each of the four LE bytes of u32Sig for printable ASCII.
 * Printable range is [0x20, 0x7E] inclusive (space through tilde).
 */
static uint32_t
b5176_is_ascii_sig(uint32_t u32Sig)
{
	uint32_t u32I;
	uint8_t u8B;

	for (u32I = 0u; u32I < 4u; u32I++) {
		u8B = (uint8_t)((u32Sig >> (u32I * 8u)) & 0xFFu);
		if (u8B < 0x20u || u8B > 0x7Eu) {
			return 0u;
		}
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_acpi_is_ascii_sig_u - 1 if all four signature bytes are printable.
 *
 * sig: four-char ACPI signature packed little-endian
 *
 * Soft probe only; does not validate against a known table list.
 * No parent wires.
 */
uint32_t
gj_acpi_is_ascii_sig_u(uint32_t u32Sig)
{
	(void)NULL;
	return b5176_is_ascii_sig(u32Sig);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_acpi_is_ascii_sig_u(uint32_t u32Sig)
    __attribute__((alias("gj_acpi_is_ascii_sig_u")));
