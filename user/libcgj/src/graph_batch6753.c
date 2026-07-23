/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6753: Landlock rule type validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_type_ok_u_6753(uint32_t rule_type);
 *     - Return 1 if rule_type is a known Landlock rule type (1..2), else 0.
 *   uint32_t __gj_ll_rule_type_ok_u_6753  (alias)
 *   __libcgj_batch6753_marker = "libcgj-batch6753"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_type_ok_u_6753 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6753_marker[] = "libcgj-batch6753";

/* Known Landlock rule types: PATH_BENEATH=1, NET_PORT=2. */
#define B6753_LL_RULE_MIN ((uint32_t)1u)
#define B6753_LL_RULE_MAX ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6753_ll_rule_type_ok(uint32_t u32RuleType)
{
	return (u32RuleType >= B6753_LL_RULE_MIN &&
	        u32RuleType <= B6753_LL_RULE_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_type_ok_u_6753 - 1 if rule_type is a known Landlock type.
 *
 * rule_type: landlock_add_rule rule_type argument
 *
 * Returns 1 for PATH_BENEATH (1) or NET_PORT (2), else 0.
 * Soft range check; no parent wires.
 */
uint32_t
gj_ll_rule_type_ok_u_6753(uint32_t u32RuleType)
{
	(void)NULL;
	return b6753_ll_rule_type_ok(u32RuleType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_type_ok_u_6753(uint32_t u32RuleType)
    __attribute__((alias("gj_ll_rule_type_ok_u_6753")));
