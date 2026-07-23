/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8687: securebits catalog ok tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_securebits_ok_u_8687(void);
 *     - Return 1 (securebits catalog soft-ok).
 *   uint32_t __gj_securebits_ok_u_8687  (alias)
 *   __libcgj_batch8687_marker = "libcgj-batch8687"
 *
 * Exclusive continuum CREATE-ONLY (8681-8690: cap/prctl id stubs —
 * cap_chown_id, cap_dac_override_id, cap_setuid_id, cap_setgid_id,
 * cap_net_admin_id, prctl_ready, securebits_ok, no_new_privs_id,
 * cap_ready, batch_id_8690). Unique gj_securebits_ok_u_8687 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8687_marker[] = "libcgj-batch8687";

/* Soft securebits catalog ok lamp. */
#define B8687_SECUREBITS_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8687_securebits_ok(void)
{
	return B8687_SECUREBITS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_securebits_ok_u_8687 - soft securebits catalog ok tag.
 *
 * Always returns 1 (securebits path soft-ok). Does not read or write
 * process securebits via prctl. No parent wires.
 */
uint32_t
gj_securebits_ok_u_8687(void)
{
	(void)NULL;
	return b8687_securebits_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_securebits_ok_u_8687(void)
    __attribute__((alias("gj_securebits_ok_u_8687")));
