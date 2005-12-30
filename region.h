/* $Id: region.h,v 1.1.2.4 2005/12/30 17:37:29 amura Exp $ */
/*
 * This file is the header file for region parts
 * of the NG display editor.
 */
#ifndef __REGION_H__
#define __REGION_H__

#include "i_line.h"

#ifdef __cplusplus
extern "C" {
#endif

int getregion _PRO((REGION *));
int killregion _PRO((int, int));
int setprefix _PRO((int, int));
int copyregion _PRO((int, int));

#ifdef __cplusplus
}
#endif

#endif /* __REGION_H__ */