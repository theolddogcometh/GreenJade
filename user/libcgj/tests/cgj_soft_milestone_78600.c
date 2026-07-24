/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78600 \
 *   user/libcgj/tests/cgj_soft_milestone_78600.c -ldl
 * Direct: -DCGJ_SOFT_M78600_DIRECT + graph_batch78591.c … graph_batch78600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78600
#define CGJ_SOFT_B0 78591
#define CGJ_SOFT_B1 78592
#define CGJ_SOFT_B2 78593
#define CGJ_SOFT_B3 78594
#define CGJ_SOFT_B4 78595
#define CGJ_SOFT_B5 78596
#define CGJ_SOFT_B6 78597
#define CGJ_SOFT_B7 78598
#define CGJ_SOFT_B8 78599
#include "_cgj_soft_m_template.inc"
