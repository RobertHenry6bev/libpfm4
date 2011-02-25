/*
 * Performance events:
 *
 *    Copyright (C) 2008-2009, Thomas Gleixner <tglx@linutronix.de>
 *    Copyright (C) 2008-2009, Red Hat, Inc., Ingo Molnar
 *    Copyright (C) 2008-2009, Red Hat, Inc., Peter Zijlstra
 *
 * Data type definitions, declarations, prototypes.
 *
 *    Started by: Thomas Gleixner and Ingo Molnar
 *
 * This file is licensed under GPLv2. For licensing details
 * see Linux kernel source tree and file COPYING in the top
 * level directory.
 */
#ifndef _LINUX_PERF_EVENT_H
#define _LINUX_PERF_EVENT_H

#include <sys/types.h>
#include <asm/types.h>

/*
 * hw_event.type
 */
enum perf_type_id {
	PERF_TYPE_HARDWARE		= 0,
	PERF_TYPE_SOFTWARE		= 1,
	PERF_TYPE_TRACEPOINT		= 2,
	PERF_TYPE_HW_CACHE		= 3,
	PERF_TYPE_RAW			= 4,
	PERF_TYPE_BREAKPOINT		= 5,
	PERF_TYPE_MAX,			/* non-ABI */
};


/*
 * Generalized performance event event_id types, used by the
 * attr.event_id parameter of the sys_perf_event_open()
 * syscall:
 */
enum perf_hw_id {
	/*
	 * Common hardware events, generalized by the kernel:
	 */
	PERF_COUNT_HW_CPU_CYCLES		= 0,
	PERF_COUNT_HW_INSTRUCTIONS		= 1,
	PERF_COUNT_HW_CACHE_REFERENCES		= 2,
	PERF_COUNT_HW_CACHE_MISSES		= 3,
	PERF_COUNT_HW_BRANCH_INSTRUCTIONS	= 4,
	PERF_COUNT_HW_BRANCH_MISSES		= 5,
	PERF_COUNT_HW_BUS_CYCLES		= 6,

	PERF_COUNT_HW_MAX,			/* non-ABI */
};

/*
 * Generalized hardware cache events:
 *
 *       { L1-D, L1-I, LLC, ITLB, DTLB, BPU } x
 *       { read, write, prefetch } x
 *       { accesses, misses }
 */
enum perf_hw_cache_id {
	PERF_COUNT_HW_CACHE_L1D			= 0,
	PERF_COUNT_HW_CACHE_L1I			= 1,
	PERF_COUNT_HW_CACHE_LL			= 2,
	PERF_COUNT_HW_CACHE_DTLB		= 3,
	PERF_COUNT_HW_CACHE_ITLB		= 4,
	PERF_COUNT_HW_CACHE_BPU			= 5,

	PERF_COUNT_HW_CACHE_MAX,		/* non-ABI */
};

enum perf_hw_cache_op_id {
	PERF_COUNT_HW_CACHE_OP_READ		= 0,
	PERF_COUNT_HW_CACHE_OP_WRITE		= 1,
	PERF_COUNT_HW_CACHE_OP_PREFETCH		= 2,

	PERF_COUNT_HW_CACHE_OP_MAX,		/* non-ABI */
};

enum perf_hw_cache_op_result_id {
	PERF_COUNT_HW_CACHE_RESULT_ACCESS	= 0,
	PERF_COUNT_HW_CACHE_RESULT_MISS		= 1,

	PERF_COUNT_HW_CACHE_RESULT_MAX,		/* non-ABI */
};

/*
 * Special "software" events provided by the kernel, even if the hardware
 * does not support performance events. These events measure various
 * physical and sw events of the kernel (and allow the profiling of them as
 * well):
 */
enum perf_sw_ids {
	PERF_COUNT_SW_CPU_CLOCK			= 0,
	PERF_COUNT_SW_TASK_CLOCK		= 1,
	PERF_COUNT_SW_PAGE_FAULTS		= 2,
	PERF_COUNT_SW_CONTEXT_SWITCHES		= 3,
	PERF_COUNT_SW_CPU_MIGRATIONS		= 4,
	PERF_COUNT_SW_PAGE_FAULTS_MIN		= 5,
	PERF_COUNT_SW_PAGE_FAULTS_MAJ		= 6,
	PERF_COUNT_SW_ALIGNMENT_FAULTS		= 7,
	PERF_COUNT_SW_EMULATION_FAULTS		= 8,

