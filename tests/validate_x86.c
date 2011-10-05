/*
 * validate_x86.c - validate event tables + encodings
 *
 * Copyright (c) 2010 Google, Inc
 * Contributed by Stephane Eranian <eranian@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of libpfm, a performance monitoring support library for
 * applications on Linux.
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <err.h>

#include <perfmon/pfmlib.h>

#define MAX_ENCODING	8

typedef struct {
	const char *name;
	const char *fstr;
	uint64_t codes[MAX_ENCODING];
	int ret, count;
} test_event_t;

static const test_event_t x86_test_events[]={
	{ .name = "core::INST_RETIRED:ANY_P",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5300c0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:ANY_P",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5300c0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:DEAD",
	  .ret  = PFM_ERR_ATTR, /* cannot know if it is umask or mod */
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:u:u",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5100c0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:u=0:k=1:u=1",
	  .ret  = PFM_ERR_ATTR_SET,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:c=1:i",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1d300c0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:c=1:i=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1d300c0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:c=2",
	  .ret = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x25300c0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:c=320",
	  .ret  = PFM_ERR_ATTR_VAL,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "core::INST_RETIRED:ANY_P:t=1",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "core::L2_LINES_IN",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x537024ull,
	},
	{ .name = "core::L2_LINES_IN:SELF",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x537024ull,
	  .fstr = "core::L2_LINES_IN:SELF:ANY:k=1:u=1:e=0:i=0:c=0",
	},
	{ .name = "core::L2_LINES_IN:SELF:BOTH_CORES",
	  .ret  = PFM_ERR_FEATCOMB,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "core::L2_LINES_IN:SELF:PREFETCH",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x535024ull,
	},
	{ .name = "core::L2_LINES_IN:SELF:PREFETCH:ANY",
	  .ret  = PFM_ERR_FEATCOMB,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "core::RS_UOPS_DISPATCHED_NONE",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1d300a0ull,
	},
	{ .name = "core::RS_UOPS_DISPATCHED_NONE:c=2",
	  .ret  = PFM_ERR_ATTR_SET,
	  .count = 1,
	  .codes[0] = 0ull,
	},
	{ .name = "core::branch_instructions_retired",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5300c4ull,
	  .fstr = "core::BR_INST_RETIRED:ANY:k=1:u=1:e=0:i=0:c=0"
	},
	{ .name = "nhm::branch_instructions_retired",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5300c4ull,
	  .fstr = "nhm::BR_INST_RETIRED:ALL_BRANCHES:k=1:u=1:e=0:i=0:c=0:t=0"
	},
	{ .name = "wsm::BRANCH_INSTRUCTIONS_RETIRED",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5300c4ull, /* architected encoding, guaranteed to exist */
	  .fstr = "wsm::BR_INST_RETIRED:ALL_BRANCHES:k=1:u=1:e=0:i=0:c=0:t=0"
	},
	{ .name = "nhm::ARITH:DIV:k",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1d60114ull,
	  .fstr = "nhm::ARITH:CYCLES_DIV_BUSY:k=1:u=0:e=1:i=1:c=1:t=0",
	},
	{ .name = "nhm::ARITH:CYCLES_DIV_BUSY:k=1:u=1:e=1:i=1:c=1:t=0",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1d70114ull,
	  .fstr = "nhm::ARITH:CYCLES_DIV_BUSY:k=1:u=1:e=1:i=1:c=1:t=0",
	},
	{ .name = "wsm::UOPS_EXECUTED:CORE_STALL_COUNT:u",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1f53fb1ull,
	  .fstr = "wsm::UOPS_EXECUTED:CORE_STALL_CYCLES:k=0:u=1:e=1:i=1:c=1:t=1",
	},
	{ .name = "wsm::UOPS_EXECUTED:CORE_STALL_COUNT:u:t=0",
	  .ret  = PFM_ERR_ATTR_SET,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "wsm_unc::unc_qmc_writes:full_any:partial_any",
	  .ret  = PFM_ERR_FEATCOMB,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "wsm_unc::unc_qmc_writes",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x50072full,
	  .fstr = "wsm_unc::UNC_QMC_WRITES:FULL_ANY:e=0:i=0:c=0:o=0",
	},
	{ .name = "wsm_unc::unc_qmc_writes:full_any",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x50072full,
	  .fstr = "wsm_unc::UNC_QMC_WRITES:FULL_ANY:e=0:i=0:c=0:o=0",
	},
	{ .name = "wsm_unc::unc_qmc_writes:full_ch0",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x50012full,
	  .fstr = "wsm_unc::UNC_QMC_WRITES:FULL_CH0:e=0:i=0:c=0:o=0",
	},
	{ .name = "wsm_unc::unc_qmc_writes:partial_any",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x50382full,
	  .fstr = "wsm_unc::UNC_QMC_WRITES:PARTIAL_ANY:e=0:i=0:c=0:o=0",
	},
	{ .name = "wsm_unc::unc_qmc_writes:partial_ch0",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x50082full,
	  .fstr = "wsm_unc::UNC_QMC_WRITES:PARTIAL_CH0:e=0:i=0:c=0:o=0",
	},
	{ .name = "wsm_unc::unc_qmc_writes:partial_ch0:partial_ch1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x50182full,
	  .fstr = "wsm_unc::UNC_QMC_WRITES:PARTIAL_CH0:PARTIAL_CH1:e=0:i=0:c=0:o=0",
	},
	{ .name = "amd64_fam10h_barcelona::DISPATCHED_FPU",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x533f00ull,
	  .fstr = "amd64_fam10h_barcelona::DISPATCHED_FPU:ALL:k=1:u=1:e=0:i=0:c=0:h=0:g=0"
	},
	{ .name = "amd64_fam10h_barcelona::DISPATCHED_FPU:k:u=0",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x523f00ull,
	  .fstr = "amd64_fam10h_barcelona::DISPATCHED_FPU:ALL:k=1:u=0:e=0:i=0:c=0:h=0:g=0"
	},
	{ .name = "amd64_fam10h_barcelona::DISPATCHED_FPU:OPS_ADD:OPS_MULTIPLY",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x530300ull,
	  .fstr = "amd64_fam10h_barcelona::DISPATCHED_FPU:OPS_ADD:OPS_MULTIPLY:k=1:u=1:e=0:i=0:c=0:h=0:g=0",
	},
	{ .name = "amd64_fam10h_barcelona::L2_CACHE_MISS:ALL:DATA",
	  .ret  = PFM_ERR_FEATCOMB,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "amd64_fam10h_barcelona::MEMORY_CONTROLLER_REQUESTS",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x10053fff0ull,
	  .fstr = "amd64_fam10h_barcelona::MEMORY_CONTROLLER_REQUESTS:ALL:k=1:u=1:e=0:i=0:c=0:h=0:g=0",
	},
	{ .name = "amd64_k8_revb::RETURN_STACK_OVERFLOWS:g=1:u",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "amd64_k8_revb::RETURN_STACK_HITS:e=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x570088ull,
	  .fstr = "amd64_k8_revb::RETURN_STACK_HITS:k=1:u=1:e=1:i=0:c=0",
	},
	{ .name = "amd64_k8_revb::PROBE:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x533fecull,
	  .fstr = "amd64_k8_revb::PROBE:ALL:k=1:u=1:e=0:i=0:c=0",
	},
	{ .name = "amd64_k8_revc::PROBE:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x533fecull,
	  .fstr = "amd64_k8_revc::PROBE:ALL:k=1:u=1:e=0:i=0:c=0",
	},
	{ .name = "amd64_k8_revd::PROBE:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x537fecull,
	  .fstr = "amd64_k8_revd::PROBE:ALL:k=1:u=1:e=0:i=0:c=0"
	},
	{ .name = "amd64_k8_reve::PROBE:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x537fecull,
	  .fstr = "amd64_k8_reve::PROBE:ALL:k=1:u=1:e=0:i=0:c=0"
	},
	{ .name = "amd64_k8_revf::PROBE:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x537fecull,
	  .fstr = "amd64_k8_revf::PROBE:ALL:k=1:u=1:e=0:i=0:c=0"
	},
	{ .name = "amd64_k8_revg::PROBE:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x537fecull,
	  .fstr = "amd64_k8_revg::PROBE:ALL:k=1:u=1:e=0:i=0:c=0"
	},
	{ .name = "amd64_fam10h_barcelona::L1_DTLB_MISS_AND_L2_DTLB_HIT:L2_1G_TLB_HIT",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	  .codes[0] = 0ull,
	},
	{ .name = "amd64_fam10h_barcelona::L1_DTLB_MISS_AND_L2_DTLB_HIT:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x530345ull,
	  .fstr = "amd64_fam10h_barcelona::L1_DTLB_MISS_AND_L2_DTLB_HIT:ALL:k=1:u=1:e=0:i=0:c=0:h=0:g=0"
	},
	{ .name = "amd64_fam10h_shanghai::L1_DTLB_MISS_AND_L2_DTLB_HIT:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x530745ull,
	  .fstr = "amd64_fam10h_shanghai::L1_DTLB_MISS_AND_L2_DTLB_HIT:ALL:k=1:u=1:e=0:i=0:c=0:h=0:g=0"
	},
	{ .name = "amd64_fam10h_istanbul::L1_DTLB_MISS_AND_L2_DTLB_HIT:ALL",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x530745ull,
	  .fstr = "amd64_fam10h_istanbul::L1_DTLB_MISS_AND_L2_DTLB_HIT:ALL:k=1:u=1:e=0:i=0:c=0:h=0:g=0"
	},
	{ .name = "amd64_fam10h_barcelona::READ_REQUEST_TO_L3_CACHE",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x40053f7e0ull,
	  .fstr = "amd64_fam10h_barcelona::READ_REQUEST_TO_L3_CACHE:ANY_READ:ALL_CORES:k=1:u=1:e=0:i=0:c=0:h=0:g=0",
	},
	{ .name = "amd64_fam10h_shanghai::READ_REQUEST_TO_L3_CACHE",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x40053f7e0ull,
	  .fstr = "amd64_fam10h_shanghai::READ_REQUEST_TO_L3_CACHE:ANY_READ:ALL_CORES:k=1:u=1:e=0:i=0:c=0:h=0:g=0",
	},
	{ .name = "core::RAT_STALLS:ANY:u:c=1,cycles", /* must cut at comma */
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1510fd2ull,
	  .fstr = "core::RAT_STALLS:ANY:k=0:u=1:e=0:i=0:c=1"
	},
	{ .name = "wsm::mem_uncore_retired:remote_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x53200f,
	  .fstr = "wsm::MEM_UNCORE_RETIRED:REMOTE_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm_dp::mem_uncore_retired:remote_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x53100f,
	  .fstr = "wsm_dp::MEM_UNCORE_RETIRED:REMOTE_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::mem_uncore_retired:local_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x53100f,
	  .fstr = "wsm::MEM_UNCORE_RETIRED:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm_dp::mem_uncore_retired:local_dram",
	  .ret  = PFM_ERR_ATTR,
	  .count = 1,
	  .codes[0] = 0,
	},
	{ .name = "nhm::mem_uncore_retired:uncacheable",
	  .ret  = PFM_ERR_ATTR,
	  .count = 1,
	  .codes[0] = 0,
	},
	{ .name = "nhm::mem_uncore_retired:l3_data_miss_unknown",
	  .ret  = PFM_ERR_ATTR,
	  .count = 1,
	  .codes[0] = 0,
	},
	{ .name = "nhm_ex::mem_uncore_retired:uncacheable",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x53800f,
	  .fstr = "nhm_ex::MEM_UNCORE_RETIRED:UNCACHEABLE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm_ex::mem_uncore_retired:l3_data_miss_unknown",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x53010f,
	  .fstr = "nhm_ex::MEM_UNCORE_RETIRED:L3_DATA_MISS_UNKNOWN:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::mem_uncore_retired:local_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x53200f,
	  .fstr = "nhm::MEM_UNCORE_RETIRED:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm_ex::mem_uncore_retired:local_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x53200f,
	  .fstr = "nhm_ex::MEM_UNCORE_RETIRED:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_0:k",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5201b7,
	  .codes[1] = 0xffff,
	  .fstr = "wsm::OFFCORE_RESPONSE_0:ANY_REQUEST:ANY_RESPONSE:k=1:u=0:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_0:local_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x40ff,
	  .fstr = "wsm::OFFCORE_RESPONSE_0:ANY_REQUEST:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_0:PF_IFETCH",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0xff40,
	  .fstr = "wsm::OFFCORE_RESPONSE_0:PF_IFETCH:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_0:ANY_DATA:LOCAL_DRAM",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x4011,
	  .fstr = "wsm::OFFCORE_RESPONSE_0:DMND_DATA_RD:PF_DATA_RD:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_0:DMND_RFO:DMND_DATA_RD:LOCAL_DRAM:REMOTE_DRAM",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x6003,
	  .fstr = "wsm::OFFCORE_RESPONSE_0:DMND_DATA_RD:DMND_RFO:REMOTE_DRAM:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_1:local_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x40ff,
	  .fstr = "wsm::OFFCORE_RESPONSE_1:ANY_REQUEST:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_1:PF_IFETCH",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0xff40,
	  .fstr = "wsm::OFFCORE_RESPONSE_1:PF_IFETCH:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_1:ANY_DATA:LOCAL_DRAM",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x4011,
	  .fstr = "wsm::OFFCORE_RESPONSE_1:DMND_DATA_RD:PF_DATA_RD:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::offcore_response_1:DMND_RFO:DMND_DATA_RD:LOCAL_DRAM:REMOTE_DRAM",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x6003,
	  .fstr = "wsm::OFFCORE_RESPONSE_1:DMND_DATA_RD:DMND_RFO:REMOTE_DRAM:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::offcore_response_0:k",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5201b7,
	  .codes[1] = 0xffff,
	  .fstr = "nhm::OFFCORE_RESPONSE_0:ANY_REQUEST:ANY_RESPONSE:k=1:u=0:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::offcore_response_0:local_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x40ff,
	  .fstr = "nhm::OFFCORE_RESPONSE_0:ANY_REQUEST:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::offcore_response_0:any_llc_miss",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0xf8ff,
	  .fstr = "nhm::OFFCORE_RESPONSE_0:ANY_REQUEST:REMOTE_CACHE_HITM:REMOTE_CACHE_FWD:REMOTE_DRAM:LOCAL_DRAM:NON_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::offcore_response_0:any_dram",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x60ff,
	  .fstr = "nhm::OFFCORE_RESPONSE_0:ANY_REQUEST:REMOTE_DRAM:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::offcore_response_0:PF_IFETCH",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0xff40,
	  .fstr = "nhm::OFFCORE_RESPONSE_0:PF_IFETCH:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::offcore_response_0:ANY_DATA:LOCAL_DRAM",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x4011,
	  .fstr = "nhm::OFFCORE_RESPONSE_0:DMND_DATA_RD:PF_DATA_RD:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "nhm::offcore_response_0:DMND_RFO:DMND_DATA_RD:LOCAL_DRAM:REMOTE_DRAM",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x6003,
	  .fstr = "nhm::OFFCORE_RESPONSE_0:DMND_DATA_RD:DMND_RFO:REMOTE_DRAM:LOCAL_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "amd64_k8_revg::DISPATCHED_FPU:0xff:k",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x52ff00ull,
	  .fstr = "amd64_k8_revg::DISPATCHED_FPU:0xff:k=1:u=0:e=0:i=0:c=0"
	},
	{ .name = "amd64_k8_revg::DISPATCHED_FPU:0x4ff",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	},
	{ .name = "amd64_fam10h_barcelona::DISPATCHED_FPU:0x4ff:u",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x40051ff00ull,
	  .fstr = "amd64_fam10h_barcelona::DISPATCHED_FPU:0x4ff:k=0:u=1:e=0:i=0:c=0:h=0:g=0"
	},
	{ .name = "wsm::inst_retired:0xff:k",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x52ffc0,
	  .fstr = "wsm::INST_RETIRED:0xff:k=1:u=0:e=0:i=0:c=0:t=0",
	},
	{ .name = "wsm::uops_issued:0xff:stall_cycles",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	},
	{ .name = "wsm::uops_issued:0xff:0xf1",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	},
	{ .name = "wsm::uops_issued:0xff=",
	  .ret  = PFM_ERR_ATTR_VAL,
	  .count = 0,
	},
	{ .name = "wsm::uops_issued:123",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	},
	{ .name = "wsm::uops_issued:0xfff",
	  .ret  = PFM_ERR_ATTR,
	  .count = 0,
	},
	{ .name = "netburst::global_power_events",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x2600020f,
	  .codes[1] = 0x3d000,
	  .fstr = "netburst::global_power_events:RUNNING:k=1:u=1:e=0:cmpl=0:thr=0",
	},
	{ .name = "netburst::global_power_events:RUNNING:u:k",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x2600020f,
	  .codes[1] = 0x3d000,
	  .fstr = "netburst::global_power_events:RUNNING:k=1:u=1:e=0:cmpl=0:thr=0",
	},
	{ .name = "netburst::global_power_events:RUNNING:e",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x2600020f,
	  .codes[1] = 0x107d000,
	  .fstr = "netburst::global_power_events:RUNNING:k=1:u=1:e=1:cmpl=0:thr=0",
	},
	{ .name = "netburst::global_power_events:RUNNING:cmpl:e:u",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x26000205,
	  .codes[1] = 0x10fd000,
	  .fstr = "netburst::global_power_events:RUNNING:k=0:u=1:e=1:cmpl=1:thr=0",
	},
	{ .name = "netburst::global_power_events:RUNNING:cmpl:thr=8:u",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x26000205,
	  .codes[1] = 0x8fd000,
	  .fstr = "netburst::global_power_events:RUNNING:k=0:u=1:e=0:cmpl=1:thr=8",
	},
	{ .name = "netburst::global_power_events:RUNNING:cmpl:thr=32:u",
	  .ret  = PFM_ERR_ATTR_VAL,
	  .count = 0,
	},
	{ .name = "netburst::instr_completed:nbogus",
	  .ret  = PFM_ERR_NOTFOUND,
	  .count = 0,
	},
	{ .name = "netburst_p::instr_completed:nbogus",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0xe00020f,
	  .codes[1] = 0x39000,
	  .fstr = "netburst_p::instr_completed:NBOGUS:k=1:u=1:e=0:cmpl=0:thr=0",
	},
	{ .name = "snb::cpl_cycles:ring0_trans:u",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x155015c,
	  .fstr = "snb::CPL_CYCLES:RING0:k=0:u=1:e=1:i=0:c=1:t=0",
	},
	{ .name = "snb::cpl_cycles:ring0_trans:e=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x157015cull,
	},
	{ .name = "snb::OFFCORE_REQUESTS_OUTSTanding:ALL_DATA_RD_CYCLES",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1530860,
	  .fstr = "snb::OFFCORE_REQUESTS_OUTSTANDING:ALL_DATA_RD:k=1:u=1:e=0:i=0:c=1:t=0",
	},
	{ .name = "snb::uops_issued:core_stall_cycles:u:k",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x1f3010e,
	  .fstr = "snb::UOPS_ISSUED:ANY:k=1:u=1:e=0:i=1:c=1:t=1",
	},
	{ .name = "snb::LLC_REFERences:k:t=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x724f2e,
	  .fstr = "snb::LAST_LEVEL_CACHE_REFERENCES:k=1:u=0:e=0:i=0:c=0:t=1",
	},
	{ .name = "snb::ITLB:0x1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5301ae,
	  .fstr = "snb::ITLB:0x1:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:DMND_RFO:ANY_RESPONSE",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x10002,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_RFO:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:DMND_RFO:ANY_REQUEST",
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x18fff,
	},
	{ .name = "snb::offcore_response_0:DMND_RFO",
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x10002,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_RFO:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:any_response",
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x18fff,
	  .fstr = "snb::OFFCORE_RESPONSE_0:ANY_REQUEST:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:NO_SUPP:SNP_NONE:PF_RFO:PF_IFETCH",
	  .count = 2,
	  .codes[0] = 0x5301b7,
	  .codes[1] = 0x80020060,
	  .fstr = "snb::OFFCORE_RESPONSE_0:PF_RFO:PF_IFETCH:NO_SUPP:SNP_NONE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_1:DMND_RFO:ANY_RESPONSE",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x10002,
	  .fstr = "snb::OFFCORE_RESPONSE_1:DMND_RFO:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_1:DMND_RFO:ANY_REQUEST",
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x18fff,
	},
	{ .name = "snb::offcore_response_1:DMND_RFO",
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x10002,
	  .fstr = "snb::OFFCORE_RESPONSE_1:DMND_RFO:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_1:any_response",
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x18fff,
	  .fstr = "snb::OFFCORE_RESPONSE_1:ANY_REQUEST:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_1:NO_SUPP:SNP_NONE:PF_RFO:PF_IFETCH",
	  .count = 2,
	  .codes[0] = 0x5301bb,
	  .codes[1] = 0x80020060,
	  .fstr = "snb::OFFCORE_RESPONSE_1:PF_RFO:PF_IFETCH:NO_SUPP:SNP_NONE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "amd64_fam14h_bobcat::MAB_REQUESTS:DC_BUFFER_0",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x530068,
	  .fstr = "amd64_fam14h_bobcat::MAB_REQUESTS:DC_BUFFER_0:k=1:u=1:e=0:i=0:c=0:h=0:g=0",
	},
	{ .name = "amd64_fam14h_bobcat::MAB_REQUESTS:DC_BUFFER_0:DC_BUFFER_1",
	  .ret  = PFM_ERR_FEATCOMB,
	},
	{ .name = "amd64_fam14h_bobcat::MAB_REQUESTS:DC_BUFFER_0:IC_BUFFER_0",
	  .ret  = PFM_ERR_FEATCOMB,
	},
	{ .name = "amd64_fam14h_bobcat::MAB_REQUESTS:ANY_DC_BUFFER",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x530b68,
	  .fstr = "amd64_fam14h_bobcat::MAB_REQUESTS:ANY_DC_BUFFER:k=1:u=1:e=0:i=0:c=0:h=0:g=0",
	},
	{ .name = "amd64_fam14h_bobcat::MAB_REQUESTS:ANY_IC_BUFFER",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x530a68,
	  .fstr = "amd64_fam14h_bobcat::MAB_REQUESTS:ANY_IC_BUFFER:k=1:u=1:e=0:i=0:c=0:h=0:g=0",
	},
	{ .name = "amd64_fam14h_bobcat::MAB_REQUESTS:ANY_IC_BUFFER:IC_BUFFER_1",
	  .ret  = PFM_ERR_FEATCOMB,
	},

	{ .name = "core::INST_RETIRED:ANY_P:e",
	  .ret  = PFM_ERR_ATTR,
	},
	{ .name = "core::INST_RETIRED:ANY_P:e:c=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x15700c0ull,
	},
	{ .name = "atom::INST_RETIRED:ANY_P:e",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x5700c0ull,
	},
	{ .name = "atom::INST_RETIRED:ANY_P:e:c=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x15700c0ull,
	},
	{ .name = "nhm::INST_RETIRED:ANY_P:e",
	  .ret  = PFM_ERR_ATTR,
	},
	{ .name = "nhm::INST_RETIRED:ANY_P:e:c=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x15700c0ull,
	},
	{ .name = "wsm::INST_RETIRED:ANY_P:e",
	  .ret  = PFM_ERR_ATTR,
	},
	{ .name = "wsm::INST_RETIRED:ANY_P:e:c=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x15700c0ull,
	},
	{ .name = "snb::INST_RETIRED:ANY_P:e",
	  .ret  = PFM_ERR_ATTR,
	},
	{ .name = "snb::INST_RETIRED:ANY_P:e:c=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x15700c0ull,
	},
	{ .name = "snb::INST_RETIRED:ANY_P:e:c=1",
	  .ret  = PFM_SUCCESS,
	  .count = 1,
	  .codes[0] = 0x15700c0ull,
	},
	{ .name = "snb::offcore_response_0:any_request",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7ull,
	  .codes[1]= 0x18fffull,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_DATA_RD:DMND_RFO:DMND_IFETCH:WB:PF_DATA_RD:PF_RFO:PF_IFETCH:PF_LLC_DATA_RD:PF_LLC_RFO:PF_LLC_IFETCH:BUS_LOCKS:STRM_ST:OTHER:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:dmnd_data_rd",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7ull,
	  .codes[1]=0x10001ull,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_DATA_RD:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:dmnd_data_rd:llc_hite",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7ull,
	  .codes[1]=0x3f80080001ull,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_DATA_RD:LLC_HITE:SNP_ANY:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:dmnd_data_rd:llc_hite:snp_any",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7ull,
	  .codes[1]=0x3f80080001ull,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_DATA_RD:LLC_HITE:SNP_NONE:SNP_NOT_NEEDED:SNP_MISS:SNP_NO_FWD:SNP_FWD:HITM:NON_DRAM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:dmnd_data_rd:llc_hite:hitm",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7ull,
	  .codes[1]=0x1000080001ull,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_DATA_RD:LLC_HITE:HITM:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:dmnd_data_rd:any_response",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7ull,
	  .codes[1]=0x10001ull,
	  .fstr = "snb::OFFCORE_RESPONSE_0:DMND_DATA_RD:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
	{ .name = "snb::offcore_response_0:dmnd_data_rd:any_response:snp_any",
	  .ret  = PFM_ERR_FEATCOMB,
	},
	{ .name = "snb::offcore_response_0:dmnd_data_rd:any_response:llc_hitmesf",
	  .ret  = PFM_ERR_FEATCOMB,
	},
	{ .name = "snb::offcore_response_0:any_response",
	  .ret  = PFM_SUCCESS,
	  .count = 2,
	  .codes[0] = 0x5301b7ull,
	  .codes[1]=0x18fffull,
	  .fstr = "snb::OFFCORE_RESPONSE_0:ANY_REQUEST:ANY_RESPONSE:k=1:u=1:e=0:i=0:c=0:t=0",
	},
};
#define NUM_TEST_EVENTS (sizeof(x86_test_events)/sizeof(test_event_t))

