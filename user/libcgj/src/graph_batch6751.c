/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6751: Landlock LANDLOCK_RULE_PATH_BENEATH type stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_path_beneath_u_6751(uint32_t rule_type);
 *     - Return 1 if rule_type is LANDLOCK_RULE_PATH_BENEATH (1), else 0.
 *   uint32_t __gj_ll_rule_path_beneath_u_6751  (alias)
 *   __libcgj_batch6751_marker = "libcgj-batch6751"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_path_beneath_u_6751 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6751_marker[] = "libcgj-batch6751";

/* LANDLOCK_RULE_PATH_BENEATH: landlock_path_beneath_attr rule (value 1). */
#define B6751_LL_RULE_PATH_BENEATH ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6751_ll_rule_path_beneath(uint32_t u32RuleType)
{
	return (u32RuleType == B6751_LL_RULE_PATH_BENEATH) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_path_beneath_u_6751 - 1 if rule_type is PATH_BENEATH.
 *
 * rule_type: landlock_add_rule rule_type argument
 *
 * Returns 1 when rule_type equals LANDLOCK_RULE_PATH_BENEATH (1), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_ll_rule_path_beneath_u_6751(uint32_t u32RuleType)
{
	(void)NULL;
	return b6751_ll_rule_path_beneath(u32RuleType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_path_beneath_u_6751(uint32_t u32RuleType)
    __attribute__((alias("gj_ll_rule_path_beneath_u_6751")));
