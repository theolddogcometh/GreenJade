/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79400 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79400.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79400 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79400 \
 *   user/libcgj/tests/cgj_soft_milestone_79400.c -ldl
 * Direct: -DCGJ_SOFT_M79400_DIRECT + graph_batch79391.c … graph_batch79400.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79400_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79400
#define CGJ_SOFT_B0 79391
#define CGJ_SOFT_B1 79392
#define CGJ_SOFT_B2 79393
#define CGJ_SOFT_B3 79394
#define CGJ_SOFT_B4 79395
#define CGJ_SOFT_B5 79396
#define CGJ_SOFT_B6 79397
#define CGJ_SOFT_B7 79398
#define CGJ_SOFT_B8 79399
#include "_cgj_soft_m_template.inc"
