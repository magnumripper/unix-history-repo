/*-
 * Copyright (c) 2002-2009 Luigi Rizzo, Universita` di Pisa
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _IPFW2_H
#define _IPFW2_H

/*
 * The default rule number.  By the design of ip_fw, the default rule
 * is the last one, so its number can also serve as the highest number
 * allowed for a rule.  The ip_fw code relies on both meanings of this
 * constant. 
 */
#define	IPFW_DEFAULT_RULE	65535

#define	RESVD_SET		31	/*set for default and persistent rules*/
#define	IPFW_MAX_SETS		32	/* Number of sets supported by ipfw*/

/*
 * Default number of ipfw tables.
 */
#define	IPFW_TABLES_MAX		65535
#define	IPFW_TABLES_DEFAULT	128

/*
 * Most commands (queue, pipe, tag, untag, limit...) can have a 16-bit
 * argument between 1 and 65534. The value 0 is unused, the value
 * 65535 (IP_FW_TABLEARG) is used to represent 'tablearg', i.e. the
 * can be 1..65534, or 65535 to indicate the use of a 'tablearg'
 * result of the most recent table() lookup.
 * Note that 16bit is only a historical limit, resulting from
 * the use of a 16-bit fields for that value. In reality, we can have
 * 2^32 pipes, queues, tag values and so on, and use 0 as a tablearg.
 */
#define	IPFW_ARG_MIN		1
#define	IPFW_ARG_MAX		65534
#define IP_FW_TABLEARG		65535	/* XXX should use 0 */

/*
 * Number of entries in the call stack of the call/return commands.
 * Call stack currently is an uint16_t array with rule numbers.
 */
#define	IPFW_CALLSTACK_SIZE	16

/* IP_FW3 header/opcodes */
typedef struct _ip_fw3_opheader {
	uint16_t opcode;	/* Operation opcode */
	uint16_t version;	/* Opcode version */
	uint16_t reserved[2];	/* Align to 64-bit boundary */
} ip_fw3_opheader;


/* IPFW extented tables support */
#define	IP_FW_TABLE_XADD	86	/* add entry */
#define	IP_FW_TABLE_XDEL	87	/* delete entry */
#define	IP_FW_TABLE_XGETSIZE	88	/* get table size (deprecated) */
#define	IP_FW_TABLE_XLIST	89	/* list table contents */
#define	IP_FW_TABLE_XDESTROY	90	/* destroy table */
#define	IP_FW_TABLES_XGETSIZE	91	/* get size for tables list */
#define	IP_FW_TABLES_XLIST	92	/* list all tables  */
#define	IP_FW_TABLE_XINFO	93	/* request info for one table */
#define	IP_FW_TABLE_XFLUSH	94	/* flush table data */
#define	IP_FW_TABLE_XCREATE	95	/* create new table  */
//#define	IP_FW_TABLE_XMODIFY	96	/* modify existing table */
#define	IP_FW_XGET		97	/* Retrieve configuration */
#define	IP_FW_XADD		98	/* add entry */
#define	IP_FW_TABLE_XFIND	99	/* finds an entry */
#define	IP_FW_XIFLIST		100	/* list tracked interfaces */
#define	IP_FW_TABLES_ALIST	101	/* list table algorithms */

/*
 * Usage guidelines:
 *
 * IP_FW_TABLE_XLIST(ver 1): Dumps all table data
 *   Request(getsockopt): [ ipfw_obj_lheader ], size = ipfw_xtable_info.size
 *   Reply: [ ipfw_obj_lheader ipfw_xtable_info ipfw_table_xentry x N ]
 *
 * IP_FW_TABLE_XDESTROY: Destroys given table
 *   Request(setsockopt): [ ipfw_obj_header ]
 *
 * IP_FW_TABLES_XGETSIZE: Get buffer size needed to list info for all tables.
 *   Request(getsockopt): [ empty ], size = sizeof(ipfw_obj_lheader)
 *   Reply: [ ipfw_obj_lheader ]
 *
 * IP_FW_TABLES_XLIST: Lists all tables currently available in kernel.
 *   Request(getsockopt): [ ipfw_obj_lheader ], size = ipfw_obj_lheader.size
 *   Reply: [ ipfw_obj_lheader ipfw_xtable_info x N ]
 *
 * IP_FW_TABLE_XINFO: Store table info to buffer.
 *   Request(getsockopt): [ ipfw_obj_header ipfw_xtable_info(empty)]
 *   Reply: [ ipfw_obj_header ipfw_xtable_info ]
 *
 * IP_FW_TABLE_XFLUSH: Removes all data from given table leaving type etc..
 *   Request(setsockopt): [ ipfw_obj_header ]
 */

