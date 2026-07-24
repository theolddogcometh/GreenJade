/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77600 \
 *   user/libcgj/tests/cgj_soft_milestone_77600.c -ldl
 * Direct: -DCGJ_SOFT_M77600_DIRECT + graph_batch77591.c … graph_batch77600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77600
#define CGJ_SOFT_B0 77591
#define CGJ_SOFT_B1 77592
#define CGJ_SOFT_B2 77593
#define CGJ_SOFT_B3 77594
#define CGJ_SOFT_B4 77595
#define CGJ_SOFT_B5 77596
#define CGJ_SOFT_B6 77597
#define CGJ_SOFT_B7 77598
#define CGJ_SOFT_B8 77599
#include "_cgj_soft_m_template.inc"
