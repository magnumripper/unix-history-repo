/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)isa.h	5.7 (Berkeley) 5/9/91
 *	$Id: pc98.h,v 1.2 1996/09/03 10:23:48 asami Exp $
 */

#ifndef _PC98_PC98_PC98_H_
#define	_PC98_PC98_PC98_H_

/* BEWARE:  Included in both assembler and C code */

/*
 * PC98 Bus conventions
 */
/*
 * PC98 Bus conventions
 * modified for PC9801 by A.Kojima F.Ukai M.Ishii 
 *			Kyoto University Microcomputer Club (KMC)
 */

/*
 * Input / Output Port Assignments
 */

#ifndef IO_ISABEGIN
#define	IO_ISABEGIN	0x000		/* 0x000 - Beginning of I/O Registers */

/* PC98 IO address ... very dirty (^_^; */

#define IO_ICU1		0x000		/* 8259A Interrupt Controller #1 */
#define IO_DMA		0x001		/* 8237A DMA Controller */
#define IO_ICU2		0x008		/* 8259A Interrupt Controller #2 */
#define IO_RTC		0x020		/* 4990A RTC */
#define IO_DMAPG	0x021		/* DMA Page Registers */
#define IO_COM1		0x030		/* 8251A RS232C serial I/O (int) */
#define IO_SYSPORT	0x031		/* 8255A System Port */
#define IO_PPI		0x035		/* Programmable Peripheral Interface */
#define IO_LPT		0x040		/* 8255A Printer Port */
#define IO_KBD		0x041		/* 8251A Keyboard */
#define IO_NMI		0x050		/* NMI Control */
#define IO_WAIT		0x05F		/* WAIT 0.6 us */
#define IO_GDC1		0x060		/* 7220 GDC Text Control */
#define IO_TIMER	0x071		/* 8253C Timer */
#define IO_SASI		0x080		/* SASI Hard Disk Controller */
#define IO_FD1		0x090		/* 765A 1MB FDC */
#define	IO_GDC2		0x0a0		/* 7220 GDC Graphic Control */
#define	IO_CGROM	0x0a1		/* Character ROM */
#define	IO_COM2		0x0b1		/* 8251A RS232C serial I/O (ext) */
#define	IO_COM3		0x0b9		/* 8251A RS232C serial I/O (ext) */
#define IO_FDPORT	0x0be		/* FD I/F port (1M<->640K,EMTON) */
#define IO_FD2		0x0c8		/* 765A 640KB FDC */
#define IO_SIO1		0x0d0		/* MC16550II ext RS232C */
#define IO_REEST	0x0F0		/* CPU FPU reset */
#define IO_A2OEN	0x0F2		/* A20 enable */
#define IO_A20CT	0x0F6		/* A20 control enable/disable */
#define IO_NPX		0x0F8		/* Numeric Coprocessor */
#define	IO_SOUND	0x188		/* YM2203 FM sound board */
#define	IO_EGC		0x4a0		/* 7220 GDC Graphic Control */
#define IO_SCSI		0xcc0		/* SCSI Controller */
#define IO_SIO2		0x8d0		/* MC16550II ext RS232C */
#define IO_BEEPF	0x3fdb		/* beep frequency */
#define IO_MOUSE	0x7fd9		/* mouse */
#define IO_BMS		0x7fd9		/* Bus Mouse */
#define IO_MSE		0x7fd9		/* Bus Mouse */
#define IO_MOUSETM	0xdfbd		/* mouse timer */

#define IO_WD1_NEC	0x640		/* 98note IDE Hard disk controller */
#define IO_WD1_EPSON	0x80		/* 386note Hard disk controller */
#define	IO_WD1		IO_WD1_NEC	/* IDE Hard disk controller */

#define	IO_ISAEND	0xFFFF		/* - 0x3FF End of I/O Registers */
#endif /* !IO_ISABEGIN */

/*
 * Input / Output Port Sizes - these are from several sources, and tend
 * to be the larger of what was found, ie COM ports can be 4, but some
 * boards do not fully decode the address, thus 8 ports are used.
 */

#ifndef	IO_ISASIZES
#define	IO_ISASIZES

#define	IO_COMSIZE	8		/* 8250, 16X50 com controllers (4?) */
#define	IO_CGASIZE	16		/* CGA controllers */
#define	IO_DMASIZE	16		/* 8237 DMA controllers */
#define	IO_DPGSIZE	32		/* 74LS612 DMA page registers */
#define	IO_FDCSIZE	8		/* Nec765 floppy controllers */
#define	IO_WDCSIZE	8		/* WD compatible disk controllers */
#define	IO_GAMSIZE	16		/* AT compatible game controllers */
#define	IO_ICUSIZE	16		/* 8259A interrupt controllers */
#define	IO_KBDSIZE	16		/* 8042 Keyboard controllers */
#define	IO_LPTSIZE	8		/* LPT controllers, some use only 4 */
#define	IO_MDASIZE	16		/* Monochrome display controllers */
#define	IO_RTCSIZE	16		/* CMOS real time clock, NMI control */
#define	IO_TMRSIZE	16		/* 8253 programmable timers */
#define	IO_NPXSIZE	16		/* 80387/80487 NPX registers */
#define	IO_VGASIZE	16		/* VGA controllers */
#define IO_EISASIZE	4096		/* EISA controllers */
#define	IO_PMPSIZE	2		/* 82347 power management peripheral */

