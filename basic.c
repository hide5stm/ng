/* $Id: basic.c,v 1.12 2003/03/08 01:27:05 amura Exp $ */
/*
 *		Basic cursor motion commands.
 *
 * The routines in this file are the basic
 * command functions for moving the cursor around on
 * the screen, setting mark, and swapping dot with
 * mark. Only moves between lines, which might make the
 * current buffer framing bad, are hard.
 */
/* 90.01.29	Modified for Ng 1.0 by S.Yoshida */

#include "config.h"	/* 90.12.20  by S.Yoshida */
#include "def.h"

VOID setgoal();

/*
 * Go to beginning of line.
 */
/*ARGSUSED*/
int
gotobol(f, n)
int f, n;
{
    curwp->w_doto  = 0;
    return (TRUE);
}

/*
 * Move cursor backwards. Do the
 * right thing if the count is less than
 * 0. Error if you try to move back from
 * the beginning of the buffer.
#ifdef	KANJI
 * (91.01.01  Add comment by S.Yoshida)
 * When after moving "n" bytes, here is KANJI 2nd byte,
 * go to the KANJI 1st byte of that char.
#endif
 */
/*ARGSUSED*/
int
backchar(f, n)
int f;
register int n;
{
    register LINE *lp;
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    register int kanji2nd = 0;	/* Now on a KANJI 2nd byte. */
#endif /* KANJI */ 
   if (n < 0)
	return forwchar(f, -n);
    while (n--) {
	if (curwp->w_doto == 0) {
	    if ((lp=lback(curwp->w_dotp)) == curbp->b_linep) {
		if (!(f & FFRAND))
		    ewprintf("Beginning of buffer");
		return (FALSE);
	    }
	    curwp->w_dotp  = lp;
	    curwp->w_doto  = llength(lp);
	    curwp->w_flag |= WFMOVE;
	}
	else {
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
	    if (kanji2nd) {
		kanji2nd--;
	    }
	    else if (ISKANJI(lgetc(curwp->w_dotp, curwp->w_doto - 1))) {
#ifdef HOJO_KANJI
		if (ISHOJO(lgetc(curwp->w_dotp, curwp->w_doto - 2)))
		    kanji2nd = 2;
		else
#endif /* HOJO_KANJI */
		    kanji2nd = 1;
	    }
#endif /* KANJI */
	    curwp->w_doto--;
	}
    }
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    if (kanji2nd) {			/* When stop at KANJI 2nd byte. */
	if (
#ifdef	HOJO_KANJI
	    (kanji2nd==2 && curwp->w_doto == 1) ||
#endif
	    curwp->w_doto == 0) { /* This is illegal, but... */
	    if ((lp=lback(curwp->w_dotp)) == curbp->b_linep) {
		if (!(f & FFRAND))
		    ewprintf("Beginning of buffer");
		return (FALSE);
	    }
	    curwp->w_dotp  = lp;
	    curwp->w_doto  = llength(lp);
	    curwp->w_flag |= WFMOVE;
	}
	else {		/* Go back KANJI 1st byte.	*/
	    curwp->w_doto -= kanji2nd;
	}
    }
#endif	/* KANJI */
    return TRUE;
}

/*
 * Go to end of line.
 */
/*ARGSUSED*/
int
gotoeol(f, n)
int f, n;
{
    curwp->w_doto  = llength(curwp->w_dotp);
    return (TRUE);
}

/*
 * Move cursor forwards. Do the
 * right thing if the count is less than
 * 0. Error if you try to move forward
 * from the end of the buffer.
#ifdef	KANJI
 * (91.01.01  Add comment by S.Yoshida)
 * When after moving "n" bytes, here is KANJI 2nd byte,
 * if "n" is 1 byte, go to next char, other go back to
 * the KANJI 1st byte of that char.
#endif
 */
