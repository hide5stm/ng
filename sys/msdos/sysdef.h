/* $Id: sysdef.h,v 1.1 2000/06/27 01:47:58 amura Exp $ */
/*
 *		MS-DOS based systems
 */

/*
 * $Log: sysdef.h,v $
 * Revision 1.1  2000/06/27 01:47:58  amura
 * Initial revision
 *
 */

#include <stdio.h>
#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
#include <mem.h>
#else	/* NOT __TURBOC__ */
#include <memory.h>			/* need to use memmove().	*/
#endif	/* __TURBOC__ */

#define	KBLOCK	1024			/* Kill grow.			*/
#define	GOOD	0			/* Good exit status.		*/
#define	MAXPATH	256			/* Maximum length of path for chdir */
#ifndef	NO_SHELL	/* 91.01.10  by K.Maeda */
#define	CMDLINELENGTH	80		/* Maximum length of shell command. */
#endif	/* NO_SHELL */
#define	NO_RESIZE			/* Screen size is constant.	*/
#define	BSMAP	TRUE			/* Bs map feature can use.	*/
					/* (default mode is bsmap on)	*/
#define	MAKEBACKUP	FALSE		/* Making backup file is off.	*/
#ifdef	__TURBOC__	/* 90.03.23  by A.Shirahashi */
#define	LOCAL_VARARGS
#endif	/* __TURBOC__ */

#ifdef	DO_METAKEY
#ifndef METABIT
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
#define METABIT 0x100
#else	/* NOT KANJI */
#define METABIT 0x80
#endif	/* KANJI */
#endif	/* METABIT */
#endif	/* DO_METAKEY */

typedef long	RSIZE;			/* Type for file/region sizes	*/
typedef short	KCHAR;			/* Type for internal keystrokes	*/

/*
 * Macros used by the buffer name making code.
 * Start at the end of the file name, scan to the left
 * until BDC1 (or BDC2, if defined) is reached. The buffer
 * name starts just to the right of that location, and
 * stops at end of string (or at the next BDC3 character,
 * if defined). BDC2 and BDC3 are mainly for VMS.
 */
#define	BDC1	'\\'			/* Buffer names.		*/

#define MALLOCROUND(m)	(m+=7,m&=~7)	/* round up to 8 byte boundry	*/

#define	bcopy(s,d,n)	memmove(d,s,n)	/* copy memory area.		*/
#define	fncmp		strcmp		/* file name comparison		*/
#define	unlinkdir(fn)	rmdir(fn)	/* unlink directory		*/
char *getenv();
#define	gettermtype()	getenv("TERM")	/* determine terminal type	*/