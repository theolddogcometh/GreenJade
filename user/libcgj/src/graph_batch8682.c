/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8682: CAP_DAC_OVERRIDE capability id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_dac_override_id_8682(void);
 *     - Return soft CAP_DAC_OVERRIDE capability bit index (1).
 *   uint32_t __gj_cap_dac_override_id_8682  (alias)
 *   __libcgj_batch8682_marker = "libcgj-batch8682"
 *
 * Exclusive continuum CREATE-ONLY (8681-8690: cap/prctl id stubs —
 * cap_chown_id, cap_dac_override_id, cap_setuid_id, cap_setgid_id,
 * cap_net_admin_id, prctl_ready, securebits_ok, no_new_privs_id,
 * cap_ready, batch_id_8690). Unique gj_cap_dac_override_id_8682
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8682_marker[] = "libcgj-batch8682";

/* Soft CAP_DAC_OVERRIDE capability bit index. */
#define B8682_CAP_DAC_OVERRIDE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8682_cap_dac_override_id(void)
{
	return B8682_CAP_DAC_OVERRIDE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_dac_override_id_8682 - soft CAP_DAC_OVERRIDE capability catalog id.
 *
 * Always returns 1. Pure-data stub for product capability catalogs;
 * does not invoke capget/capset. No parent wires.
 */
uint32_t
gj_cap_dac_override_id_8682(void)
{
	(void)NULL;
	return b8682_cap_dac_override_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_dac_override_id_8682(void)
    __attribute__((alias("gj_cap_dac_override_id_8682")));