/*ARGSUSED*/
int
forwchar(f, n)
int f;
register int n;
{
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    register int kanji2nd = 0;		/* Now on a KANJI 2nd byte. */
    register int oldn = n;
#endif /* KANJI */
    if (n < 0)
	return backchar(f, -n);
    while (n--) {
	if (curwp->w_doto == llength(curwp->w_dotp)) {
	    curwp->w_dotp  = lforw(curwp->w_dotp);
	    if (curwp->w_dotp == curbp->b_linep) {
		curwp->w_dotp = lback(curwp->w_dotp);
		if (!(f & FFRAND))
		    ewprintf("End of buffer");
		return FALSE;
	    }
	    curwp->w_doto  = 0;
	    curwp->w_flag |= WFMOVE;
	}
	else {
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
	    if (kanji2nd) {
		kanji2nd--;
#ifdef HOJO_KANJI
	    }
	    else if (ISHOJO(lgetc(curwp->w_dotp, curwp->w_doto))) {
		kanji2nd = 2;
#endif /* HOJO_KANJI */
	    }
	    else if (ISKANJI(lgetc(curwp->w_dotp, curwp->w_doto))) {
		kanji2nd = 1;
	    }
#endif /* KANJI */
	    curwp->w_doto++;
	}
    }
#ifdef KANJI	/* 90.01.29  by S.Yoshida */
    if (kanji2nd) {			/* When stop at KANJI 2nd byte.	*/
	if (oldn == 1) {		/* Special case. Go to next char. */
	    curwp->w_doto += kanji2nd;
	}
	else {			/* Go back KANJI 1st byte.	*/
	    curwp->w_doto--;
#ifdef HOJO_KANJI
	    if (ISHOJO(lgetc(curwp->w_dotp, curwp->w_doto)))
		curwp->w_doto--;
#endif
	}
    }
#endif /* KANJI */
    return TRUE;
}

/*
 * Go to the beginning of the
 * buffer. Setting WFHARD is conservative,
 * but almost always the case.
 */
int
gotobob(f, n)
int f, n;
{
    (VOID) setmark(f, n) ;
    curwp->w_dotp  = lforw(curbp->b_linep);
    curwp->w_doto  = 0;
    curwp->w_flag |= WFHARD;
    return TRUE;
}

/*
 * Go to the end of the buffer.
 * Setting WFHARD is conservative, but
 * almost always the case.
 */
int
gotoeob(f, n)
int f, n;
{
    (VOID) setmark(f, n) ;
    curwp->w_dotp  = lback(curbp->b_linep);
    curwp->w_doto  = llength(curwp->w_dotp);
    curwp->w_flag |= WFHARD;
    return TRUE;
}

/*
 * COMMAND: next-line-add-newlines
 */
static int flag_nextline = TRUE;
/*ARGSUSED*/
int
nextline(f, n)
int f, n;
{
    register int s;
    char buf[NINPUT];

    if ((f & FFARG) == 0) {
	if ((s = ereply("next-line-add-newlines : ", buf, sizeof(buf)))
	    != TRUE)
	    return (s);
	if (ISDIGIT(buf[0]) || buf[0] == '-')
	    n = atoi(buf) > 0;
	else if (buf[0] == 't' || buf[0] == 'T')
	    n = TRUE;
	else
	    n = FALSE;
    }
    flag_nextline = n;
    return (TRUE);
}


#ifdef	ADDFUNC
int line_number_mode = FALSE;

int
linenumbermode(f, n)
int f, n;
{
    register int s;
    register WINDOW *wp;
    char buf[NINPUT];

    if ((f & FFARG) == 0) {
	if ((s = ereply("line-number-mode : ", buf, sizeof(buf))) != TRUE)
	    return (s);
	if (ISDIGIT(buf[0]) || buf[0] == '-')
	    n = (atoi(buf) > 0);
	else if (buf[0] == 't' || buf[0] == 'T')
	    n = TRUE;
	else
	    n = FALSE;
    }
    line_number_mode = n;
    for (wp=wheadp; wp!=NULL; wp=wp->w_wndp)
	wp->w_flag |= WFMODE;
    return (TRUE);
}
#endif

/*
 * Move forward by full lines.
 * If the number of lines to move is less
 * than zero, call the backward line function to
 * actually do it. The last command controls how
 * the goal column is set.
 */