#endif /* !IO_ISASIZES */

/*
 * Input / Output Memory Physical Addresses
 */

#ifndef	IOM_BEGIN
#define	IOM_BEGIN	0x0a0000		/* Start of I/O Memory "hole" */
#define	IOM_END		0x100000		/* End of I/O Memory "hole" */
#define	IOM_SIZE	(IOM_END - IOM_BEGIN)
#endif /* !RAM_BEGIN */

/*
 * RAM Physical Address Space (ignoring the above mentioned "hole")
 */

#ifndef	RAM_BEGIN
#define	RAM_BEGIN	0x0000000	/* Start of RAM Memory */
#ifdef	EPSON_BOUNCEDMA
#define	RAM_END		0x0f00000	/* End of EPSON GR?? RAM Memory */
#else
#define	RAM_END		0x1000000	/* End of RAM Memory */
#endif
#define	RAM_SIZE	(RAM_END - RAM_BEGIN)
#endif /* !RAM_BEGIN */

#ifndef PC98 /* IBM-PC */
/*
 * Oddball Physical Memory Addresses
 */
#ifndef	COMPAQ_RAMRELOC
#define	COMPAQ_RAMRELOC	0x80c00000	/* Compaq RAM relocation/diag */
#define	COMPAQ_RAMSETUP	0x80c00002	/* Compaq RAM setup */
#define	WEITEK_FPU	0xC0000000	/* WTL 2167 */
#define	CYRIX_EMC	0xC0000000	/* Cyrix EMC */
#endif	COMPAQ_RAMRELOC
#endif

#define PC98_VECTOR_SIZE			(0x400)
#define PC98_SYSTEM_PARAMETER_SIZE		(0x230)

#define PC98_SAVE_AREA(highreso_flag)	(0xa1000)
#define PC98_SAVE_AREA_ADDRESS		(0x10)

#define OFS_BOOT_boothowto 0x210
#define OFS_BOOT_bootdev   0x214
#define OFS_BOOT_cyloffset 0x218
#define OFS_WD_BIOS_SECSIZE(i)	(0x200+(i)*6)
#define OFS_WD_BIOS_NCYL(i) (0x202+(i)*6)
#define OFS_WD_BIOS_HEAD(i) (0x205+(i)*6)
#define OFS_WD_BIOS_SEC(i) (0x204+(i)*6)
#define OFS_pc98_machine_type 0x220
#define OFS_epson_machine_id	0x224
#define OFS_epson_bios_id	0x225
#define OFS_epson_system_type	0x226

#define	M_NEC_PC98	0x0001
#define	M_EPSON_PC98	0x0002
#define	M_NOT_H98	0x0010
#define	M_H98		0x0020
#define	M_NOTE		0x0040
#define	M_NORMAL	0x1000
#define	M_HIGHRESO	0x2000
#define	M_8M		0x8000

#if defined(KERNEL) && !defined(LOCORE)
/* BIOS parameter block */
extern unsigned char	pc98_system_parameter[]; /* in locore.c */
#define PC98_SYSTEM_PARAMETER(x)	pc98_system_parameter[(x)-0x400]
#define BOOT_boothowto (*(unsigned long*)(&pc98_system_parameter[OFS_BOOT_boothowto]))
#define BOOT_bootdev   (*(unsigned long*)(&pc98_system_parameter[OFS_BOOT_bootdev]))
#define BOOT_cyloffset (*(unsigned long*)(&pc98_system_parameter[OFS_BOOT_cyloffset]))
#define WD_BIOS_SECSIZE(i) (*(unsigned short*)(&pc98_system_parameter[OFS_WD_BIOS_SECSIZE(i)]))
#define WD_BIOS_NCYL(i) (*(unsigned short*)(&pc98_system_parameter[OFS_WD_BIOS_NCYL(i)]))
#define WD_BIOS_HEAD(i) (pc98_system_parameter[OFS_WD_BIOS_HEAD(i)])
#define WD_BIOS_SEC(i) (pc98_system_parameter[OFS_WD_BIOS_SEC(i)])
#define pc98_machine_type (*(unsigned long*)&pc98_system_parameter[OFS_pc98_machine_type])
#define epson_machine_id	(pc98_system_parameter[OFS_epson_machine_id])
#define epson_bios_id		(pc98_system_parameter[OFS_epson_bios_id])
#define epson_system_type	(pc98_system_parameter[OFS_epson_system_type])

# define PC98_TYPE_CHECK(x)	((pc98_machine_type & (x)) == (x))
#endif /* KERNEL */

/*
 * Obtained from NetBSD/pc98
 */
#define MADDRUNK -1

#endif /* !_PC98_PC98_PC98_H_ */
