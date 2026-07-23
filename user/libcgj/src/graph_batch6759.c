/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6759: Landlock rule type errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_type_errorish_u_6759(uint32_t rule_type);
 *     - Return 1 if rule_type is unknown/invalid (not 1..2), else 0.
 *   uint32_t __gj_ll_rule_type_errorish_u_6759  (alias)
 *   __libcgj_batch6759_marker = "libcgj-batch6759"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_type_errorish_u_6759 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6759_marker[] = "libcgj-batch6759";

/* Known Landlock rule types: PATH_BENEATH=1, NET_PORT=2. */
#define B6759_LL_RULE_MIN ((uint32_t)1u)
#define B6759_LL_RULE_MAX ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6759_ll_rule_type_errorish(uint32_t u32RuleType)
{
	return (u32RuleType < B6759_LL_RULE_MIN ||
	        u32RuleType > B6759_LL_RULE_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_type_errorish_u_6759 - 1 if rule_type is unknown/invalid.
 *
 * rule_type: landlock_add_rule rule_type argument
 *
 * Returns 1 when rule_type is outside the known set (1..2), including 0
 * and future/unknown values. Soft inverse of type_ok; no parent wires.
 */
uint32_t
gj_ll_rule_type_errorish_u_6759(uint32_t u32RuleType)
{
	(void)NULL;
	return b6759_ll_rule_type_errorish(u32RuleType);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_type_errorish_u_6759(uint32_t u32RuleType)
    __attribute__((alias("gj_ll_rule_type_errorish_u_6759")));
