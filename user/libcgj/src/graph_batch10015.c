/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10015: soft Steam Runtime ok unit (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamrt_ok_u_10015(void);
 *     - Returns 0 (Steam Runtime soft ok not complete for this
 *       bar3-related continuum; still open). Soft pure-data readiness
 *       lamp; does not probe steam-runtime / scout / soldier / sniper.
 *   uint32_t __gj_steamrt_ok_u_10015  (alias)
 *   __libcgj_batch10015_marker = "libcgj-batch10015"
 *
 * Exclusive continuum CREATE-ONLY (10011-10020: proton/dxvk soft id
 * stubs — proton_ge_ok_u_10011, dxvk_ok_u_10012, vkd3d_ok_u_10013,
 * wine_prefix_ok_u_10014, steamrt_ok_u_10015, gamescope_ok_u_10016,
 * mangohud_ok_u_10017, proton_launch_ok_u_10018,
 * proton_soft_ready_u_10019, batch_id_10020). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10015_marker[] = "libcgj-batch10015";

/* Steam Runtime soft-ok lamp for continuum 10011-10020 (still open). */
#define B10015_STEAMRT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10015_ok(void)
{
	return B10015_STEAMRT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamrt_ok_u_10015 - report Steam Runtime soft-ok lamp (open).
 *
 * Always returns 0 (not complete / still open). Soft pure-data only;
 * does not probe steamrt paths or call libc. No parent wires.
 */
uint32_t
gj_steamrt_ok_u_10015(void)
{
	(void)NULL;
	return b10015_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamrt_ok_u_10015(void)
    __attribute__((alias("gj_steamrt_ok_u_10015")));
