/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14120: Proton/Wine soft path ready tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_soft_path_ready_14120(uint32_t flags);
 *     - Returns 1 if soft path pipeline ready bit is set in flags
 *       (bit0), else 0. Product presence tag for Proton/Wine soft path
 *       stub readiness (wave closer).
 *   uint32_t __gj_pw_soft_path_ready_14120  (alias)
 *   __libcgj_batch14120_marker = "libcgj-batch14120"
 *
 * Exclusive continuum CREATE-ONLY (14111-14120: proton/wine soft path —
 * soft_path_max_14111, soft_path_len_ok_14112, soft_path_min_ok_14113,
 * wine_bottle_path_len_ok_14114, proton_files_path_len_ok_14115,
 * soft_path_depth_ok_14116, dosdevices_path_len_ok_14117,
 * drive_c_path_len_ok_14118, soft_path_hash_mix_14119,
 * soft_path_ready_14120). Unique surface only; no multi-def. Distinct
 * from proton_rt_path_hash_ready_5979. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14120_marker[] = "libcgj-batch14120";

/* Bit0: soft-path pipeline ready. */
#define B14120_READY_BIT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14120_soft_path_ready(uint32_t u32Flags)
{
	return ((u32Flags & B14120_READY_BIT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_soft_path_ready_14120 - soft Proton/Wine path ready product tag.
 *
 * flags: bit0 set means soft-path stubs are considered ready.
 * Returns 1 or 0. No parent wires.
 */
uint32_t
gj_pw_soft_path_ready_14120(uint32_t u32Flags)
{
	(void)NULL;
	return b14120_soft_path_ready(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_soft_path_ready_14120(uint32_t u32Flags)
    __attribute__((alias("gj_pw_soft_path_ready_14120")));
