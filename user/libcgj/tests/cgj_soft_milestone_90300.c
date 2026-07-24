/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90300 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90300.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90300 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90300 \
 *   user/libcgj/tests/cgj_soft_milestone_90300.c -ldl
 * Direct: -DCGJ_SOFT_M90300_DIRECT + graph_batch90291.c … graph_batch90300.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90300_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90300
#define CGJ_SOFT_B0 90291
#define CGJ_SOFT_B1 90292
#define CGJ_SOFT_B2 90293
#define CGJ_SOFT_B3 90294
#define CGJ_SOFT_B4 90295
#define CGJ_SOFT_B5 90296
#define CGJ_SOFT_B6 90297
#define CGJ_SOFT_B7 90298
#define CGJ_SOFT_B8 90299
#include "_cgj_soft_m_template.inc"