/*
 * The kernel representation of ipfw rules is made of a list of
 * 'instructions' (for all practical purposes equivalent to BPF
 * instructions), which specify which fields of the packet
 * (or its metadata) should be analysed.
 *
 * Each instruction is stored in a structure which begins with
 * "ipfw_insn", and can contain extra fields depending on the
 * instruction type (listed below).
 * Note that the code is written so that individual instructions
 * have a size which is a multiple of 32 bits. This means that, if
 * such structures contain pointers or other 64-bit entities,
 * (there is just one instance now) they may end up unaligned on
 * 64-bit architectures, so the must be handled with care.
 *
 * "enum ipfw_opcodes" are the opcodes supported. We can have up
 * to 256 different opcodes. When adding new opcodes, they should
 * be appended to the end of the opcode list before O_LAST_OPCODE,
 * this will prevent the ABI from being broken, otherwise users
 * will have to recompile ipfw(8) when they update the kernel.
 */

enum ipfw_opcodes {		/* arguments (4 byte each)	*/
	O_NOP,

	O_IP_SRC,		/* u32 = IP			*/
	O_IP_SRC_MASK,		/* ip = IP/mask			*/
	O_IP_SRC_ME,		/* none				*/
	O_IP_SRC_SET,		/* u32=base, arg1=len, bitmap	*/

	O_IP_DST,		/* u32 = IP			*/
	O_IP_DST_MASK,		/* ip = IP/mask			*/
	O_IP_DST_ME,		/* none				*/
	O_IP_DST_SET,		/* u32=base, arg1=len, bitmap	*/

	O_IP_SRCPORT,		/* (n)port list:mask 4 byte ea	*/
	O_IP_DSTPORT,		/* (n)port list:mask 4 byte ea	*/
	O_PROTO,		/* arg1=protocol		*/

	O_MACADDR2,		/* 2 mac addr:mask		*/
	O_MAC_TYPE,		/* same as srcport		*/

	O_LAYER2,		/* none				*/
	O_IN,			/* none				*/
	O_FRAG,			/* none				*/

	O_RECV,			/* none				*/
	O_XMIT,			/* none				*/
	O_VIA,			/* none				*/

	O_IPOPT,		/* arg1 = 2*u8 bitmap		*/
	O_IPLEN,		/* arg1 = len			*/
	O_IPID,			/* arg1 = id			*/

	O_IPTOS,		/* arg1 = id			*/
	O_IPPRECEDENCE,		/* arg1 = precedence << 5	*/
	O_IPTTL,		/* arg1 = TTL			*/

	O_IPVER,		/* arg1 = version		*/
	O_UID,			/* u32 = id			*/
	O_GID,			/* u32 = id			*/
	O_ESTAB,		/* none (tcp established)	*/
	O_TCPFLAGS,		/* arg1 = 2*u8 bitmap		*/
	O_TCPWIN,		/* arg1 = desired win		*/
	O_TCPSEQ,		/* u32 = desired seq.		*/
	O_TCPACK,		/* u32 = desired seq.		*/
	O_ICMPTYPE,		/* u32 = icmp bitmap		*/
	O_TCPOPTS,		/* arg1 = 2*u8 bitmap		*/

	O_VERREVPATH,		/* none				*/
	O_VERSRCREACH,		/* none				*/

	O_PROBE_STATE,		/* none				*/
	O_KEEP_STATE,		/* none				*/
	O_LIMIT,		/* ipfw_insn_limit		*/
	O_LIMIT_PARENT,		/* dyn_type, not an opcode.	*/

	/*
	 * These are really 'actions'.
	 */

	O_LOG,			/* ipfw_insn_log		*/
	O_PROB,			/* u32 = match probability	*/

