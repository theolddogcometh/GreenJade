/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88700 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88700.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88700 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88700 \
 *   user/libcgj/tests/cgj_soft_milestone_88700.c -ldl
 * Direct: -DCGJ_SOFT_M88700_DIRECT + graph_batch88691.c … graph_batch88700.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88700_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88700
#define CGJ_SOFT_B0 88691
#define CGJ_SOFT_B1 88692
#define CGJ_SOFT_B2 88693
#define CGJ_SOFT_B3 88694
#define CGJ_SOFT_B4 88695
#define CGJ_SOFT_B5 88696
#define CGJ_SOFT_B6 88697
#define CGJ_SOFT_B7 88698
#define CGJ_SOFT_B8 88699
#include "_cgj_soft_m_template.inc"
