/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5661: Steam client bootstrap root layout code.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_client_boot_root_5661(void);
 *     - Returns the soft install-root layout tag for the Steam client
 *       bootstrap continuum (always 0x53544d31, fourCC STM1). Not a
 *       filesystem path probe.
 *   uint32_t gj_batch_id_5661(void);
 *   uint32_t __gj_steam_client_boot_root_5661 / __gj_batch_id_5661
 *   __libcgj_batch5661_marker = "libcgj-batch5661"
 *
 * Exclusive continuum CREATE-ONLY (5661-5670: Steam client bootstrap,
 * steam.sh launch codes, steamui ready integers —
 * steam_client_boot_root_5661, steam_client_boot_phase_5662,
 * steam_client_boot_ready_5663, steam_sh_launch_mode_5664,
 * steam_sh_exit_code_5665, steam_sh_ready_5666, steamui_ready_int_5667,
 * steamui_module_ready_5668, steamui_ready_score_5669,
 * batch_id_5670). Unique *_5661 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5661_marker[] = "libcgj-batch5661";

#define B5661_BATCH_ID   5661u
/* Soft "STM1" fourCC-style layout tag for Steam client root. */
#define B5661_BOOT_ROOT  0x53544d31u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5661_root(void)
{
	return B5661_BOOT_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_client_boot_root_5661 - Steam client bootstrap root layout tag.
 *
 * Always returns 0x53544d31. Soft pure-data only; does not open paths
 * or probe $STEAMROOT. Does not call libc. No parent wires.
 */
uint32_t
gj_steam_client_boot_root_5661(void)
{
	(void)NULL;
	return b5661_root();
}

uint32_t
gj_batch_id_5661(void)
{
	return B5661_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_client_boot_root_5661(void)
    __attribute__((alias("gj_steam_client_boot_root_5661")));

uint32_t __gj_batch_id_5661(void)
    __attribute__((alias("gj_batch_id_5661")));
