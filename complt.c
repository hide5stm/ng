/*
 *	Complete completion functions.
 */
/* 90.12.10  Created by Sawayanagi Yosirou */

/* $Id: complt.c,v 1.1 1999/05/21 02:05:34 amura Exp $ */

/* $Log: complt.c,v $
/* Revision 1.1  1999/05/21 02:05:34  amura
/* Initial revision
/*
*/

#include    "config.h"
#ifdef	NEW_COMPLETE
#include    "def.h"
#include    "kbd.h"
#include    "complt.h"
#define    LIST_COL    35

BUFFER    *bp = NULL;
BUFFER    *prev_bp = NULL;
WINDOW    *prev_wp = NULL;
WINDOW    prev_window;

/*
 * do some completion.
 */
int
complete (buf, flags)
    char    *buf;
    int    flags;
{
    int    res;
    static int complete_funcname ();
    static int complete_buffername ();
    static int complete_filename ();

    switch (flags & (EFFUNC | EFBUF | EFFILE))
      {
      case EFFUNC:
	res = complete_funcname (buf);
	break;
      case EFBUF:
        res = complete_buffername (buf);
	break;
      case EFFILE:
	res = complete_filename (buf);
	break;
      default:
        panic ("broken complete call: flags");
      }

    return (res);
}

char *
complete_message (matchnum)
    int    matchnum;
{
    char    *msg;

    switch (matchnum)
      {
      case COMPLT_NOT_UNIQUE:
	msg = " [Complete, but not unique]";
	break;
      case COMPLT_AMBIGUOUS:
	msg = " [Ambiguous]";
	break;
      case COMPLT_SOLE:
/*	msg = "";*/
	msg = " [Sole completion]";
	break;
      case COMPLT_NO_MATCH:
	msg = " [No match]";
	break;
      default:
	msg = " [Internal error]";
      }

    return (msg);
}

/* complete function name */
static int
complete_funcname (name)
    char    *name;
{
    int    fnlen;
    int    minlen;
    int    matchnum;
    int    res;
    int    i, j;
    char    *cand;

    fnlen = strlen (name);

    /* compare names and make the common string of them */
    matchnum = 0;
    for (i = name_fent(name, TRUE); i < nfunct; i++)
      {
        cand = functnames[i].n_name;
	j = strncmp (cand, name, fnlen);
        if (j < 0)
	  continue;
        else if (j > 0)
	  break;	/* because functnames[] are in dictionary order */

	if (matchnum == 0)
	  {
	    for (j = fnlen; cand[j] != '\0'; j++)
	      name[j] = cand[j];
	    name[j] = '\0';
	    minlen = j;
	  }
	else
	  {
	    for (j = fnlen; name[j] != '\0'; j++)
              {
		if (cand[j] != name[j])
		    break;
              }
	    name[j] = '\0';
	    if (cand[j] == '\0')
	        minlen = j;
	  }
        matchnum++;
      }

    if (matchnum > 1)
      res = (minlen == strlen (name)) ? COMPLT_NOT_UNIQUE : COMPLT_AMBIGUOUS;
    else if (matchnum == 1)
      res = COMPLT_SOLE;
    else if (matchnum == 0)
      res = COMPLT_NO_MATCH;
    else
      res = -1;

    return (res);
}

static int
complete_buffername (name)
    char    *name;
{
    int    fnlen;
    int    minlen;
    int    matchnum;
    int    res;
    int    i, j;
    char    *cand;
    LIST    *lh;

    fnlen = strlen (name);

    /* compare names and make the common string of them */
    matchnum = 0;
    for (lh = &(bheadp->b_list); lh != NULL; lh = lh->l_next)
      {
        cand = lh->l_name;
        if (strncmp (cand, name, fnlen) != 0)
	  continue;
	if (matchnum == 0)
	  {
	    for (j = fnlen; cand[j] != '\0'; j++)
	      name[j] = cand[j];
	    name[j] = '\0';
	    minlen = j;
	  }
	else
	  {
            for (j = fnlen; name[j] != '\0'; j++)
              {
                if (cand[j] != name[j])
	          break;
              }
	    name[j] = '\0';
	    if (cand[j] == '\0')
	        minlen = j;
	  }
        matchnum++;
      }

    if (matchnum > 1)
      res = (minlen == strlen (name)) ? COMPLT_NOT_UNIQUE : COMPLT_AMBIGUOUS;
    else if (matchnum == 1)
      res = COMPLT_SOLE;
    else if (matchnum == 0)
      res = COMPLT_NO_MATCH;
    else
      res = -1;

    return (res);
}

