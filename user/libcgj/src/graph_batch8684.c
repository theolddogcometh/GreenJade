/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8684: CAP_SETGID capability id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_setgid_id_8684(void);
 *     - Return soft CAP_SETGID capability bit index (6).
 *   uint32_t __gj_cap_setgid_id_8684  (alias)
 *   __libcgj_batch8684_marker = "libcgj-batch8684"
 *
 * Exclusive continuum CREATE-ONLY (8681-8690: cap/prctl id stubs —
 * cap_chown_id, cap_dac_override_id, cap_setuid_id, cap_setgid_id,
 * cap_net_admin_id, prctl_ready, securebits_ok, no_new_privs_id,
 * cap_ready, batch_id_8690). Unique gj_cap_setgid_id_8684 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8684_marker[] = "libcgj-batch8684";

/* Soft CAP_SETGID capability bit index. */
#define B8684_CAP_SETGID  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8684_cap_setgid_id(void)
{
	return B8684_CAP_SETGID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_setgid_id_8684 - soft CAP_SETGID capability catalog id.
 *
 * Always returns 6. Pure-data stub for product capability catalogs;
 * does not invoke capget/capset or setgid. No parent wires.
 */
uint32_t
gj_cap_setgid_id_8684(void)
{
	(void)NULL;
	return b8684_cap_setgid_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_setgid_id_8684(void)
    __attribute__((alias("gj_cap_setgid_id_8684")));