/*ARGSUSED*/
int
forwline(f, n)
int f, n;
{
    register LINE *dlp;
    
    if (n < 0)
	return backline(f|FFRAND, -n);
    if ((lastflag&CFCPCN) == 0)		/* Fix goal.		*/
	setgoal();
    thisflag |= CFCPCN;
    if (n == 0) return TRUE;
    dlp = curwp->w_dotp;
    while (lforw(dlp)!=curbp->b_linep && n--)
	dlp = lforw(dlp);
    curwp->w_flag |= WFMOVE;
    if (n > 0) /* ^N at end of buffer creates lines (like gnu) */
    {
	if (!flag_nextline) {
	    dlp = lback(curbp->b_linep);
	    curwp->w_dotp  = dlp;
	    curwp->w_doto  = getgoal(dlp);
	}
	else
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	    if (curbp->b_flag & BFRONLY) { /* If this buffer is read-only, */
		warnreadonly();		   /* do only displaying warning.  */
	    }
	    else {
#endif	/* READONLY */
#ifdef	AUTOSAVE	/* 96.12.25 by M.Suzuki	*/
		curbp->b_flag |= BFACHG;
#endif	/* AUTOSAVE */
		if (!(curbp->b_flag&BFCHG)) {	/* first change */
		    curbp->b_flag |= BFCHG;
		    curwp->w_flag |= WFMODE;
		}
		curwp->w_doto = llength(curwp->w_dotp);
		while (n-- > 0)
		    lnewline();
#ifdef	READONLY	/* 91.01.05  by S.Yoshida */
	    }
#endif	/* READONLY */
    }
    else {
	curwp->w_dotp  = dlp;
	curwp->w_doto  = getgoal(dlp);
    }
#ifdef ADDFUNC
    if (line_number_mode)
	curwp->w_flag |= WFHARD;
    return n>0 ? FALSE : TRUE;
#else
    return TRUE;
#endif
}

/*
 * This function is like "forwline", but
 * goes backwards. The scheme is exactly the same.
 * Check for arguments that are less than zero and
 * call your alternate. Figure out the new line and
 * call "movedot" to perform the motion.
 */
/*ARGSUSED*/
int
backline(f, n)
int f, n;
{
    register LINE *dlp;

    if (n < 0)
	return forwline(f|FFRAND, -n);
    if ((lastflag&CFCPCN) == 0)		/* Fix goal.		*/
	setgoal();
    thisflag |= CFCPCN;
    dlp = curwp->w_dotp;
    while (n-- && lback(dlp)!=curbp->b_linep)
	dlp = lback(dlp);
    curwp->w_dotp  = dlp;
    curwp->w_doto  = getgoal(dlp);
    curwp->w_flag |= WFMOVE;
#ifdef ADDFUNC		/* amura */
    if (line_number_mode)
	curwp->w_flag |= WFHARD;
    return n >= 0 ? FALSE : TRUE;
#else
    return TRUE;
#endif
}

/*
 * Set the current goal column,
 * which is saved in the external variable "curgoal",
 * to the current cursor column. The column is never off
 * the edge of the screen; it's more like display then
 * show position.
 */
VOID
setgoal()
{
    curgoal = getcolpos() - 1;		/* Get the position.	*/
    /* we can now display past end of display, don't chop! */
}

/*
 * This routine looks at a line (pointed
 * to by the LINE pointer "dlp") and the current
 * vertical motion goal column (set by the "setgoal"
 * routine above) and returns the best offset to use
 * when a vertical motion is made into the line.
 */