static int check_test_events(FILE *fp)
{
	const test_event_t *e;
	char *fstr;
	uint64_t *codes;
	int count, i, j;
	int ret, errors = 0;

	for (i=0, e = x86_test_events; i < NUM_TEST_EVENTS; i++, e++) {
		codes = NULL;
		count = 0;
		fstr = NULL;
		ret = pfm_get_event_encoding(e->name, PFM_PLM0 | PFM_PLM3, &fstr, NULL, &codes, &count);
		if (ret != e->ret) {
			fprintf(fp,"Event%d %s, ret=%s(%d) expected %s(%d)\n", i, e->name, pfm_strerror(ret), ret, pfm_strerror(e->ret), e->ret);
			errors++;
		} else {
			if (ret != PFM_SUCCESS) {
				if (fstr) {
					fprintf(fp,"Event%d %s, expected fstr NULL but it is not\n", i, e->name);
					errors++;
				}
				if (count != 0) {
					fprintf(fp,"Event%d %s, expected count=0 instead of %d\n", i, e->name, count);
					errors++;
				}
				if (codes) {
					fprintf(fp,"Event%d %s, expected codes[] NULL but it is not\n", i, e->name);
					errors++;
				}
			} else {
				if (count != e->count) {
					fprintf(fp,"Event%d %s, count=%d expected %d\n", i, e->name, count, e->count);
					errors++;
				}
				for (j=0; j < count; j++) {
					if (codes[j] != e->codes[j]) {
						fprintf(fp,"Event%d %s, codes[%d]=%#"PRIx64" expected %#"PRIx64"\n", i, e->name, j, codes[j], e->codes[j]);
						errors++;
					}
				}
				if (e->fstr && strcmp(fstr, e->fstr)) {
					fprintf(fp,"Event%d %s, fstr=%s expected %s\n", i, e->name, fstr, e->fstr);
					errors++;
				}
			}
		}
		if (codes)
			free(codes);
		if (fstr)
			free(fstr);
	}
	printf("\t %d x86 events: %d errors\n", i, errors);
	return errors;
}

int
validate_arch(FILE *fp)
{
	return check_test_events(fp);
}
