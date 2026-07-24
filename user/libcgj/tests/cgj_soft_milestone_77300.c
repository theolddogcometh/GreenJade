/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77300 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77300.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77300 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77300 \
 *   user/libcgj/tests/cgj_soft_milestone_77300.c -ldl
 * Direct: -DCGJ_SOFT_M77300_DIRECT + graph_batch77291.c … graph_batch77300.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77300_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77300
#define CGJ_SOFT_B0 77291
#define CGJ_SOFT_B1 77292
#define CGJ_SOFT_B2 77293
#define CGJ_SOFT_B3 77294
#define CGJ_SOFT_B4 77295
#define CGJ_SOFT_B5 77296
#define CGJ_SOFT_B6 77297
#define CGJ_SOFT_B7 77298
#define CGJ_SOFT_B8 77299
#include "_cgj_soft_m_template.inc"
