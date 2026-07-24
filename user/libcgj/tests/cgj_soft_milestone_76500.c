/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76500 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_76500.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76500 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76500 \
 *   user/libcgj/tests/cgj_soft_milestone_76500.c -ldl
 * Direct: -DCGJ_SOFT_M76500_DIRECT + graph_batch76491.c … graph_batch76500.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M76500_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 76500
#define CGJ_SOFT_B0 76491
#define CGJ_SOFT_B1 76492
#define CGJ_SOFT_B2 76493
#define CGJ_SOFT_B3 76494
#define CGJ_SOFT_B4 76495
#define CGJ_SOFT_B5 76496
#define CGJ_SOFT_B6 76497
#define CGJ_SOFT_B7 76498
#define CGJ_SOFT_B8 76499
#include "_cgj_soft_m_template.inc"
