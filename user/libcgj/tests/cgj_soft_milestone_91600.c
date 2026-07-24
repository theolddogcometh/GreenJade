/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91600 \
 *   user/libcgj/tests/cgj_soft_milestone_91600.c -ldl
 * Direct: -DCGJ_SOFT_M91600_DIRECT + graph_batch91591.c … graph_batch91600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91600
#define CGJ_SOFT_B0 91591
#define CGJ_SOFT_B1 91592
#define CGJ_SOFT_B2 91593
#define CGJ_SOFT_B3 91594
#define CGJ_SOFT_B4 91595
#define CGJ_SOFT_B5 91596
#define CGJ_SOFT_B6 91597
#define CGJ_SOFT_B7 91598
#define CGJ_SOFT_B8 91599
#include "_cgj_soft_m_template.inc"