	PERF_COUNT_SW_MAX,			/* non-ABI */
};

/*
 * Bits that can be set in attr.sample_type to request information
 * in the overflow packets.
 */
enum perf_event_sample_format {
	PERF_SAMPLE_IP			= 1U << 0,
	PERF_SAMPLE_TID			= 1U << 1,
	PERF_SAMPLE_TIME		= 1U << 2,
	PERF_SAMPLE_ADDR		= 1U << 3,
	PERF_SAMPLE_READ		= 1U << 4,
	PERF_SAMPLE_CALLCHAIN		= 1U << 5,
	PERF_SAMPLE_ID			= 1U << 6,
	PERF_SAMPLE_CPU			= 1U << 7,
	PERF_SAMPLE_PERIOD		= 1U << 8,
	PERF_SAMPLE_STREAM_ID		= 1U << 9,
	PERF_SAMPLE_RAW			= 1U << 10,

	PERF_SAMPLE_MAX			= 1U << 11,	/* non-ABI */
};

/*
 * The format of the data returned by read() on a perf event fd,
 * as specified by attr.read_format:
 *
 * struct read_format {
 * 	{ u64		value;
 * 	  { u64		time_enabled; } && PERF_FORMAT_ENABLED
 * 	  { u64		time_running; } && PERF_FORMAT_RUNNING
 * 	  { u64		id;           } && PERF_FORMAT_ID
 * 	} && !PERF_FORMAT_GROUP
 *
 * 	{ u64		nr;
 * 	  { u64		time_enabled; } && PERF_FORMAT_ENABLED
 * 	  { u64		time_running; } && PERF_FORMAT_RUNNING
 * 	  { u64		value;
 * 	    { u64	id;           } && PERF_FORMAT_ID
 * 	  }		cntr[nr];
 * 	} && PERF_FORMAT_GROUP
 * };
 */
enum perf_event_read_format {
	PERF_FORMAT_TOTAL_TIME_ENABLED	= 1U << 0,
	PERF_FORMAT_TOTAL_TIME_RUNNING	= 1U << 1,
	PERF_FORMAT_ID			= 1U << 2,
	PERF_FORMAT_GROUP		= 1U << 3,

	PERF_FORMAT_MAX = 1U << 4, 	/* non-ABI */
};

#define PERF_ATTR_SIZE_VER0	64	/* sizeof first published struct */

/* SWIG doesn't deal well with anonymous nested structures */
#ifdef SWIG
#define SWIG_NAME(x) x
#else
#define SWIG_NAME(x)
#endif /* SWIG */

/*
 * Hardware event_id to monitor via a performance monitoring event:
 */
struct perf_event_attr {
	/*
	 * Major type: hardware/software/tracepoint/etc.
	 */
	__u32	type;
	/*
	 * Size of the attr structure, for fwd/bwd compat.
	 */
	__u32	size;
	/*
	 * Type specific configuration information.
	 */
	__u64	config;

	union {
		__u64	sample_period;
		__u64	sample_freq;
	} SWIG_NAME(sample);
	__u64	sample_type;
	__u64	read_format;

	__u64		disabled       :  1, /* off by default        */
			inherit	       :  1, /* children inherit it   */
			pinned	       :  1, /* must always be on PMU */
			exclusive      :  1, /* only group on PMU     */
			exclude_user   :  1, /* don't count user      */
			exclude_kernel :  1, /* ditto kernel          */
			exclude_hv     :  1, /* ditto hypervisor      */
			exclude_idle   :  1, /* don't count when idle */
			mmap           :  1, /* include mmap data     */
			comm	       :  1, /* include comm data     */
			freq           :  1, /* use freq, not period  */
			inherit_stat   :  1, /* per task counts       */
			enable_on_exec :  1, /* next exec enables     */
			task           :  1, /* trace fork/exit       */
			watermark      :  1, /* wakeup_watermark      */
			/*
			 * precise_ip:
			 *  0 - SAMPLE_IP can have arbitrary skid
			 *  1 - SAMPLE_IP must have constant skid
			 *  2 - SAMPLE_IP requested to have 0 skid
			 *  3 - SAMPLE_IP must have 0 skid
			 *
			 *  See also PERF_RECORD_MISC_EXACT_IP
			 */
			precise_ip     :  2, /* skid constraint       */
			mmap_data      :  1, /* non-exec mmap data    */
			sample_id_all  :  1, /* sample_type all events */

