/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1969: Vulkan fragment shader stage bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_shader_stage_frag_p(uint32_t stages);
 *     — Return 1 if VK_SHADER_STAGE_FRAGMENT_BIT (0x10) is set in stages,
 *       else 0. Pure stage-mask bit predicate; does not inspect modules.
 *   int __gj_shader_stage_frag_p  (alias)
 *   __libcgj_batch1969_marker = "libcgj-batch1969"
 *
 * Vulkan/GL enum exclusive pure-data wave (1961–1970). Distinct from
 * gj_shader_stage_vert_p — unique name. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1969_marker[] = "libcgj-batch1969";

/* VK_SHADER_STAGE_FRAGMENT_BIT */
#define B1969_STAGE_FRAG  0x00000010u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if fragment stage bit is set. */
static int
b1969_frag_p(uint32_t uStages)
{
	return ((uStages & B1969_STAGE_FRAG) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shader_stage_frag_p — 1 if FRAGMENT stage bit is set.
 *
 * stages: VkShaderStageFlags as uint32_t
 * Returns 1 when bit 4 is set, else 0.
 */
int
gj_shader_stage_frag_p(uint32_t uStages)
{
	(void)NULL;
	return b1969_frag_p(uStages);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shader_stage_frag_p(uint32_t uStages)
    __attribute__((alias("gj_shader_stage_frag_p")));
