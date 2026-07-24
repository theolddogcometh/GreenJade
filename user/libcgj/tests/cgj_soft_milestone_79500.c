/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79500 \
 *   user/libcgj/tests/cgj_soft_milestone_79500.c -ldl
 * Direct: -DCGJ_SOFT_M79500_DIRECT + graph_batch79491.c … graph_batch79500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79500
#define CGJ_SOFT_B0 79491
#define CGJ_SOFT_B1 79492
#define CGJ_SOFT_B2 79493
#define CGJ_SOFT_B3 79494
#define CGJ_SOFT_B4 79495
#define CGJ_SOFT_B5 79496
#define CGJ_SOFT_B6 79497
#define CGJ_SOFT_B7 79498
#define CGJ_SOFT_B8 79499
#include "_cgj_soft_m_template.inc"
