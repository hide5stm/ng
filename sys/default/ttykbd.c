/* $Id: ttykbd.c,v 1.4 2003/02/22 08:09:47 amura Exp $ */
/*
 * Name:	MG 2a
 *		Termcap keyboard driver using key files
 * Created:	22-Nov-1987 Mic Kaczmarczik (mic@emx.cc.utexas.edu)
 */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

#ifdef	XKEYS
/*
 * Get keyboard character.  Very simple if you use keymaps and keys files.
 * Bob was right -- the old XKEYS code is not the right solution.
 * FKEYS code is not usefull other than to help debug FKEYS code in
 * extend.c.
 */

#ifdef FKEYS
char *keystrings[] = { NULL };
#endif

/*
 * Turn on function keys using KS, then load a keys file, if available.
 * The keys file is located in the same manner as the startup file is,
 * depending on what startupfile() does on your system.
 */
extern int ttputc _PRO((int));

VOID
#ifdef ADDOPT
ttykeymapinit(ngrcfile)
char *ngrcfile;
#else
ttykeymapinit()
#endif
{
#ifndef WITHOUT_TERMCAP
    extern char *KS;
#endif
#ifndef	NO_STARTUP
    char *cp, *startupfile();

    if (cp = gettermtype()) {
#ifdef ADDOPT
	if (((cp = startupfile(ngrcfile, cp)) != NULL)
#else
	if (((cp = startupfile(cp)) != NULL)
#endif
	    && (load(cp) != TRUE))
	ewprintf("Error reading key initialization file");
    }
#endif
}

/*
 * Start keypad mode -- called by update() and spawncli()
 */
VOID
ttykeypadstart()
{
#ifndef	WITHOUT_TERMCAP
    extern char *KS;
    if (KS && *KS)			/* turn on keypad	*/
	putpad(KS, 1);
#endif
}

/*
 * Clean up the keyboard -- called by tttidy() and spawncli()
 */
VOID
ttykeymaptidy()
{
#ifndef	WITHOUT_TERMCAP
    extern char *KE;
    if (KE && *KE)
	putpad(KE, 1);	/* turn off keypad		*/
#endif
}

#endif /* XKEYS */
