/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8558: regex character-class kind ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_re_class_ok_u_8558(uint32_t kind);
 *     - Return 1 if kind is a known soft character-class token id
 *       (open '[', close ']', negate '^', range '-'), else 0. Catalog
 *       validator only; not a class parser or matcher.
 *   uint32_t __gj_re_class_ok_u_8558  (alias)
 *   __libcgj_batch8558_marker = "libcgj-batch8558"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Unique
 * gj_re_class_ok_u_8558 surface only; no multi-def. No parent wires.
 * No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8558_marker[] = "libcgj-batch8558";

/* Soft character-class token catalog ids (ASCII delimiters / ops). */
#define B8558_CLASS_OPEN  ((uint32_t)0x5Bu) /* '[' */
#define B8558_CLASS_CLOSE ((uint32_t)0x5Du) /* ']' */
#define B8558_CLASS_NEG   ((uint32_t)0x5Eu) /* '^' */
#define B8558_CLASS_RANGE ((uint32_t)0x2Du) /* '-' */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8558_class_ok(uint32_t u32Kind)
{
	if (u32Kind == B8558_CLASS_OPEN) {
		return 1u;
	}
	if (u32Kind == B8558_CLASS_CLOSE) {
		return 1u;
	}
	if (u32Kind == B8558_CLASS_NEG) {
		return 1u;
	}
	if (u32Kind == B8558_CLASS_RANGE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_re_class_ok_u_8558 - 1 if kind is a known class token id.
 *
 * kind: soft character-class token under test
 *
 * Returns 1 for '[', ']', '^', or '-', else 0. Soft class-token catalog
 * validator; does not parse bracket expressions. No parent wires.
 */
uint32_t
gj_re_class_ok_u_8558(uint32_t u32Kind)
{
	(void)NULL;
	return b8558_class_ok(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_re_class_ok_u_8558(uint32_t u32Kind)
    __attribute__((alias("gj_re_class_ok_u_8558")));