#ifndef NO_FILECOMP
static int
complete_filename (name)
    char    *name;
{
    int    fnlen;
    int    minlen;
    int    matchnum;
    int    res;
    int    i, j;
    int    fnnum;
    char    *cand;
    char    *filenames;
    int    fffiles ();

    fnlen = strlen (name);

    if ((fnnum = fffiles (name, &filenames)) == -1)
      return (-1);    /* error */

    /* compare names and make a common string of them */
    matchnum = 0;
    cand = filenames;
    for (i = 0; i < fnnum; i++)
      {
	if (matchnum == 0)
	  {
	    for (j = fnlen; cand[j] != '\0'; j++)
	      name[j] = cand[j];
	    name[j] = '\0';
	    minlen = j;
	  }
	else
	  {
	    for (j = fnlen; name[j] != '\0'; j++)
              {
		if (cand[j] != name[j])
		  break;
	      }
	    name[j] = '\0';
	    if (cand[j] == '\0')
	        minlen = j;
	  }
	matchnum++;
	cand += (strlen (cand) + 1);
      }
    free (filenames);

    if (matchnum > 1)
      res = (minlen == strlen (name)) ? COMPLT_NOT_UNIQUE : COMPLT_AMBIGUOUS;
    else if (matchnum == 1)
      res = COMPLT_SOLE;
    else if (matchnum == 0)
      res = COMPLT_NO_MATCH;
    else
      res = -1;

    return (res);
}
#endif	/* NO_FILECOMP */

int
complete_list_names (buf, flags)
    char    *buf;
    int    flags;
{
    int    res;
    int    cur_row;
    int    cur_col;
    WINDOW    *wp;
    static int complete_list_funcnames ();
    static int complete_list_buffernames ();
    static int complete_list_filenames ();

    if ((bp = bfind ("*Completions*", TRUE)) == NULL)
      return (FALSE);
    bp->b_flag &= ~BFCHG;    /* avoid recursive veread */
    if (bclear (bp) != TRUE)
      return (FALSE);

    if (addline(bp, "Possible completions are:") == FALSE)
      return (FALSE);

    switch (flags & (EFFUNC | EFBUF | EFFILE))
      {
      case EFFUNC:
	res = complete_list_funcnames (buf, bp);
	break;
      case EFBUF:
        res = complete_list_buffernames (buf, bp);
	break;
      case EFFILE:
	res = complete_list_filenames (buf, bp);
	break;
      default:
        panic ("broken complete_list_names call: flags");
      }

    cur_row = ttrow;
    cur_col = ttcol;

    bp->b_dotp = lforw (bp->b_linep);	/* put dot at beginning of buffer */
    bp->b_doto = 0;
    /* setup window */
    if (curwp->w_bufp != bp)
      {
	if (wheadp->w_wndp == NULL)
	  {
	    if (splitwind (FFRAND, 0) == FALSE)
	      return (FALSE);
	    prev_wp = wheadp;
	    curwp = wheadp->w_wndp;
	    prev_bp = NULL;
	  }
	else
	  {
            for (wp = wheadp; wp->w_wndp != NULL; wp = wp->w_wndp)
              ;
            prev_wp = curwp;
	    curwp = wp;
	    prev_bp = curwp->w_bufp;
	    prev_window.w_dotp = curwp->w_dotp;
	    prev_window.w_doto = curwp->w_doto;
	    prev_window.w_markp = curwp->w_markp;
	    prev_window.w_marko = curwp->w_marko;
	  }
      }
    for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
      {
        if (wp->w_bufp == bp)
	  {
	    wp->w_flag |= WFMODE | WFFORCE | WFHARD;
	    wp->w_dotp = bp->b_dotp;
	    wp->w_doto = bp->b_doto;
	  }
      }
    curbp = bp;
    if (showbuffer (bp, curwp, WFFORCE | WFHARD) == FALSE)
      return (FALSE);

    update ();
    ttmove (cur_row, cur_col);

    return (TRUE);
}

static int
complete_list_funcnames (name, bp)
    char    *name;
    BUFFER    *bp;
{
    int    fnlen;
    int    i, j;
    char    *cand;
    char    line[NFILEN];

    fnlen = strlen (name);

    line[0] = '\0';
    for (i = name_fent(name, TRUE); i < nfunct; i++)
      {
        cand = functnames[i].n_name;
	j = strncmp (cand, name, fnlen);
        if (j < 0)
	  continue;
        else if (j > 0)
	  break;	/* because functnames[] are in dictionary order */

	if (line[0] == '\0')
	  {
	    if (strlen (cand) < LIST_COL)
	      strcpy (line, cand);
	    else
	      addline (bp, cand);
	  }
	else
	  {
	    if (strlen (cand) < LIST_COL)
	      {
		for (j = strlen (line); j < LIST_COL; j++)
		  line[j] = ' ';
		line[j] = '\0';
		strcat (line, cand);
		addline (bp, line);
	      }
	    else
	      {
		addline (bp, line);
		addline (bp, cand);
	      }
	    line[0] = '\0';
	  }
      }
    if (line[0] != '\0')
      addline (bp, line);
    return (TRUE);
}

