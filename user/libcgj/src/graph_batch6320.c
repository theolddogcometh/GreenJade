/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6320: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6320(void);
 *     - Returns the compile-time graph batch number for this TU (6320).
 *   uint32_t __gj_batch_id_6320  (alias)
 *   __libcgj_batch6320_marker = "libcgj-batch6320"
 *
 * Shader cache path stubs exclusive CREATE-ONLY (6311-6320:
 * path_has_shadercache_6311, path_has_mesa_shader_cache_6312,
 * path_has_mesa_shader_cache_db_6313, path_has_fossilize_6314,
 * path_has_foz_6315, path_has_dxvk_cache_6316,
 * path_has_pipeline_cache_6317, path_has_shader_cache_6318,
 * path_has_glcache_6319, batch_id_6320). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6320_marker[] = "libcgj-batch6320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6320_id(void)
{
	return 6320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6320 - report this TU's graph batch number.
 *
 * Always returns 6320. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6320(void)
{
	(void)NULL;
	return b6320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6320(void)
    __attribute__((alias("gj_batch_id_6320")));
