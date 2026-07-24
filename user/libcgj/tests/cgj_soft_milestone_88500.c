/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88500 \
 *   user/libcgj/tests/cgj_soft_milestone_88500.c -ldl
 * Direct: -DCGJ_SOFT_M88500_DIRECT + graph_batch88491.c … graph_batch88500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88500
#define CGJ_SOFT_B0 88491
#define CGJ_SOFT_B1 88492
#define CGJ_SOFT_B2 88493
#define CGJ_SOFT_B3 88494
#define CGJ_SOFT_B4 88495
#define CGJ_SOFT_B5 88496
#define CGJ_SOFT_B6 88497
#define CGJ_SOFT_B7 88498
#define CGJ_SOFT_B8 88499
#include "_cgj_soft_m_template.inc"
