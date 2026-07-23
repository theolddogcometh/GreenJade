/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6754: Landlock PATH_BENEATH rule type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_type_path_id_6754(void);
 *     - Return LANDLOCK_RULE_PATH_BENEATH constant (1).
 *   uint32_t __gj_ll_rule_type_path_id_6754  (alias)
 *   __libcgj_batch6754_marker = "libcgj-batch6754"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_type_path_id_6754 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6754_marker[] = "libcgj-batch6754";

/* LANDLOCK_RULE_PATH_BENEATH = 1 */
#define B6754_LL_RULE_PATH_BENEATH ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6754_ll_rule_type_path_id(void)
{
	return B6754_LL_RULE_PATH_BENEATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_type_path_id_6754 - compile-time PATH_BENEATH rule type id.
 *
 * Always returns 1 (LANDLOCK_RULE_PATH_BENEATH). Soft UAPI constant
 * export for freestanding graph probes. No parent wires.
 */
uint32_t
gj_ll_rule_type_path_id_6754(void)
{
	(void)NULL;
	return b6754_ll_rule_type_path_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_type_path_id_6754(void)
    __attribute__((alias("gj_ll_rule_type_path_id_6754")));