			__reserved_1   : 45;

	union {
		__u32	wakeup_events;		/* wakeup every n events */
		__u32	wakeup_watermark;	/* bytes before wakeup */
	} SWIG_NAME(wakeup);

	__u32        bp_type;
	__u64        bp_addr;
	__u64        bp_len;
};

/*
 * Ioctls that can be done on a perf event fd:
 */
#define PERF_EVENT_IOC_ENABLE		_IO ('$', 0)
#define PERF_EVENT_IOC_DISABLE		_IO ('$', 1)
#define PERF_EVENT_IOC_REFRESH		_IO ('$', 2)
#define PERF_EVENT_IOC_RESET		_IO ('$', 3)
#define PERF_EVENT_IOC_PERIOD		_IOW('$', 4, u64)
#define PERF_EVENT_IOC_SET_OUTPUT	_IO ('$', 5)
#define PERF_EVENT_IOC_SET_FILTER	_IOW('$', 6, char *)

enum perf_event_ioc_flags {
	PERF_IOC_FLAG_GROUP		= 1U << 0,
};

/*
 * Structure of the page that can be mapped via mmap
 */
struct perf_event_mmap_page {
	__u32	version;		/* version number of this structure */
	__u32	compat_version;		/* lowest version this is compat with */
	__u32	lock;			/* seqlock for synchronization */
	__u32	index;			/* hardware event identifier */
	int64_t		offset;			/* add to hardware event value */
	__u64	time_enabled;		/* time event active */
	__u64	time_running;		/* time event on cpu */

		/*
		 * Hole for extension of the self monitor capabilities
		 */
	__u64	__reserved[123];	/* align to 1k */

	/*
	 * Control data for the mmap() data buffer.
	 *
	 * User-space reading the @data_head value should issue an rmb(), on
	 * SMP capable platforms, after reading this value -- see
	 * perf_event_wakeup().
	 *
	 * When the mapping is PROT_WRITE the @data_tail value should be
	 * written by userspace to reflect the last read data. In this case
	 * the kernel will not over-write unread data.
	 */
	__u64   	data_head;		/* head in the data section */
	__u64	data_tail;		/* user-space written tail */
};

#define PERF_EVENT_MISC_CPUMODE_MASK	(3 << 0)
#define PERF_EVENT_MISC_CPUMODE_UNKNOWN	(0 << 0)
#define PERF_EVENT_MISC_KERNEL		(1 << 0)
#define PERF_EVENT_MISC_USER		(2 << 0)
#define PERF_EVENT_MISC_HYPERVISOR	(3 << 0)

#define PERF_RECORD_MISC_EXACT			(1 << 14)
/*
 * Indicates that the content of PERF_SAMPLE_IP points to
 * the actual instruction that triggered the event. See also
 * perf_event_attr::precise_ip.
 */
#define PERF_RECORD_MISC_EXACT_IP               (1 << 14)
/*
 * Reserve the last bit to indicate some extended misc field
 */
#define PERF_RECORD_MISC_EXT_RESERVED		(1 << 15)

struct perf_event_header {
	__u32	type;
	__u16	misc;
	__u16	size;
};

enum perf_event_type {

	/*
	 * The MMAP events record the PROT_EXEC mappings so that we can
	 * correlate userspace IPs to code. They have the following structure:
	 *
	 * struct {
	 *	struct perf_event_header	header;
	 *
	 *	u32				pid, tid;
	 *	u64				addr;
	 *	u64				len;
	 *	u64				pgoff;
	 *	char				filename[];
	 * };
	 */
	PERF_RECORD_MMAP			= 1,

