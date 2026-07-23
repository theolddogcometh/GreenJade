/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8683: CAP_SETUID capability id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_setuid_id_8683(void);
 *     - Return soft CAP_SETUID capability bit index (7).
 *   uint32_t __gj_cap_setuid_id_8683  (alias)
 *   __libcgj_batch8683_marker = "libcgj-batch8683"
 *
 * Exclusive continuum CREATE-ONLY (8681-8690: cap/prctl id stubs —
 * cap_chown_id, cap_dac_override_id, cap_setuid_id, cap_setgid_id,
 * cap_net_admin_id, prctl_ready, securebits_ok, no_new_privs_id,
 * cap_ready, batch_id_8690). Unique gj_cap_setuid_id_8683 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8683_marker[] = "libcgj-batch8683";

/* Soft CAP_SETUID capability bit index. */
#define B8683_CAP_SETUID  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8683_cap_setuid_id(void)
{
	return B8683_CAP_SETUID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_setuid_id_8683 - soft CAP_SETUID capability catalog id.
 *
 * Always returns 7. Pure-data stub for product capability catalogs;
 * does not invoke capget/capset or setuid. No parent wires.
 */
uint32_t
gj_cap_setuid_id_8683(void)
{
	(void)NULL;
	return b8683_cap_setuid_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_setuid_id_8683(void)
    __attribute__((alias("gj_cap_setuid_id_8683")));
