/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78500 \
 *   user/libcgj/tests/cgj_soft_milestone_78500.c -ldl
 * Direct: -DCGJ_SOFT_M78500_DIRECT + graph_batch78491.c … graph_batch78500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78500
#define CGJ_SOFT_B0 78491
#define CGJ_SOFT_B1 78492
#define CGJ_SOFT_B2 78493
#define CGJ_SOFT_B3 78494
#define CGJ_SOFT_B4 78495
#define CGJ_SOFT_B5 78496
#define CGJ_SOFT_B6 78497
#define CGJ_SOFT_B7 78498
#define CGJ_SOFT_B8 78499
#include "_cgj_soft_m_template.inc"
