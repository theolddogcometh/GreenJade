/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79600 \
 *   user/libcgj/tests/cgj_soft_milestone_79600.c -ldl
 * Direct: -DCGJ_SOFT_M79600_DIRECT + graph_batch79591.c … graph_batch79600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79600
#define CGJ_SOFT_B0 79591
#define CGJ_SOFT_B1 79592
#define CGJ_SOFT_B2 79593
#define CGJ_SOFT_B3 79594
#define CGJ_SOFT_B4 79595
#define CGJ_SOFT_B5 79596
#define CGJ_SOFT_B6 79597
#define CGJ_SOFT_B7 79598
#define CGJ_SOFT_B8 79599
#include "_cgj_soft_m_template.inc"
