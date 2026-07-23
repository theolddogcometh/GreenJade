/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5631: install-path Secure Boot required lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_secure_boot_required_5631(void);
 *     - Returns 1 (install continuum expects Secure Boot enabled on
 *       target firmware). Soft pure-data product tag only.
 *   uint32_t __gj_secure_boot_required_5631  (alias)
 *   __libcgj_batch5631_marker = "libcgj-batch5631"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_secure_boot_required_5631 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5631_marker[] = "libcgj-batch5631";

/* Install path prefers Secure Boot enabled. */
#define B5631_SB_REQUIRED  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5631_sb_required(void)
{
	return B5631_SB_REQUIRED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_secure_boot_required_5631 - Secure Boot required lamp for install.
 *
 * Always returns 1. Soft pure-data only; does not probe EFI variables
 * or firmware. Does not call libc. No parent wires.
 */
uint32_t
gj_secure_boot_required_5631(void)
{
	(void)NULL;
	return b5631_sb_required();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_secure_boot_required_5631(void)
    __attribute__((alias("gj_secure_boot_required_5631")));