int
getgoal(dlp)
register LINE *dlp;
{
    register int c;
    register int col;
    register int newcol;
    register int dbo;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
    register int kanji2nd = FALSE;	/* Now on a KANJI 2nd byte. */
#endif	/* KANJI */
#ifdef  VARIABLE_TAB
    int tab = curbp->b_tabwidth;
#endif  /* VARIABLE_TAB */

    col = 0;
    dbo = 0;
    while (dbo != llength(dlp)) {
	c = lgetc(dlp, dbo);
	newcol = col;
#ifdef	HOJO_KANJI
	if (ISHOJO(c) && !kanji2nd) {
	    dbo++;
	    continue;
	}
#endif	/* HOJO_KANJI */
	if (c == '\t' && !(curbp->b_flag & BFNOTAB))
#ifdef VARIABLE_TAB
	    newcol = (newcol/tab + 1)*tab -1;
#else
	    newcol |= 0x07;
#endif
	else if (ISCTRL(c) != FALSE)
	    ++newcol;
#ifdef HANKANA  /* 92.11.21  by S.Sasaki */
	else if (ISHANKANA(c) && !kanji2nd)
	    --newcol;
#endif  /* HANKANA */
	++newcol;
	if (newcol > curgoal)
	    break;
	col = newcol;
	++dbo;
#ifdef	KANJI	/* 90.01.29  by S.Yoshida */
	if (kanji2nd) {
	    kanji2nd = FALSE;
	}
	else if (ISKANJI(c)) {
	    kanji2nd = TRUE;
	}
    }
    if (kanji2nd) {		/* When stop at KANJI 2nd byte,	*/
	dbo--;			/* go back KANJI 1st byte.	*/
#endif	/* KANJI */
    }
    return (dbo);
}

/*
 * Scroll forward by a specified number
 * of lines, or by a full page if no argument.
 * The "2" is the window overlap (this is the default
 * value from ITS EMACS). Because the top line in
 * the window is zapped, we have to do a hard
 * update and get it back.
 */
/*ARGSUSED*/
int
forwpage(f, n)
int f;
register int n;
{
    register LINE *lp;
    
    if (!(f & FFARG)) {
	n = curwp->w_ntrows - 2;	/* Default scroll.	*/
	if (n <= 0)			/* Forget the overlap	*/
	    n = 1;			/* if tiny window.	*/
    }
    else if (n < 0)
	return backpage(f|FFRAND, -n);
#ifdef	CVMVAS
    else					/* Convert from pages	*/
	n *= curwp->w_ntrows;			/* to lines.		*/
#endif
    lp = curwp->w_linep;
    n += curwp->w_lines;
    while (n>0 && lforw(lp)!=curbp->b_linep) {
	n -= countlines(lp);
	if (n < 0)
	    break;
	lp = lforw(lp);
    }
    if (n > 0) /* LAST row */
	n = countlines(lp) - 1;
    if (n < 0)
	n = countlines(lp) + n;
    curwp->w_linep = lp;
    curwp->w_lines = n;
    curwp->w_flag |= WFHARD;
    /* if in current window, don't move dot */
    for (n = curwp->w_ntrows; n>=0 && lp!=curbp->b_linep; lp = lforw(lp)) {
	int x,y;
	if (lp==curwp->w_dotp && 
	    colrow(lp, curwp->w_doto, &x, &y) < n)
	    return TRUE;
	n -= countlines(lp);
    }
    curwp->w_dotp  = curwp->w_linep;
    curwp->w_doto  = skipline(curwp->w_linep, curwp->w_lines);
    return TRUE;
}

/*
 * This command is like "forwpage",
 * but it goes backwards. The "2", like above,
 * is the overlap between the two windows. The
 * value is from the ITS EMACS manual. The
 * hard update is done because the top line in
 * the window is zapped.
 */
/*ARGSUSED*/
int
backpage(f, n)
int f;
register int n;
{
    register LINE *lp;
    
    if (!(f & FFARG)) {
	n = curwp->w_ntrows - 2;	/* Default scroll.	*/
	if (n <= 0)			/* Don't blow up if the */
	    n = 1;			/* window is tiny.	*/
    }
    else if (n < 0)
	return forwpage(f|FFRAND, -n);
#ifdef	CVMVAS
    else				/* Convert from pages	*/
	n *= curwp->w_ntrows;		/* to lines.		*/
#endif
    lp = curwp->w_linep;
    n -= curwp->w_lines;
    while (n>0 && lback(lp)!=curbp->b_linep) {
	lp = lback(lp);
	n -= countlines(lp);
    }
    if (n > 0)
	n = 0;
    if (n < 0
	) n = -n;
    curwp->w_linep = lp;
    curwp->w_lines = n;
    curwp->w_flag |= WFHARD;
    /* if in current window, don't move dot */
    for (n = curwp->w_ntrows; n>=0 && lp!=curbp->b_linep; lp = lforw(lp)) {
	int x,y;
	if (lp==curwp->w_dotp && 
	    colrow(lp, curwp->w_doto, &x, &y) < n)
	    return TRUE;
	n -= countlines(lp);
    }
    curwp->w_dotp = curwp->w_linep;
    curwp->w_doto  = skipline(curwp->w_linep, curwp->w_lines);
    return TRUE;
}

