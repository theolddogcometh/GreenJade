/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91500 \
 *   user/libcgj/tests/cgj_soft_milestone_91500.c -ldl
 * Direct: -DCGJ_SOFT_M91500_DIRECT + graph_batch91491.c … graph_batch91500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91500
#define CGJ_SOFT_B0 91491
#define CGJ_SOFT_B1 91492
#define CGJ_SOFT_B2 91493
#define CGJ_SOFT_B3 91494
#define CGJ_SOFT_B4 91495
#define CGJ_SOFT_B5 91496
#define CGJ_SOFT_B6 91497
#define CGJ_SOFT_B7 91498
#define CGJ_SOFT_B8 91499
#include "_cgj_soft_m_template.inc"