	O_CHECK_STATE,		/* none				*/
	O_ACCEPT,		/* none				*/
	O_DENY,			/* none 			*/
	O_REJECT,		/* arg1=icmp arg (same as deny)	*/
	O_COUNT,		/* none				*/
	O_SKIPTO,		/* arg1=next rule number	*/
	O_PIPE,			/* arg1=pipe number		*/
	O_QUEUE,		/* arg1=queue number		*/
	O_DIVERT,		/* arg1=port number		*/
	O_TEE,			/* arg1=port number		*/
	O_FORWARD_IP,		/* fwd sockaddr			*/
	O_FORWARD_MAC,		/* fwd mac			*/
	O_NAT,                  /* nope                         */
	O_REASS,                /* none                         */
	
	/*
	 * More opcodes.
	 */
	O_IPSEC,		/* has ipsec history 		*/
	O_IP_SRC_LOOKUP,	/* arg1=table number, u32=value	*/
	O_IP_DST_LOOKUP,	/* arg1=table number, u32=value	*/
	O_ANTISPOOF,		/* none				*/
	O_JAIL,			/* u32 = id			*/
	O_ALTQ,			/* u32 = altq classif. qid	*/
	O_DIVERTED,		/* arg1=bitmap (1:loop, 2:out)	*/
	O_TCPDATALEN,		/* arg1 = tcp data len		*/
	O_IP6_SRC,		/* address without mask		*/
	O_IP6_SRC_ME,		/* my addresses			*/
	O_IP6_SRC_MASK,		/* address with the mask	*/
	O_IP6_DST,
	O_IP6_DST_ME,
	O_IP6_DST_MASK,
	O_FLOW6ID,		/* for flow id tag in the ipv6 pkt */
	O_ICMP6TYPE,		/* icmp6 packet type filtering	*/
	O_EXT_HDR,		/* filtering for ipv6 extension header */
	O_IP6,

	/*
	 * actions for ng_ipfw
	 */
	O_NETGRAPH,		/* send to ng_ipfw		*/
	O_NGTEE,		/* copy to ng_ipfw		*/

	O_IP4,

	O_UNREACH6,		/* arg1=icmpv6 code arg (deny)  */

	O_TAG,   		/* arg1=tag number */
	O_TAGGED,		/* arg1=tag number */

	O_SETFIB,		/* arg1=FIB number */
	O_FIB,			/* arg1=FIB desired fib number */
	
	O_SOCKARG,		/* socket argument */

	O_CALLRETURN,		/* arg1=called rule number */

	O_FORWARD_IP6,		/* fwd sockaddr_in6             */

	O_DSCP,			/* 2 u32 = DSCP mask */
	O_SETDSCP,		/* arg1=DSCP value */

	O_LAST_OPCODE		/* not an opcode!		*/
};


/*
 * The extension header are filtered only for presence using a bit
 * vector with a flag for each header.
 */
#define EXT_FRAGMENT	0x1
#define EXT_HOPOPTS	0x2
#define EXT_ROUTING	0x4
#define EXT_AH		0x8
#define EXT_ESP		0x10
#define EXT_DSTOPTS	0x20
#define EXT_RTHDR0		0x40
#define EXT_RTHDR2		0x80

/*
 * Template for instructions.
 *
 * ipfw_insn is used for all instructions which require no operands,
 * a single 16-bit value (arg1), or a couple of 8-bit values.
 *
 * For other instructions which require different/larger arguments
 * we have derived structures, ipfw_insn_*.
 *
 * The size of the instruction (in 32-bit words) is in the low
 * 6 bits of "len". The 2 remaining bits are used to implement
 * NOT and OR on individual instructions. Given a type, you can
 * compute the length to be put in "len" using F_INSN_SIZE(t)
 *
 * F_NOT	negates the match result of the instruction.
 *
 * F_OR		is used to build or blocks. By default, instructions
 *		are evaluated as part of a logical AND. An "or" block
 *		{ X or Y or Z } contains F_OR set in all but the last
 *		instruction of the block. A match will cause the code
 *		to skip past the last instruction of the block.
 *
 * NOTA BENE: in a couple of places we assume that
 *	sizeof(ipfw_insn) == sizeof(u_int32_t)
 * this needs to be fixed.
 *
 */
