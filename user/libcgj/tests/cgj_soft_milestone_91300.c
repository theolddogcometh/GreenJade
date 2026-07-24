/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91300 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91300.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91300 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91300 \
 *   user/libcgj/tests/cgj_soft_milestone_91300.c -ldl
 * Direct: -DCGJ_SOFT_M91300_DIRECT + graph_batch91291.c … graph_batch91300.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91300_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91300
#define CGJ_SOFT_B0 91291
#define CGJ_SOFT_B1 91292
#define CGJ_SOFT_B2 91293
#define CGJ_SOFT_B3 91294
#define CGJ_SOFT_B4 91295
#define CGJ_SOFT_B5 91296
#define CGJ_SOFT_B6 91297
#define CGJ_SOFT_B7 91298
#define CGJ_SOFT_B8 91299
#include "_cgj_soft_m_template.inc"