/* These functions are provided for compatibility with Gosling's Emacs.
 *    They are used to scroll the display up (or down) one line at a time.
 */
#ifdef GOSMACS
int
forw1page(f, n)
int f, n;
{
    if (!(f & FFARG))  {
	n = 1;
	f = FFUNIV;
    }
    forwpage(f|FFRAND, n);
}

int
back1page(f, n)
int f, n;
{
    if (!(f & FFARG)) {
	n = 1;
	f = FFUNIV;
    }
    backpage(f|FFRAND, n);
}
#endif

/*
 * Page the other window. Check to make sure it exists, then
 * nextwind, forwpage and restore window pointers.
 */
int
pagenext(f, n)
int f, n;
{
    register WINDOW *wp;

    if (wheadp->w_wndp == NULL) {
	ewprintf("No other window");
	return FALSE;
    }
    wp = curwp;
    (VOID) nextwind(f, n);
    (VOID) forwpage(f, n);
    curwp = wp;
    curbp = wp->w_bufp;
    return TRUE;
}

/*
 * Internal set mark routine, used by other functions (daveb).
 */
VOID
isetmark()
{
    curwp->w_bufp->b_markp = curwp->w_dotp;
    curwp->w_bufp->b_marko = curwp->w_doto;
}

/*
 * Set the mark in the current window
 * to the value of dot. A message is written to
 * the echo line.  (ewprintf knows about macros)
 */
/*ARGSUSED*/
int
setmark(f, n)
int f, n;
{
    isetmark();
    ewprintf("Mark set");
    return TRUE;
}

/*
 * Swap the values of "dot" and "mark" in
 * the current window. This is pretty easy, because
 * all of the hard work gets done by the standard routine
 * that moves the mark about. The only possible
 * error is "no mark".
 */
/*ARGSUSED*/
int
swapmark(f, n)
int f, n;
{
    register LINE *odotp;
    register int odoto;
    BUFFER *bp = curwp->w_bufp;
    
    if (bp->b_markp == NULL) {
	ewprintf("No mark in this buffer");
	return FALSE;
    }
    odotp = curwp->w_dotp;
    odoto = curwp->w_doto;
    curwp->w_dotp  = bp->b_markp;
    curwp->w_doto  = bp->b_marko;
    bp->b_markp = odotp;
    bp->b_marko = odoto;
    curwp->w_flag |= WFMOVE;
    return TRUE;
}

/*
 * Go to a specific line, mostly for
 * looking up errors in C programs, which give the
 * error a line number. If an argument is present, then
 * it is the line number, else prompt for a line number
 * to use.
 */
/*ARGSUSED*/
int
gotoline(f, n)
int f;
register int n;
{
    register LINE *clp;
    register int s;
    char buf[NINPUT];

    if (!(f & FFARG)) {
	if ((s=ereply("Goto line: ", buf, sizeof(buf))) != TRUE)
	    return s;
	n = atoi(buf);
    }
    
    if (n > 0) {
	clp = lforw(curbp->b_linep);	/* "clp" is first line	*/
	while (--n > 0) {
	    if (lforw(clp) == curbp->b_linep) break;
	    clp = lforw(clp);
	}
    }
    else {
	clp = lback(curbp->b_linep);	/* clp is last line */
	while (n < 0) {
	    if (lback(clp) == curbp->b_linep) break;
	    clp = lback(clp);
	    n++;
	}
    }
    curwp->w_dotp = clp;
    curwp->w_doto = 0;
    curwp->w_flag |= WFMOVE;
    return TRUE;
}