	/*
	 * struct {
	 * 	struct perf_event_header	header;
	 * 	u64				id;
	 * 	u64				lost;
	 * };
	 */
	PERF_RECORD_LOST			= 2,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *
	 *	u32				pid, tid;
	 *	char				comm[];
	 * };
	 */
	PERF_RECORD_COMM			= 3,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u32				pid, ppid;
	 *	u32				tid, ptid;
	 *	u64				time;
	 * };
	 */
	PERF_RECORD_EXIT			= 4,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u64				time;
	 *	u64				id;
	 *	u64				stream_id;
	 * };
	 */
	PERF_RECORD_THROTTLE		= 5,
	PERF_RECORD_UNTHROTTLE		= 6,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *	u32				pid, ppid;
	 *	u32				tid, ptid;
	 *	{ u64				time;     } && PERF_SAMPLE_TIME
	 * };
	 */
	PERF_RECORD_FORK			= 7,

	/*
	 * struct {
	 * 	struct perf_event_header	header;
	 * 	u32				pid, tid;
	 * 	struct read_format		values;
	 * };
	 */
	PERF_RECORD_READ			= 8,

	/*
	 * struct {
	 *	struct perf_event_header	header;
	 *
	 *	{ u64			ip;	  } && PERF_SAMPLE_IP
	 *	{ u32			pid, tid; } && PERF_SAMPLE_TID
	 *	{ u64			time;     } && PERF_SAMPLE_TIME
	 *	{ u64			addr;     } && PERF_SAMPLE_ADDR
	 *	{ u64			id;	  } && PERF_SAMPLE_ID
	 *	{ u64			stream_id;} && PERF_SAMPLE_STREAM_ID
	 *	{ u32			cpu, res; } && PERF_SAMPLE_CPU
	 * 	{ u64			period;   } && PERF_SAMPLE_PERIOD
	 *
	 *	{ struct read_format	values;	  } && PERF_SAMPLE_READ
	 *
	 *	{ u64			nr,
	 *	  u64			ips[nr];  } && PERF_SAMPLE_CALLCHAIN
	 *
	 * 	#
	 * 	# The RAW record below is opaque data wrt the ABI
	 * 	#
	 * 	# That is, the ABI doesn't make any promises wrt to
	 * 	# the stability of its content, it may vary depending
	 * 	# on event, hardware, kernel version and phase of
	 * 	# the moon.
	 * 	#
	 * 	# In other words, PERF_SAMPLE_RAW contents are not an ABI.
	 * 	#
	 *
	 *	{ u32			size;
	 *	  char                  data[size];}&& PERF_SAMPLE_RAW
	 * };
	 */
	PERF_RECORD_SAMPLE		= 9,

	PERF_RECORD_MAX,			/* non-ABI */
};

enum perf_callchain_context {
	PERF_CONTEXT_HV			= (__u64)-32,
	PERF_CONTEXT_KERNEL		= (__u64)-128,
	PERF_CONTEXT_USER		= (__u64)-512,

	PERF_CONTEXT_GUEST		= (__u64)-2048,
	PERF_CONTEXT_GUEST_KERNEL	= (__u64)-2176,
	PERF_CONTEXT_GUEST_USER		= (__u64)-2560,

	PERF_CONTEXT_MAX		= (__u64)-4095,
};

#define PERF_FLAG_FD_NO_GROUP	(1U << 0)
#define PERF_FLAG_FD_OUTPUT	(1U << 1)
#define PERF_FLAG_PID_CGROUP	(1U << 2)

#ifdef __linux__
#include <unistd.h>
#include <sys/syscall.h>

#ifndef __NR_perf_event_open
#ifdef __x86_64__
# define __NR_perf_event_open	298
#endif

#ifdef __i386__
# define __NR_perf_event_open 336
#endif

#ifdef __powerpc__
#define __NR_perf_event_open 319
#endif

#ifdef __arm__
#if defined(__ARM_EABI__) || defined(__thumb__)
#define __NR_perf_event_open 364
#else
#define __NR_perf_event_open (0x900000+364)
#endif
#endif
#endif /* __NR_perf_event_open */

static inline int
perf_event_open(
	struct perf_event_attr		*hw_event_uptr,
	pid_t				pid,
	int				cpu,
	int				group_fd,
	unsigned long			flags)
{
	return syscall(
		__NR_perf_event_open, hw_event_uptr, pid, cpu, group_fd, flags);
}
#endif
#endif /* _LINUX_PERF_EVENT_H */
