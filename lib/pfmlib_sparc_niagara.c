/*
 * pfmlib_sparc_niagara.c : SPARC Niagara I, II
 *
 * Copyright (c) 2011 Google, Inc
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
 * Core PMU = architectural perfmon v2 + PEBS
 */

/* private headers */
#include "pfmlib_priv.h"
#include "pfmlib_sparc_priv.h"
#include "events/sparc_niagara1_events.h"
#include "events/sparc_niagara2_events.h"

pfmlib_pmu_t sparc_niagara1_support={
	.desc			= "Sparc Niagara I",
	.name			= "niagara1",
	.pmu			= PFM_PMU_SPARC_NIAGARA1,
	.pme_count		= PME_SPARC_NIAGARA1_EVENT_COUNT,
	.max_encoding		= 2,
	.pe			= niagara1_pe,
	.atdesc			= NULL,
	.flags			= 0,

	.pmu_detect		= pfm_sparc_detect,
	.get_event_encoding	= pfm_sparc_get_encoding,
	.get_event_first	= pfm_sparc_get_event_first,
	.get_event_next		= pfm_sparc_get_event_next,
	.event_is_valid		= pfm_sparc_event_is_valid,
	.validate_table		= pfm_sparc_validate_table,
	.get_event_info		= pfm_sparc_get_event_info,
	.get_event_attr_info	= pfm_sparc_get_event_attr_info,
	.validate_pattrs	= pfm_sparc_validate_pattrs,
	.get_event_nattrs	= pfm_sparc_get_event_nattrs,
};

pfmlib_pmu_t sparc_niagara2_support={
	.desc			= "Sparc Niagara II",
	.name			= "niagara2",
	.pmu			= PFM_PMU_SPARC_NIAGARA2,
	.pme_count		= PME_SPARC_NIAGARA2_EVENT_COUNT,
	.max_encoding		= 2,
	.pe			= niagara2_pe,
	.atdesc			= NULL,
	.flags			= 0,

	.pmu_detect		= pfm_sparc_detect,
	.get_event_encoding	= pfm_sparc_get_encoding,
	.get_event_first	= pfm_sparc_get_event_first,
	.get_event_next		= pfm_sparc_get_event_next,
	.event_is_valid		= pfm_sparc_event_is_valid,
	.validate_table		= pfm_sparc_validate_table,
	.get_event_info		= pfm_sparc_get_event_info,
	.get_event_attr_info	= pfm_sparc_get_event_attr_info,
	.validate_pattrs	= pfm_sparc_validate_pattrs,
	.get_event_nattrs	= pfm_sparc_get_event_nattrs,
};
