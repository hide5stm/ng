/* $Id: spawn.c,v 1.6 2003/02/22 08:09:47 amura Exp $ */
/*
 * Name:	MG
 * Version:	2x
 *		Spawn an AmigaDOS subprocess
 * Last edit:	05-Sep-1987
 * By:		...!ihnp4!seismo!ut-sally!ut-ngp!mic
 */

#include <libraries/dos.h>
#include <libraries/dosextens.h>
#undef TRUE
#undef FALSE
#include "config.h"	/* Dec. 15, 1992 by H.Ohkubo */
#include "def.h"	/* AFTER system files to avoid redef's */
#ifdef INLINE_PRAGMAS
#include <pragmas/dos_pragmas.h>
#include <pragmas/exec_pragmas.h>
#else
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#endif

extern struct DOSBase *DOSBase;

/*
 * Create a subjob with a copy of the command intrepreter in it.
 * This is really a way to get a new copy of the CLI, because
 * we don't wait around for the new process to quit.  Note the use
 * of a file handle to nil: to avoid the "endcli" message going out
 * to Emacs's standard output.
 */
int
spawncli(f, n)
int f, n;
{
    BPTR nil;
    
    ewprintf("[Starting new CLI]");
    nil = Open("NIL:", MODE_NEWFILE);
    if (nil == (BPTR)0) { /* highly unlikely */
	ewprintf("Can't create nil file handle");
	return (FALSE);
    }
#ifdef	EXTD_DIR
    ensurecwd();
#endif
    Execute("NEWCLI \"CON:0/0/640/200/MicroEmacs Subprocess\"", nil, nil);
    Close(nil);
    return (TRUE);
}


/* Dec.19,1992 Add by H.Ohkubo */
/* from spawn.c in SYSV. Modified for AmigaDos */
#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
/*
 *	Call process in subshell.
 * Execute COMMAND binding standard input to file INPUT.
 * NULL as INPUT means standard input should be bound to
 * /dev/null or whatever equivalent in your OS.
 * All output during the execution (including standard error output)
 * should go into a scratch file, whose name call_process() returns.
 * Return value NULL means error in some stage of the execution.
 * In that case, scratch file should be deleted.
 */
char *
call_process(command, input)
char *command;
char *input;
{
    char *tmp;
    BPTR in, out;
    extern char *mktemp _PRO((char *));
    
    if ((tmp = mktemp("RAM:ngXXX.XXX")) == NULL)
	return NULL;
    if ((in = Open(input ? input : "NIL:", MODE_NEWFILE)) == 0L)
	return NULL;
    if ((out = Open(tmp, MODE_NEWFILE)) == 0L) {
	Close(in);
	return NULL;
    }
    
#ifdef	EXTD_DIR
    ensurecwd();
#endif
    Execute(command, in, out);
    Close(in);
    Close(out);
    
    return tmp;
}
#endif	/* NO_SHELL */