static int
complete_list_buffernames (name, bp)
    char    *name;
    BUFFER    *bp;
{
    int    fnlen;
    int    i, j;
    char    *cand;
    char    line[NFILEN];
    LIST    *lh;

    fnlen = strlen (name);

    line[0] = '\0';
    for (lh = &(bheadp->b_list); lh != NULL; lh = lh->l_next)
      {
        cand = lh->l_name;
        if (strncmp (cand, name, fnlen) != 0)
	  continue;

	if (line[0] == '\0')
	  {
	    if (strlen (cand) < LIST_COL)
	      strcpy (line, cand);
	    else
	      addline (bp, cand);
	  }
	else
	  {
	    if (strlen (cand) < LIST_COL)
	      {
		for (j = strlen (line); j < LIST_COL; j++)
		  line[j] = ' ';
		line[j] = '\0';
		strcat (line, cand);
		addline (bp, line);
	      }
	    else
	      {
		addline (bp, line);
		addline (bp, cand);
	      }
	    line[0] = '\0';
	  }
      }
    if (line[0] != '\0')
      addline (bp, line);
    return (TRUE);
}

static int
complete_list_filenames (name, bp)
    char    *name;
    BUFFER    *bp;
{
    int    fnlen;
    int    i, j;
    int    fnnum;
    char    *cand;
    char    line[NFILEN];
    char    *filenames;
    int    fffiles ();
    char    *file_name_part ();

    fnlen = strlen (name);

    if ((fnnum = fffiles (name, &filenames)) == -1)
      return (FALSE);    /* error */

    line[0] = '\0';
    cand = filenames;
    for (i = 0; i < fnnum; i++)
      {
	cand = file_name_part (cand);
	if (line[0] == '\0')
	  {
	    if (strlen (cand) < LIST_COL)
	      strcpy (line, cand);
	    else
	      addline (bp, cand);
	  }
	else
	  {
	    if (strlen (cand) < LIST_COL)
	      {
		for (j = strlen (line); j < LIST_COL; j++)
		  line[j] = ' ';
		line[j] = '\0';
		strcat (line, cand);
		addline (bp, line);
	      }
	    else
	      {
		addline (bp, line);
		addline (bp, cand);
	      }
	    line[0] = '\0';
	  }
	cand += (strlen (cand) + 1);
      }
    if (line[0] != '\0')
      addline (bp, line);
    free (filenames);
    return (TRUE);
}

int
complete_del_list ()
{
    int    cur_row;
    int    cur_col;

    if (bp == NULL)
      return (FALSE);
    cur_row = ttrow;
    cur_col = ttcol;
    if (prev_bp == NULL)
      {
        if (wheadp->w_wndp != NULL)
	  delwind (FFRAND, 0);
      }
    else
      {
	curbp = prev_bp;
	showbuffer (prev_bp, curwp, WFFORCE | WFHARD);
	curwp->w_dotp = prev_window.w_dotp;
	curwp->w_doto = prev_window.w_doto;
	curwp->w_markp = prev_window.w_markp;
	curwp->w_marko = prev_window.w_marko;
	curwp->w_flag |= WFMOVE;
	curwp = prev_wp;
	curbp = curwp->w_bufp;
      }      
    bp = NULL;
    prev_wp = NULL;
    prev_bp = NULL;
    update ();
    ttmove (cur_row, cur_col);
    /* 91.01.17  Add to delete *Completions* buffer. by S.Yoshida */
    eargset("*Completions*");
    killbuffer(0, 1);

    return (TRUE);
}

int
complete_scroll_up ()
{
    int    cur_row;
    int    cur_col;

    if (bp == NULL)
      return (FALSE);
    cur_row = ttrow;
    cur_col = ttcol;
    backpage (FFRAND, 0);
    update ();
    ttmove (cur_row, cur_col);
    return (TRUE);
}

int
complete_scroll_down ()
{
    int    cur_row;
    int    cur_col;

    if (bp == NULL)
      return (FALSE);
    cur_row = ttrow;
    cur_col = ttcol;
    forwpage (FFRAND, 0);
    update ();
    ttmove (cur_row, cur_col);
    return (TRUE);
}
#endif	/* NEW_COMPLETE */
