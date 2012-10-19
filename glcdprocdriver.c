/*
 * "C" wrapper implementation for cglcdDriver
 *
 * (c) 2005 - 2010 Lucian Muresan <lucianm AT users.sourceforge.net>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "glcddriver.h"
#include "glcdprocdriver.h"


#ifdef __cplusplus
extern "C" {
#endif

GlcdDriver* glcddriverCreate()
{
    cglcdDriver* cpp_drv = new cglcdDriver();
    GlcdDriver* c_drv = reinterpret_cast<GlcdDriver*>(cpp_drv);
    return c_drv;
}

void glcddriverDestroy(GlcdDriver* drv)
{
    cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
    delete cpp_drv;
}

int glcddriverTextWidth(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->TextWidth();
}

int glcddriverTextHeight(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->TextHeight();
}

int glcddriverCellWidth(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->CellWidth();
}

int glcddriverCellHeight(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->CellHeight();
}

int glcddriverMaxCustomChars(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->MaxCustomChars();
}

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
						int nContrast)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->Init(	displayName,
							fontFile,
							bUseFT,
							CharEncoding,
							nPixHeight,
							bShowBigBorder,
							bShowThinBorder,
							bShowDebugFrame,
							nPixShiftX,
							nPixShiftY,
							bUpsideDown,
							bBacklight,
							nContrast);
}

void glcddriverDeInit(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->DeInit();
}

void glcddriverClear(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->Clear();
}

void glcddriverRefresh(GlcdDriver* drv, bool refreshAll)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->Refresh(refreshAll);
}

void glcddriverSetBrightness(GlcdDriver* drv, unsigned int percent)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->SetBrightness(percent);
}

unsigned int glcddriverGetBrightness(GlcdDriver* drv)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->GetBrightness();
}

void glcddriverInvert(GlcdDriver* drv, bool bInvert)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->Invert(bInvert);
}

void glcddriverDrawText(GlcdDriver* drv, int x, int y, char text[])
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->DrawString(x, y, text);
}

void glcddriverDrawCharacter(GlcdDriver* drv, int x, int y, char c)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->DrawChar(x, y, c);
}

int glcddriverDrawIcon(GlcdDriver* drv, int x, int y, int icon)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	return cpp_drv->DrawIcon(x, y, icon);
}

void glcddriverSetCustomChar(GlcdDriver* drv, int pos, char *pixeldata)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->SetCustomChar(pos, pixeldata);
}

void glcddriverDrawBigNum(GlcdDriver* drv, int x, int num)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->DrawBigNum(x, num);
}

void glcddriverDrawBarH(GlcdDriver* drv, int x, int y, int len, int promille, int options)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->DrawBarH(x, y, len, promille, options);
}

void glcddriverDrawBarV(GlcdDriver* drv, int x, int y, int len, int promille, int options)
{
	cglcdDriver* cpp_drv = reinterpret_cast<cglcdDriver*>(drv);
	cpp_drv->DrawBarV(x, y, len, promille, options);
}



#ifdef __cplusplus
}
#endif
