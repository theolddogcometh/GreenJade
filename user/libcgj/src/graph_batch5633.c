/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5633: TPM present ok predicate (install path).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tpm_present_ok_5633(uint32_t present);
 *     - Return 1 when present != 0 (TPM device reported present by
 *       probe path), else 0. Soft gate only.
 *   uint32_t __gj_tpm_present_ok_5633  (alias)
 *   __libcgj_batch5633_marker = "libcgj-batch5633"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_tpm_present_ok_5633 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5633_marker[] = "libcgj-batch5633";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5633_present_ok(uint32_t u32Present)
{
	return (u32Present != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tpm_present_ok_5633 - true when a TPM is reported present.
 *
 * present: non-zero if probe/sysfs path indicated a TPM device
 *
 * Returns 1 when present != 0, else 0. Does not open devices.
 * No parent wires.
 */
uint32_t
gj_tpm_present_ok_5633(uint32_t u32Present)
{
	(void)NULL;
	return b5633_present_ok(u32Present);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tpm_present_ok_5633(uint32_t u32Present)
    __attribute__((alias("gj_tpm_present_ok_5633")));