typedef struct	_ipfw_insn {	/* template for instructions */
	u_int8_t 	opcode;
	u_int8_t	len;	/* number of 32-bit words */
#define	F_NOT		0x80
#define	F_OR		0x40
#define	F_LEN_MASK	0x3f
#define	F_LEN(cmd)	((cmd)->len & F_LEN_MASK)

	u_int16_t	arg1;
} ipfw_insn;

/*
 * The F_INSN_SIZE(type) computes the size, in 4-byte words, of
 * a given type.
 */
#define	F_INSN_SIZE(t)	((sizeof (t))/sizeof(u_int32_t))

/*
 * This is used to store an array of 16-bit entries (ports etc.)
 */
typedef struct	_ipfw_insn_u16 {
	ipfw_insn o;
	u_int16_t ports[2];	/* there may be more */
} ipfw_insn_u16;

/*
 * This is used to store an array of 32-bit entries
 * (uid, single IPv4 addresses etc.)
 */
typedef struct	_ipfw_insn_u32 {
	ipfw_insn o;
	u_int32_t d[1];	/* one or more */
} ipfw_insn_u32;

/*
 * This is used to store IP addr-mask pairs.
 */
typedef struct	_ipfw_insn_ip {
	ipfw_insn o;
	struct in_addr	addr;
	struct in_addr	mask;
} ipfw_insn_ip;

/*
 * This is used to forward to a given address (ip).
 */
typedef struct  _ipfw_insn_sa {
	ipfw_insn o;
	struct sockaddr_in sa;
} ipfw_insn_sa;

/*
 * This is used to forward to a given address (ipv6).
 */
typedef struct _ipfw_insn_sa6 {
	ipfw_insn o;
	struct sockaddr_in6 sa;
} ipfw_insn_sa6;

/*
 * This is used for MAC addr-mask pairs.
 */
typedef struct	_ipfw_insn_mac {
	ipfw_insn o;
	u_char addr[12];	/* dst[6] + src[6] */
	u_char mask[12];	/* dst[6] + src[6] */
} ipfw_insn_mac;

/*
 * This is used for interface match rules (recv xx, xmit xx).
 */
typedef struct	_ipfw_insn_if {
	ipfw_insn o;
	union {
		struct in_addr ip;
		int glob;
	} p;
	char name[IFNAMSIZ];
} ipfw_insn_if;

/*
 * This is used for storing an altq queue id number.
 */
typedef struct _ipfw_insn_altq {
	ipfw_insn	o;
	u_int32_t	qid;
} ipfw_insn_altq;

/*
 * This is used for limit rules.
 */
typedef struct	_ipfw_insn_limit {
	ipfw_insn o;
	u_int8_t _pad;
	u_int8_t limit_mask;	/* combination of DYN_* below	*/
#define	DYN_SRC_ADDR	0x1
#define	DYN_SRC_PORT	0x2
#define	DYN_DST_ADDR	0x4
#define	DYN_DST_PORT	0x8

	u_int16_t conn_limit;
} ipfw_insn_limit;

/*
 * This is used for log instructions.
 */
typedef struct  _ipfw_insn_log {
        ipfw_insn o;
	u_int32_t max_log;	/* how many do we log -- 0 = all */
	u_int32_t log_left;	/* how many left to log 	*/
} ipfw_insn_log;

/*
 * Data structures required by both ipfw(8) and ipfw(4) but not part of the
 * management API are protected by IPFW_INTERNAL.
 */
#ifdef IPFW_INTERNAL
/* Server pool support (LSNAT). */
struct cfg_spool {
	LIST_ENTRY(cfg_spool)   _next;          /* chain of spool instances */
	struct in_addr          addr;
	u_short                 port;
};
#endif

/* Redirect modes id. */
#define REDIR_ADDR      0x01
#define REDIR_PORT      0x02
#define REDIR_PROTO     0x04

#ifdef IPFW_INTERNAL
/* Nat redirect configuration. */
struct cfg_redir {
	LIST_ENTRY(cfg_redir)   _next;          /* chain of redir instances */
	u_int16_t               mode;           /* type of redirect mode */
	struct in_addr	        laddr;          /* local ip address */
	struct in_addr	        paddr;          /* public ip address */
	struct in_addr	        raddr;          /* remote ip address */
	u_short                 lport;          /* local port */
	u_short                 pport;          /* public port */
	u_short                 rport;          /* remote port  */
	u_short                 pport_cnt;      /* number of public ports */
	u_short                 rport_cnt;      /* number of remote ports */
	int                     proto;          /* protocol: tcp/udp */
	struct alias_link       **alink;	
	/* num of entry in spool chain */
	u_int16_t               spool_cnt;      
	/* chain of spool instances */
	LIST_HEAD(spool_chain, cfg_spool) spool_chain;
};
#endif

