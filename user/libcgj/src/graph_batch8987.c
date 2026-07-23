/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8987: soft Proton readiness unit (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ready_u_8987(void);
 *     - Returns 0 (Proton runtime readiness not complete for this
 *       bar3-related continuum; still open). Soft pure-data readiness
 *       lamp; does not probe Steam compat tools or Proton installs.
 *   uint32_t __gj_proton_ready_u_8987  (alias)
 *   __libcgj_batch8987_marker = "libcgj-batch8987"
 *
 * Exclusive continuum CREATE-ONLY (8981-8990: proton/wine soft id
 * stubs — proton_version_tag_8981, wine_prefix_tag_8982,
 * wine_dll_ok_u_8983, wine_wow64_id_8984, dxvk_ready_u_8985,
 * vkd3d_ready_u_8986, proton_ready_u_8987, wine_ready_u_8988,
 * proton_soft_ready_u_8989, batch_id_8990). Unique surface only; no
 * multi-def. Distinct from gj_proton_abi_ok_u (batch5355) and
 * gj_proton_soft_ready_u_8989. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8987_marker[] = "libcgj-batch8987";

/* Proton readiness lamp for continuum 8981-8990 (still open). */
#define B8987_PROTON_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8987_ready(void)
{
	return B8987_PROTON_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ready_u_8987 - report Proton readiness lamp (open).
 *
 * Always returns 0 (not complete / still open). Soft pure-data only;
 * does not probe Proton installs or call libc. No parent wires.
 */
uint32_t
gj_proton_ready_u_8987(void)
{
	(void)NULL;
	return b8987_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ready_u_8987(void)
    __attribute__((alias("gj_proton_ready_u_8987")));
