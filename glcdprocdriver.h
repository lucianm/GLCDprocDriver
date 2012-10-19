/*
 * "C" wrapper header for cglcdDriver
 *
 * (c) 2005 - 2010 Lucian Muresan <lucianm AT users.sourceforge.net>
 */

#ifndef _GLCDPROC_DRIVER_H_
#define _GLCDPROC_DRIVER_H_


#ifdef __cplusplus
extern "C" {
#endif


#ifndef bool
# define bool short
# define true 1
# define false 0
#endif



typedef struct glcddriver GlcdDriver;
GlcdDriver* glcddriverCreate();
void glcddriverDestroy(GlcdDriver* drv);

int glcddriverTextWidth(GlcdDriver* drv);
int glcddriverTextHeight(GlcdDriver* drv);
int glcddriverCellWidth(GlcdDriver* drv);
int glcddriverCellHeight(GlcdDriver* drv);
int glcddriverMaxCustomChars(GlcdDriver* drv);
bool glcddriverInit(	GlcdDriver* drv,
						const char displayName[],
						const char fontFile[],
						bool bUseFT,
						const char CharEncoding[],
						const int nPixHeight,
						bool bShowBigBorder,
						bool bShowThinBorder,
						bool bShowDebugFrame,
						int nPixShiftX,
						int nPixShiftY,
						bool bUpsideDown,
						bool bBacklight,
						int nContrast);
void glcddriverDeInit(GlcdDriver* drv);
void glcddriverClear(GlcdDriver* drv);
void glcddriverRefresh(GlcdDriver* drv, bool refreshAll);
void glcddriverSetBrightness(GlcdDriver* drv, unsigned int percent);
unsigned int glcddriverGetBrightness(GlcdDriver* drv);
void glcddriverInvert(GlcdDriver* drv, bool bInvert);
void glcddriverDrawText(GlcdDriver* drv, int x, int y, char text[]);
void glcddriverDrawCharacter(GlcdDriver* drv, int x, int y, char c);
int glcddriverDrawIcon(GlcdDriver* drv, int x, int y, int icon);
void glcddriverSetCustomChar(GlcdDriver* drv, int pos, char *pixeldata);
void glcddriverDrawBigNum(GlcdDriver* drv, int x, int num);
void glcddriverDrawBarH(GlcdDriver* drv, int x, int y, int len, int promille, int options);
void glcddriverDrawBarV(GlcdDriver* drv, int x, int y, int len, int promille, int options);

#ifdef __cplusplus
}
#endif


#endif
