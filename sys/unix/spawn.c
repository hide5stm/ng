/* $Id: spawn.c,v 1.8 2003/02/22 08:09:47 amura Exp $ */
/*
 *	Spawn. (for configure)
 * This interracts with the job control stuff
 * in the 4.X BSD C shell and SysVR4.
 * By:	       rtech!daveb, to use stop for ksh.
 *
 * Create from BSD UNIX version by amura, 2000
 */

#include "config.h"
#include "def.h"

#include <signal.h>
#include <string.h>
#ifdef	HAVE_VFORK_H
#include <vfork.h>
#endif
#ifdef	HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

/* some system don't have job-control even if SIGTSTP is defined... */
#ifdef	UNDEF_SIGTSTP
# ifdef SIGTSTP
#  undef SIGTSTP
# endif
#endif

#ifndef HAVE_RINDEX
#define rindex(s,c)	strrchr(s,c)
#endif
static char *shellp = NULL;		/* Saved "SHELL" name.		*/
static char *shname;

/*
 * This code does a one of 2 different
 * things, depending on what version of the shell
 * you are using. If you are using the C shell, which
 * implies that you are using job control, then MicroEMACS
 * moves the cursor to a nice place and sends itself a
 * stop signal. If you are using the Bourne shell it runs
 * a subshell using fork/exec. Bound to "C-C", and used
 * as a subcommand by "C-Z".
 *
 * Daveb -- changed sense of test so that we only spawn if you
 *	    are explicitly using /bin/sh.  This makes it stop
 *	    work with the ksh.
 */
/*ARGSUSED*/
int
spawncli(f, n)
int f, n;
{
    register pid_t pid, wpid;
#ifdef	SIGTSTP
#ifdef	HAVE_SIGPROCMASK
    sigset_t omask, newmask;
#else
    register int omask;
#endif
#endif	/* SIGTSTP */
    register RETSIGTYPE	(*oqsig)(), (*oisig)();
#ifdef	SIGWINCH	/* 90.02.14  by S.Yoshida */
    register RETSIGTYPE	(*owsig)();
#endif	/* SIGWINCH */
    int status;
    int ttcooked _PRO((void)), ttraw _PRO((void));
    int refresh _PRO((int, int));

#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
    ttykeymaptidy();
#endif /* XKEYS */
    if (shellp == NULL) {
	shellp = getenv("SHELL");
	if (shellp == NULL)
	    shellp = getenv("shell");
	if (shellp == NULL)
	    shellp = "/bin/sh";	/* Safer.		*/
	shname = rindex( shellp, '/' ); 
	shname = shname ? shname+1 : shellp;
    }
    ttcolor(CTEXT);
    ttnowindow();
    if (strcmp(shellp, "/bin/csh") == 0) {
	if (epresf != FALSE) {
	    ttmove(nrow-1, 0);
	    tteeol();
	    epresf = FALSE;
	}				/* Csh types a "\n"	*/
	ttmove(nrow-2, 0);		/* before "Stopped".	*/
    }
    else {
	ttmove(nrow-1, 0);
	if (epresf != FALSE) {
	    tteeol();
	    epresf = FALSE;
	}
    }
    if (ttcooked() == FALSE)
	return (FALSE);
#ifdef SIGTSTP
    if (strcmp(shellp, "/bin/sh")!=0 ||
	getenv("BASH_VERSION") || getenv("BASH")) {
	/* C shell, ksh	or bash	*/
#ifdef	HAVE_SIGPROCMASK
	sigfillset(&newmask);
	sigprocmask(SIG_UNBLOCK, &newmask, &omask);
	kill(0, SIGTSTP);
	sigprocmask(SIG_SETMASK, &omask, NULL);
#else
	omask = sigsetmask(0);
	kill(0, SIGTSTP);
	sigsetmask(omask);
#endif
#ifdef	SIGWINCH	/* 90.02.14  by S.Yoshida */
	refresh(FFRAND, 0);		/* May be resized.	*/
#endif	/* SIGWINCH */
    }
    else {				/* Bourne shell.	*/
#endif	/* SIGTSTP */
	oqsig = signal(SIGQUIT, SIG_IGN);
	oisig = signal(SIGINT,	SIG_IGN);
#ifdef	SIGWINCH
	owsig = signal(SIGWINCH, SIG_IGN);
#endif
	if ((pid=vfork()) < 0) {
	    signal(SIGQUIT, oqsig);
	    signal(SIGINT,  oisig);
#ifdef	SIGWINCH
	    signal(SIGWINCH, owsig);
#endif
	    ewprintf("Failed to create process");
	    return (FALSE);
	}
	if (pid == 0) {
#ifdef	EXTD_DIR
	    dirend();
#endif
	    execl(shellp, shname, "-i", NULL);
	    _exit(0);		/* Should do better!	*/
	}
	while ((wpid=wait((int*)&status))>=0 && wpid!=pid)
	    ;
	signal(SIGQUIT, oqsig);
	signal(SIGINT,  oisig);
#ifdef	SIGWINCH
	(void) signal(SIGWINCH, owsig);
	refresh(FFRAND, 0);		/* May be resized.	*/
#endif
#ifdef	SIGTSTP
    }
#endif
    sgarbf = TRUE;				/* Force repaint.	*/
#ifdef XKEYS  /* 92.03.16 by Gen KUROKI */
    ttykeypadstart();
#endif /* XKEYS */
    return ttraw();
}

#ifndef NO_SHELL	/* 91.01.10  by K.Maeda */
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifndef S_IRUSR		/* for old system compat? */
#define S_IRUSR		S_IREAD
#define S_IWUSR		S_IWRITE
#endif

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
    char buf[CMDLINELENGTH];
    static char tmpbuf[20];
    char *tmp;
    int ostdin, ostdout, ostderr, in, out, s;
#ifndef	HAVE_MKSTEMP
    extern char *mktemp();
#endif
    
    strcpy(tmpbuf, "/tmp/ngXXXXXX");
    
    if ((in = open(input ? input : "/dev/null", 0)) < 0)
	return NULL;
#ifdef HAVE_MKSTEMP	
    if ((out = mkstemp(tmpbuf)) < 0) {
	close(in);
	return NULL;
    }
    tmp = tmpbuf;
#else
    if ((tmp = mktemp(tmpbuf)) == NULL) {
	close(in);
	return NULL;
    }
    if ((out = creat(tmp, S_IRUSR | S_IWUSR)) < 0) {
	close(in);
	return NULL;
    }
#endif
    
    ostdin = dup(0); ostdout = dup(1); ostderr = dup(2);
    if (ostdin < 0 || ostdout < 0 || ostderr < 0) {
	s = -1;
	goto skip;
    }
#ifdef	HAVE_DUP2
    dup2(in, 0);
    dup2(out, 1);
    dup2(out, 2);
#else
    close(0); dup(in);
    close(1); dup(out);
    close(2); dup(out);
#endif
    strncpy(buf, command, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
#ifdef	EXTD_DIR
    ensurecwd();
#endif
    s = system(buf);
    close(in);
    close(out);
#ifdef	HAVE_DUP2
    dup2(ostdin, 0); dup2(ostdout, 1); dup2(ostderr, 2);
#else
    close(0); dup(ostdin);
    close(1); dup(ostdout);
    close(2); dup(ostderr);
#endif
skip:
    close(ostdin); close(ostdout); close(ostderr);
    if (s == -1) {
	unlink(tmp);
	return NULL;
    }
    return tmp;
}
#endif	/* NO_SHELL */
