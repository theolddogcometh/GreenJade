/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88300 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88300.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88300 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88300 \
 *   user/libcgj/tests/cgj_soft_milestone_88300.c -ldl
 * Direct: -DCGJ_SOFT_M88300_DIRECT + graph_batch88291.c … graph_batch88300.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88300_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88300
#define CGJ_SOFT_B0 88291
#define CGJ_SOFT_B1 88292
#define CGJ_SOFT_B2 88293
#define CGJ_SOFT_B3 88294
#define CGJ_SOFT_B4 88295
#define CGJ_SOFT_B5 88296
#define CGJ_SOFT_B6 88297
#define CGJ_SOFT_B7 88298
#define CGJ_SOFT_B8 88299
#include "_cgj_soft_m_template.inc"