#ifdef IPFW_INTERNAL
/* Nat configuration data struct. */
struct cfg_nat {
	/* chain of nat instances */
	LIST_ENTRY(cfg_nat)     _next;
	int                     id;                     /* nat id */
	struct in_addr          ip;                     /* nat ip address */
	char                    if_name[IF_NAMESIZE];   /* interface name */
	int                     mode;                   /* aliasing mode */
	struct libalias	        *lib;                   /* libalias instance */
	/* number of entry in spool chain */
	int                     redir_cnt;              
	/* chain of redir instances */
	LIST_HEAD(redir_chain, cfg_redir) redir_chain;  
};
#endif

#define SOF_NAT         sizeof(struct cfg_nat)
#define SOF_REDIR       sizeof(struct cfg_redir)
#define SOF_SPOOL       sizeof(struct cfg_spool)

/* Nat command. */
typedef struct	_ipfw_insn_nat {
 	ipfw_insn	o;
 	struct cfg_nat *nat;	
} ipfw_insn_nat;

/* Apply ipv6 mask on ipv6 addr */
#define APPLY_MASK(addr,mask)                          \
    (addr)->__u6_addr.__u6_addr32[0] &= (mask)->__u6_addr.__u6_addr32[0]; \
    (addr)->__u6_addr.__u6_addr32[1] &= (mask)->__u6_addr.__u6_addr32[1]; \
    (addr)->__u6_addr.__u6_addr32[2] &= (mask)->__u6_addr.__u6_addr32[2]; \
    (addr)->__u6_addr.__u6_addr32[3] &= (mask)->__u6_addr.__u6_addr32[3];

/* Structure for ipv6 */
typedef struct _ipfw_insn_ip6 {
       ipfw_insn o;
       struct in6_addr addr6;
       struct in6_addr mask6;
} ipfw_insn_ip6;

/* Used to support icmp6 types */
typedef struct _ipfw_insn_icmp6 {
       ipfw_insn o;
       uint32_t d[7]; /* XXX This number si related to the netinet/icmp6.h
                       *     define ICMP6_MAXTYPE
                       *     as follows: n = ICMP6_MAXTYPE/32 + 1
                        *     Actually is 203 
                       */
} ipfw_insn_icmp6;

/*
 * Here we have the structure representing an ipfw rule.
 *
 * Layout:
 * struct ip_fw_rule
 * [ counter block, size = rule->cntr_len ]
 * [ one or more instructions, size = rule->cmd_len * 4 ]
 *
 * It starts with a general area (with link fields).
 * Counter block may be next (if rule->cntr_len > 0),
 * followed by an array of one or more instructions, which the code
 * accesses as an array of 32-bit values. rule->cmd_len represents
 * the total instructions legth in u32 worrd, while act_ofs represents
 * rule action offset in u32 words.
 *
 * When assembling instruction, remember the following:
 *
 *  + if a rule has a "keep-state" (or "limit") option, then the
 *	first instruction (at r->cmd) MUST BE an O_PROBE_STATE
 *  + if a rule has a "log" option, then the first action
 *	(at ACTION_PTR(r)) MUST be O_LOG
 *  + if a rule has an "altq" option, it comes after "log"
 *  + if a rule has an O_TAG option, it comes after "log" and "altq"
 *
 *
 * All structures (excluding instructions) are u64-aligned.
 * Please keep this.
 */

struct ip_fw_rule {
	uint16_t	act_ofs;	/* offset of action in 32-bit units */
	uint16_t	cmd_len;	/* # of 32-bit words in cmd	*/
	uint16_t	spare;
	uint8_t		set;		/* rule set (0..31)		*/
	uint8_t		flags;		/* rule flags			*/
	uint32_t	rulenum;	/* rule number			*/
	uint32_t	id;		/* rule id			*/

