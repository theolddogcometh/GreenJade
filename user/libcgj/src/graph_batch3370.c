/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3370: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3370(void);
 *     - Returns the compile-time graph batch number for this TU (3370).
 *   uint32_t __gj_batch_id_3370  (alias)
 *   __libcgj_batch3370_marker = "libcgj-batch3370"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370): json_is_object_start_u,
 * json_is_array_start_u, json_is_string_start_u, json_is_number_start_u,
 * toml_is_table_u, toml_is_array_table_u, ini_is_keyval_u,
 * csv_field_count_u, csv_skip_field_u, batch_id_3370. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3370_marker[] = "libcgj-batch3370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3370_id(void)
{
	return 3370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3370 - report this TU's graph batch number.
 *
 * Always returns 3370.
 */
uint32_t
gj_batch_id_3370(void)
{
	(void)NULL;
	return b3370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3370(void)
    __attribute__((alias("gj_batch_id_3370")));
