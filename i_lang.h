/* $Id: i_lang.h,v 1.1.2.15 2007/08/03 13:45:18 amura Exp $ */
/*
 * This file is the language module definition of the NG
 * display editor.
 */

#ifndef __I_LANG_H__
#define __I_LANG_H__

#include "in_code.h"

#define NG_CODE_FOR_DISPLAY	0x01
#define NG_CODE_FOR_FILE	0x02
#define NG_CODE_FOR_INPUT	0x04
#define NG_CODE_FOR_FILENAME	0x08
#define NG_CODE_FOR_IO		0x10

typedef struct CODEMAP {
    char *cm_name;	/* codemap name */
    short cm_type;	/* codemap type */
    short cm_code;	/* codemap number */
#define NG_CODE_PASCII		-1	/* pretty print ascii: MUST NEED */
#define NG_CODE_NONE		0	/* unknown code: MUST NEED */
#define NG_CODE_ASCII		1	/* 7bit ascii: MUST NEED */
#define NG_CODE_BINARY		2	/* MUST NEED? */
#define NG_CODE_ISO2022		3	/* OPTIONAL */
#define NG_CODE_UCS2		(64+0)	/* OPTIONAL */
#define NG_CODE_UCS2LE		(64+1)	/* OPTIONAL */
#define NG_CODE_UCS2BE		(64+2)	/* OPTIONAL */
#define NG_CODE_UTF8		(64+3)	/* OPTIONAL */
#define NG_CODE_UTF16		(64+4)	/* OPTIONAL */
#define NG_CODE_UTF16LE		(64+5)	/* OPTIONAL */
#define NG_CODE_UTF16BE		(64+6)	/* OPTIONAL */
#define NG_CODE_LOCALBASE	256	/* basis of LOCAL code */
#define IS_NG_CODE_GLOBAL(n)	((n) < NG_CODE_LOCALBASE)
#define IS_NG_CODE_UNICODE(n)	((n) >= NG_CODE_UCS2 && (n) <= NG_CODE_UTF16BE)
#define IS_NG_CODE_LOCAL(n)	(!IS_NG_CODE_GLOBAL(n))
} CODEMAP;


#define NG_CODE_CHKLEN		-1
#define NG_CODE_MAXLEN		8

typedef struct LANG_MODULE {
    /* LANG_MODULE name */
    char *lm_name;
    
    /* return codemaps */
    CODEMAP *(*lm_get_codemap)_PRO((void));

    /* code expector */
    int (*lm_code_expect)_PRO((const char*, int));
    
    /* convert to output length without strend */
    int (*lm_out_convert_len)_PRO((int, const NG_WCHAR_t *, int));
    int (*lm_out_convert)_PRO((int, const NG_WCHAR_t *, int, char *));
    
    /* convert to input length without strend */
    int (*lm_in_convert_len)_PRO((int, const char *, int));
    int (*lm_in_convert)_PRO((int, const char *, int, NG_WCHAR_t *));
    
    /* get charactor category */
    int (*lm_get_category)_PRO((NG_WCHAR_ta));

    /* set display and keyboard coding */
    int (*lm_set_code)_PRO((int, int));
    
    /* get display and keyboard coding */
    int (*lm_get_code)_PRO((int));

    /* for input */
    NG_WCHAR_t (*lm_get_keyin_code)_PRO((int));
    
    /* for display */
    /* Return display width at this code*/
    int (*lm_width)_PRO((NG_WCHAR_ta));
    /* return display codes to buffer */
    int (*lm_get_display_code)_PRO((int, NG_WCHAR_ta, char *, int));
    /* Buf to T-VRAM convertion routine */
    VOID (*lm_displaychar)_PRO((NG_WCHAR_t*, int, int, NG_WCHAR_ta));
} LANG_MODULE;

#define LM_OUT_CONVERT_TMP(lm, code, src, dst)	do {	\
    int _len = (lm)->lm_out_convert_len((code), (src), NG_CODE_CHKLEN);\
    if (((dst)=(char *)alloca(_len+1)) != NULL)		\
        (lm)->lm_out_convert((code), (src), _len+1, (dst));\
} while (0/*CONSTCOND*/)
#define LM_OUT_CONVERT_TMP2(lm, cn, src, dst) do {	\
    int _code = (lm)->lm_get_code(cn);			\
    LM_OUT_CONVERT_TMP((lm), _code, (src), (dst));	\
} while (0/*CONSTCOND*/)
#define LM_OUT_CONVERT2(lm, cn, src, dst) \
    (lm)->lm_out_convert((lm)->lm_get_code(cn), (src), NG_CODE_CHKLEN, (dst))

#define LM_IN_CONVERT_TMP(lm, code, src, dst)	do {    \
    int _len = lm->lm_in_convert_len((code), (src), NG_CODE_CHKLEN);\
    if (((dst)=(NG_WCHAR_t *)alloca((_len+1) * sizeof(NG_WCHAR_t))) != NULL) \
        lm->lm_in_convert((code), (src), _len+1, (dst));\
} while (0/*CONSTCOND*/)
#define LM_IN_CONVERT_TMP2(lm, cn, src, dst) do {	\
    int _code = (lm)->lm_get_code(cn);			\
    LM_IN_CONVERT_TMP((lm), _code, (src), (dst));	\
} while (0/*CONSTCOND*/)
#define LM_IN_CONVERT2(lm, cn, src, dst) \
    (lm)->lm_in_convert((lm)->lm_get_code(cn), (src), NG_CODE_CHKLEN, (dst))

#define LANG_DEFINE(name, getter)	/* NOP */
extern LANG_MODULE *terminal_lang;
extern LANG_MODULE *default_lang;

#endif /* __I_LANG_H__ */