	ipfw_insn	cmd[1];		/* storage for commands		*/
};
#define	IPFW_RULE_NOOPT		0x01	/* Has no options in body	*/

/* Unaligned version */

/* Base ipfw rule counter block. */
struct ip_fw_bcounter {
	uint16_t	size;		/* Size of counter block, bytes	*/
	uint8_t		flags;		/* flags for given block	*/
	uint8_t		spare;
	uint32_t	timestamp;	/* tv_sec of last match		*/
	uint64_t	pcnt;		/* Packet counter		*/
	uint64_t	bcnt;		/* Byte counter			*/
};


#ifndef	_KERNEL
/*
 * Legacy rule format
 */
struct ip_fw {
	struct ip_fw	*x_next;	/* linked list of rules		*/
	struct ip_fw	*next_rule;	/* ptr to next [skipto] rule	*/
	/* 'next_rule' is used to pass up 'set_disable' status		*/

	uint16_t	act_ofs;	/* offset of action in 32-bit units */
	uint16_t	cmd_len;	/* # of 32-bit words in cmd	*/
	uint16_t	rulenum;	/* rule number			*/
	uint8_t	set;		/* rule set (0..31)		*/
	uint8_t		_pad;		/* padding			*/
	uint32_t	id;		/* rule id */

	/* These fields are present in all rules.			*/
	uint64_t	pcnt;		/* Packet counter		*/
	uint64_t	bcnt;		/* Byte counter			*/
	uint32_t	timestamp;	/* tv_sec of last match		*/

	ipfw_insn	cmd[1];		/* storage for commands		*/
};
#endif

#define ACTION_PTR(rule)				\
	(ipfw_insn *)( (u_int32_t *)((rule)->cmd) + ((rule)->act_ofs) )

#define RULESIZE(rule)  (sizeof(*(rule)) + (rule)->cmd_len * 4 - 4)


#if 1 // should be moved to in.h
/*
 * This structure is used as a flow mask and a flow id for various
 * parts of the code.
 * addr_type is used in userland and kernel to mark the address type.
 * fib is used in the kernel to record the fib in use.
 * _flags is used in the kernel to store tcp flags for dynamic rules.
 */
struct ipfw_flow_id {
	uint32_t	dst_ip;
	uint32_t	src_ip;
	uint16_t	dst_port;
	uint16_t	src_port;
	uint8_t		fib;
	uint8_t		proto;
	uint8_t		_flags;	/* protocol-specific flags */
	uint8_t		addr_type; /* 4=ip4, 6=ip6, 1=ether ? */
	struct in6_addr dst_ip6;
	struct in6_addr src_ip6;
	uint32_t	flow_id6;
	uint32_t	extra; /* queue/pipe or frag_id */
};
#endif

#define IS_IP6_FLOW_ID(id)	((id)->addr_type == 6)

/*
 * Dynamic ipfw rule.
 */
typedef struct _ipfw_dyn_rule ipfw_dyn_rule;

struct _ipfw_dyn_rule {
	ipfw_dyn_rule	*next;		/* linked list of rules.	*/
	struct ip_fw *rule;		/* pointer to rule		*/
	/* 'rule' is used to pass up the rule number (from the parent)	*/

	ipfw_dyn_rule *parent;		/* pointer to parent rule	*/
	u_int64_t	pcnt;		/* packet match counter		*/
	u_int64_t	bcnt;		/* byte match counter		*/
	struct ipfw_flow_id id;		/* (masked) flow id		*/
	u_int32_t	expire;		/* expire time			*/
	u_int32_t	bucket;		/* which bucket in hash table	*/
	u_int32_t	state;		/* state of this rule (typically a
					 * combination of TCP flags)
					 */
	u_int32_t	ack_fwd;	/* most recent ACKs in forward	*/
	u_int32_t	ack_rev;	/* and reverse directions (used	*/
					/* to generate keepalives)	*/
	u_int16_t	dyn_type;	/* rule type			*/
	u_int16_t	count;		/* refcount			*/
};

/*
 * Definitions for IP option names.
 */
#define	IP_FW_IPOPT_LSRR	0x01
#define	IP_FW_IPOPT_SSRR	0x02
#define	IP_FW_IPOPT_RR		0x04
#define	IP_FW_IPOPT_TS		0x08

