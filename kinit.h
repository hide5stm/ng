/* $Id: kinit.h,v 1.5 2003/02/22 08:09:47 amura Exp $ */
/*
 *	Initial KANJI code setting. (Like site-init.el in Nemacs)
 *
 *		Coded by Shigeki Yoshida (shige@csk.CO.JP)
 */
/* 90.01.29	Created by S.Yoshida */

/*	NOCONV	No KANJI code conversion.	*/
/*	SJIS	KANJI code is Shift-JIS.	*/
/*	JIS	KANJI code is JIS.		*/
/*	EUC	KANJI code is EUC.		*/
/*	NIL	Not decided.			*/
/*	_T_	Guess.				*/
#ifdef	MSDOS	/* 90.02.11  by S.Yoshida */
#define	KFIO		SJIS		/* default-kanji-fileio-code	*/
#define	KEXPECT		_T_		/* kanji-expected-code		*/
#define	KINPUT		SJIS		/* kanji-input-code		*/
#define	KDISPLAY	SJIS		/* kanji-display-code		*/
#else	/* NOT MSDOS */
#ifdef	HUMAN68K	/* 90.11.09    Sawayanagi Yosirou */
#define	KFIO		SJIS		/* default-kanji-fileio-code	*/
#define	KEXPECT		_T_		/* kanji-expected-code		*/
#define	KINPUT		SJIS		/* kanji-input-code		*/
#define	KDISPLAY	SJIS		/* kanji-display-code		*/
#else	/* NOT HUMAN68K */
#ifdef	WIN32
#define	KFIO		SJIS	/* default-kanji-fileio-code	*/
#define	KEXPECT		_T_		/* kanji-expected-code		*/
#define	KINPUT		SJIS	/* kanji-input-code		*/
#define	KDISPLAY	SJIS	/* kanji-display-code		*/
#else	/* WIN32 */
#define	KFIO		NIL		/* default-kanji-fileio-code	*/
#define	KEXPECT		_T_		/* kanji-expected-code		*/
#define	KINPUT		EUC		/* kanji-input-code		*/
#define	KDISPLAY	EUC		/* kanji-display-code		*/
#endif	/* WIN32 */
#endif	/* HUMAN68K */
#endif	/* MSDOS */

/*	'@'	Select JIS-78 [ESC-$-@].	*/
/*	'B'	Select JIS-83 [ESC-$-B].	*/
#define	TO_KFIO		'B'		/* to-kanji-fileio		*/
#define	TO_KDISPLAY	'B'		/* to-kanji-display		*/

/*	'B'	Select ASCII		     [ESC-(-B].	*/
/*	'J'	Select JIS-ROMAJI	     [ESC-(-J].	*/
/*	'H'	Illegal selection, but used. [ESC-(-H].	*/
#define	TO_AFIO		'B'		/* to-ascii-fileio		*/
#define	TO_ADISPLAY	'B'		/* to-ascii-display		*/

#ifdef  HANKANA  /* 92.11.21  by S.Sasaki */
/*	'7'	Use SI/SO for KATAKANA	     [SI / SO]	*/
/*	'8'	Through KATAKANA code	     Through.	*/
/*	'I'	Select JIS-KATAKANA	     [ESC-(-I].	*/
#define TO_KANAFIO	'7'		/* to-kana-fileio		*/
#define TO_KANADISPLAY	'7'		/* to-kana-display		*/
#endif  /* HANKANA */

#ifdef HOJO_KANJI
#define TOUFU1ST	(0x22|0x80)	/* EUC igeta */
#define TOUFU2ND	(0x2E|0x80)
#endif
