/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5665: steam.sh launch exit / status codes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_sh_exit_code_5665(uint32_t status_id);
 *     - Map a steam.sh launch status slot (0..7) to a soft exit code
 *       tag. Unknown status_id → 0xFFFFFFFFu.
 *   uint32_t gj_steam_sh_exit_ok_5665(void);
 *     - Success exit tag (always 0).
 *   uint32_t gj_batch_id_5665(void);
 *   uint32_t __gj_steam_sh_exit_code_5665 / __gj_steam_sh_exit_ok_5665
 *   uint32_t __gj_batch_id_5665  (aliases)
 *   __libcgj_batch5665_marker = "libcgj-batch5665"
 *
 * Exclusive continuum CREATE-ONLY (5661-5670: Steam client bootstrap,
 * steam.sh launch codes, steamui ready integers). Unique *_5665
 * surfaces only; no multi-def. Distinct from
 * gj_steam_sh_launch_mode_5664. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5665_marker[] = "libcgj-batch5665";

#define B5665_BATCH_ID    5665u
#define B5665_STATUS_MAX  8u
#define B5665_EXIT_OK     0u
#define B5665_EXIT_UNK    0xFFFFFFFFu

/*
 * Soft steam.sh exit / status tags (not live waitpid status):
 *   0 ok, 1 usage, 2 missing runtime, 3 missing bin, 4 already running,
 *   5 update required, 6 bootstrap fail, 7 internal.
 */
static const uint8_t s_b5665_exit[B5665_STATUS_MAX] = {
	0u,  /* 0 OK */
	1u,  /* 1 USAGE */
	2u,  /* 2 NO_RT */
	3u,  /* 3 NO_BIN */
	4u,  /* 4 RUNNING */
	5u,  /* 5 UPDATE */
	6u,  /* 6 BOOT_FAIL */
	7u   /* 7 INTERNAL */
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5665_exit(uint32_t u32Status)
{
	if (u32Status >= B5665_STATUS_MAX) {
		return B5665_EXIT_UNK;
	}
	return (uint32_t)s_b5665_exit[u32Status];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_sh_exit_code_5665 - map steam.sh status slot to exit tag.
 *
 * status_id: 0..7 known; else 0xFFFFFFFF. Soft table only.
 */
uint32_t
gj_steam_sh_exit_code_5665(uint32_t status_id)
{
	(void)NULL;
	return b5665_exit(status_id);
}

uint32_t
gj_steam_sh_exit_ok_5665(void)
{
	return B5665_EXIT_OK;
}

uint32_t
gj_batch_id_5665(void)
{
	return B5665_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_steam_sh_exit_code_5665(uint32_t status_id)
    __attribute__((alias("gj_steam_sh_exit_code_5665")));

uint32_t __gj_steam_sh_exit_ok_5665(void)
    __attribute__((alias("gj_steam_sh_exit_ok_5665")));

uint32_t __gj_batch_id_5665(void)
    __attribute__((alias("gj_batch_id_5665")));