/*
 * Definitions for TCP option names.
 */
#define	IP_FW_TCPOPT_MSS	0x01
#define	IP_FW_TCPOPT_WINDOW	0x02
#define	IP_FW_TCPOPT_SACK	0x04
#define	IP_FW_TCPOPT_TS		0x08
#define	IP_FW_TCPOPT_CC		0x10

#define	ICMP_REJECT_RST		0x100	/* fake ICMP code (send a TCP RST) */
#define	ICMP6_UNREACH_RST	0x100	/* fake ICMPv6 code (send a TCP RST) */

/*
 * These are used for lookup tables.
 */

#define	IPFW_TABLE_CIDR		1	/* Table for holding IPv4/IPv6 prefixes */
#define	IPFW_TABLE_INTERFACE	2	/* Table for holding interface names */
#define	IPFW_TABLE_NUMBER	3	/* Table for holding ports/uid/gid/etc */
#define	IPFW_TABLE_MAXTYPE	3	/* Maximum valid number */

#define	IPFW_VTYPE_U32		1	/* Skipto/tablearg integer */
#define	IPFW_VTYPE_IP		2	/* Nexthop IP address */
#define	IPFW_VTYPE_DSCP		3	/* DiffServ codepoints */

typedef struct	_ipfw_table_entry {
	in_addr_t	addr;		/* network address		*/
	u_int32_t	value;		/* value			*/
	u_int16_t	tbl;		/* table number			*/
	u_int8_t	masklen;	/* mask length			*/
} ipfw_table_entry;

typedef struct	_ipfw_table_xentry {
	uint16_t	len;		/* Total entry length		*/
	uint8_t		type;		/* entry type			*/
	uint8_t		masklen;	/* mask length			*/
	uint16_t	tbl;		/* table number			*/
	uint16_t	flags;		/* record flags			*/
	uint32_t	value;		/* value			*/
	union {
		/* Longest field needs to be aligned by 4-byte boundary	*/
		struct in6_addr	addr6;	/* IPv6 address 		*/
		char	iface[IF_NAMESIZE];	/* interface name	*/
	} k;
} ipfw_table_xentry;
#define	IPFW_TCF_INET	0x01		/* CIDR flags: IPv4 record	*/

typedef struct	_ipfw_table {
	u_int32_t	size;		/* size of entries in bytes	*/
	u_int32_t	cnt;		/* # of entries			*/
	u_int16_t	tbl;		/* table number			*/
	ipfw_table_entry ent[0];	/* entries			*/
} ipfw_table;

typedef struct	_ipfw_xtable {
	ip_fw3_opheader	opheader;	/* IP_FW3 opcode */
	uint32_t	size;		/* size of entries in bytes	*/
	uint32_t	cnt;		/* # of entries			*/
	uint16_t	tbl;		/* table number			*/
	uint8_t		type;		/* table type			*/
	ipfw_table_xentry xent[0];	/* entries			*/
} ipfw_xtable;

typedef struct  _ipfw_obj_tlv {
	uint16_t        type;		/* TLV type */
	uint16_t	flags;		/* TLV-specific flags		*/
	uint32_t        length;		/* Total length, aligned to u64	*/
} ipfw_obj_tlv;
#define	IPFW_TLV_TBL_NAME	1
#define	IPFW_TLV_TBLNAME_LIST	2
#define	IPFW_TLV_RULE_LIST	3
#define	IPFW_TLV_DYNSTATE_LIST	4
#define	IPFW_TLV_TBL_ENT	5
#define	IPFW_TLV_DYN_ENT	6
#define	IPFW_TLV_RULE_ENT	7
#define	IPFW_TLV_TBLENT_LIST	8

/* Object name TLV */
typedef struct _ipfw_obj_ntlv {
	ipfw_obj_tlv	head;		/* TLV header			*/
	uint16_t	idx;		/* Name index			*/
	uint8_t		spare;		/* unused			*/
	uint8_t		type;		/* object type, if applicable	*/
	uint32_t	set;		/* set, if applicable		*/
	char		name[64];	/* Null-terminated name		*/
} ipfw_obj_ntlv;

