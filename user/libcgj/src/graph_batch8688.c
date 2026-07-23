/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8688: PR_SET_NO_NEW_PRIVS option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_no_new_privs_id_8688(void);
 *     - Return soft PR_SET_NO_NEW_PRIVS option id (38).
 *   uint32_t __gj_no_new_privs_id_8688  (alias)
 *   __libcgj_batch8688_marker = "libcgj-batch8688"
 *
 * Exclusive continuum CREATE-ONLY (8681-8690: cap/prctl id stubs —
 * cap_chown_id, cap_dac_override_id, cap_setuid_id, cap_setgid_id,
 * cap_net_admin_id, prctl_ready, securebits_ok, no_new_privs_id,
 * cap_ready, batch_id_8690). Unique gj_no_new_privs_id_8688 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8688_marker[] = "libcgj-batch8688";

/* Soft PR_SET_NO_NEW_PRIVS option catalog id. */
#define B8688_PR_SET_NO_NEW_PRIVS  38u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8688_no_new_privs_id(void)
{
	return B8688_PR_SET_NO_NEW_PRIVS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_no_new_privs_id_8688 - soft PR_SET_NO_NEW_PRIVS option catalog id.
 *
 * Always returns 38. Pure-data stub for product prctl option catalogs;
 * does not invoke prctl(2) or set no_new_privs. No parent wires.
 */
uint32_t
gj_no_new_privs_id_8688(void)
{
	(void)NULL;
	return b8688_no_new_privs_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_no_new_privs_id_8688(void)
    __attribute__((alias("gj_no_new_privs_id_8688")));
