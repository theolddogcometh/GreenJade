/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5632: Secure Boot state ok predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_secure_boot_state_ok_5632(uint32_t state);
 *     - Soft SB state codes: 0=disabled, 1=setup mode, 2=enabled.
 *       Returns 1 only when state == 2 (user mode + SB enabled).
 *   uint32_t __gj_secure_boot_state_ok_5632  (alias)
 *   __libcgj_batch5632_marker = "libcgj-batch5632"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path). Unique
 * gj_secure_boot_state_ok_5632 surface only; no multi-def. Distinct
 * from gj_secure_boot_required_5631 (batch5631). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5632_marker[] = "libcgj-batch5632";

/* Soft state: user mode with Secure Boot active. */
#define B5632_STATE_ENABLED  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5632_state_ok(uint32_t u32State)
{
	return (u32State == B5632_STATE_ENABLED) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_secure_boot_state_ok_5632 - true when firmware SB state is enabled.
 *
 * state: soft enum (0 disabled / 1 setup / 2 enabled)
 *
 * Returns 1 for enabled only, else 0. Pure compare; no EFI I/O.
 * No parent wires.
 */
uint32_t
gj_secure_boot_state_ok_5632(uint32_t u32State)
{
	(void)NULL;
	return b5632_state_ok(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_secure_boot_state_ok_5632(uint32_t u32State)
    __attribute__((alias("gj_secure_boot_state_ok_5632")));