/* Table entry TLV */
typedef struct	_ipfw_obj_tentry {
	ipfw_obj_tlv	head;		/* TLV header			*/
	uint8_t		subtype;	/* subtype (IPv4,IPv6)		*/
	uint8_t		masklen;	/* mask length			*/
	uint16_t	idx;		/* Table name index		*/
	uint32_t	value;		/* value			*/
	uint64_t	spare;
	union {
		/* Longest field needs to be aligned by 8-byte boundary	*/
		struct in_addr addr;	/* IPv4 address			*/
		uint32_t key;		/* uid/gid/port			*/
		struct in6_addr	addr6;	/* IPv6 address 		*/
		char	iface[IF_NAMESIZE];	/* interface name	*/
	} k;
} ipfw_obj_tentry;
#define	IPFW_TF_UPDATE	0x01		/* Update record if exists	*/

typedef struct _ipfw_obj_dyntlv {
	ipfw_obj_tlv	head;
	ipfw_dyn_rule	state;
} ipfw_obj_dyntlv;
#define	IPFW_DF_LAST	0x01		/* Last state in chain		*/

/* Containter TLVs */
typedef struct _ipfw_obj_ctlv {
	ipfw_obj_tlv	head;		/* TLV header			*/
	uint32_t	count;		/* Number of sub-TLVs		*/
	uint16_t	objsize;	/* Single object size		*/
	uint8_t		version;	/* TLV version			*/
	uint8_t		spare;
} ipfw_obj_ctlv;

typedef struct _ipfw_xtable_info {
	uint8_t		type;		/* table type (cidr,iface,..)	*/
	uint8_t		ftype;		/* table value format type	*/
	uint8_t		vtype;		/* value type			*/
	uint16_t	spare0;
	uint32_t	set;		/* set table is in		*/
	uint32_t	kidx;		/* kernel index			*/
	uint32_t	refcnt;		/* number of references		*/
	uint32_t	count;		/* Number of records		*/
	uint32_t	size;		/* Total size of records	*/
	char		tablename[64];	/* table name */
	char		algoname[32];	/* algorithm name		*/
} ipfw_xtable_info;

typedef struct _ipfw_iface_info {
	char		ifname[64];	/* interface name		*/
	uint32_t	ifindex;	/* interface index		*/
	uint32_t	flags;		/* flags			*/
	uint32_t	refcnt;		/* number of references		*/
	uint32_t	gencnt;		/* number of changes		*/
	uint64_t	spare;
} ipfw_iface_info;
#define	IPFW_IFFLAG_RESOLVED	0x01	/* Interface exists		*/

typedef struct _ipfw_ta_info {
	char		algoname[32];	/* algorithm name		*/
	uint32_t	type;		/* lookup type			*/
	uint32_t	flags;
	uint32_t	refcnt;
	uint32_t	spare0;
	uint64_t	spare1;
} ipfw_ta_info;

#define	IPFW_OBJTYPE_TABLE	1
typedef struct _ipfw_obj_header {
	ip_fw3_opheader	opheader;	/* IP_FW3 opcode		*/
	uint32_t	spare;
	uint16_t	idx;		/* object name index		*/
	uint8_t		objtype;	/* object type			*/
	uint8_t		objsubtype;	/* object subtype		*/
	ipfw_obj_ntlv	ntlv;		/* object name tlv		*/
} ipfw_obj_header;

typedef struct _ipfw_obj_lheader {
	ip_fw3_opheader	opheader;	/* IP_FW3 opcode		*/
	uint32_t	set_mask;	/* disabled set mask		*/
	uint32_t	count;		/* Total objects count		*/
	uint32_t	size;		/* Total size (incl. header)	*/
	uint32_t	objsize;	/* Size of one object		*/
} ipfw_obj_lheader;

#define	IPFW_CFG_GET_STATIC	0x01
#define	IPFW_CFG_GET_STATES	0x02
#define	IPFW_CFG_GET_COUNTERS	0x04
typedef struct _ipfw_cfg_lheader {
	ip_fw3_opheader	opheader;	/* IP_FW3 opcode		*/
	uint32_t	set_mask;	/* enabled set mask		*/
	uint32_t	spare;
	uint32_t	flags;		/* Request flags		*/
	uint32_t	size;		/* neded buffer size		*/
	uint32_t	start_rule;
	uint32_t	end_rule;
} ipfw_cfg_lheader;

#endif /* _IPFW2_H */
