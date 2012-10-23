/*
 * cglcdDriver, a generic driver based on graphlcd-base, to be used
 * in the LCDproc server
 *
 * (c) 2005-2010 Lucian Muresan <lucianm AT users.sourceforge.net>
 */
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include "glcddriver.h"
#include "icons.h"


/*
 *
 * The following icon IDs are taken from lcdproc/drivers/lcd.h
 *
 */

/* Icons. If a driver does not support an icon, it can return -1 from the
 * icon function, and let the core place a replacement character.
 */
/* Icons below are one character wide */
#define ICON_BLOCK_FILLED	0x100
#define ICON_HEART_OPEN		0x108
#define ICON_HEART_FILLED	0x109
#define ICON_ARROW_UP		0x110
#define ICON_ARROW_DOWN		0x111
#define ICON_ARROW_LEFT		0x112
#define ICON_ARROW_RIGHT	0x113
#define ICON_CHECKBOX_OFF	0x120
#define ICON_CHECKBOX_ON	0x121
#define ICON_CHECKBOX_GRAY	0x122
#define ICON_SELECTOR_AT_LEFT	0x128
#define ICON_SELECTOR_AT_RIGHT	0x129
#define ICON_ELLIPSIS		0x130

/* Icons below are two characters wide */
#define ICON_STOP		0x200	/* should look like  []  */
#define ICON_PAUSE		0x201	/* should look like  ||  */
#define ICON_PLAY		0x202	/* should look like  >   */
#define ICON_PLAYR		0x203	/* should llok like  <   */
#define ICON_FF			0x204	/* should look like  >>  */
#define ICON_FR			0x205	/* should look like  <<  */
#define ICON_NEXT		0x206	/* should look like  >|  */
#define ICON_PREV		0x207	/* should look like  |<  */
#define ICON_REC		0x208	/* should look like  ()  */


cglcdDriver::cglcdDriver()
{
	m_pBitmap = 0;
	m_pDrv = 0;
	m_pFont = 0;
	m_pFntIcons = 0;

	int i;

	for (i = 0; i < TOTAL_ICONS; i++)
		m_arrIcons[i] = 0;

	for (i = 0; i < 10; i++)
		m_arrBigNums[i] = 0;

	m_nOffsetX = 0;
	m_nOffsetY = 0;
	m_bShowBigBorder = false;
	m_bShowThinBorder = false;
	m_bShowDebugFrame = false;
	m_nPixShiftX = 0;
	m_nPixShiftY = 0;
	m_nTextWidth = 0;
	m_nTextHeight = 0;
	m_nBrightness = 100; // percent
	m_bInvert = false;
	m_pMyDrvConfig = 0;
}

cglcdDriver::~cglcdDriver()
{
	DeInit();
	if (m_pDrv)
		delete m_pDrv;
	int i;
	for (i = 0; i < TOTAL_ICONS; i++)
		delete m_arrIcons[i];
	for (i = 0; i < 10; i++)
		delete m_arrBigNums[i];
	delete m_pMyDrvConfig;
}

int cglcdDriver::TextWidth() const
{
	return m_nTextWidth;
}

int cglcdDriver::TextHeight() const
{
	return m_nTextHeight;
}

int cglcdDriver::CellWidth() const
{
	if (m_pFont)
		return m_pFont->TotalWidth();
	else
		return 0;
}

int cglcdDriver::CellHeight() const
{
	if (m_pFont)
		return m_pFont->TotalHeight();
	else
		return 0;
}

void cglcdDriver::CalculateDimensions()
{
	if (m_pDrv && m_pFont)
	{
		m_nTextWidth = m_pDrv->Width()/m_pFont->TotalWidth();
		m_nTextHeight = m_pDrv->Height()/m_pFont->TotalHeight();
	}
	else
	{
		m_nTextWidth = 0;
		m_nTextHeight = 0;
	}
}

int cglcdDriver::MaxCustomChars() const
{
	// we will store custom chars at the first 30 positions of the
	// ASCII table in m_pFont
	return 30;
}

bool cglcdDriver::Init(	const std::string DisplayName,
						const std::string FontFile,
						bool bUseFT,
						const std::string CharEncoding,
						int nPixHeight,
						bool bShowBigBorder,
						bool bShowThinBorder,
						bool bShowDebugFrame,
						int nPixShiftX,
						int nPixShiftY,
						bool bUpsideDown,
						bool bBacklight,
						int nContrast)
{
	m_bShowBigBorder = bShowBigBorder;
	m_bShowThinBorder = bShowThinBorder;
	m_bShowDebugFrame = bShowDebugFrame;

	syslog(	LOG_DEBUG,
		"Initializing '%s-%d.%d.%.d%s' for the 'glcdlib' meta driver...\n",
		GLCDPROC_MODULENAME, GLCDPROC_VER_MAJOR, GLCDPROC_VER_MINOR, GLCDPROC_VER_MICRO, GLCDPROC_VER_SUPPL );

	if (GLCD::Config.Load("/etc/graphlcd.conf") == false)
	{
		syslog(LOG_ERR, "ERROR loading default graphlcd config file '/etc/graphlcd.conf'!\n");
		return false;
	}

	int displayNumber = 0;

	if (GLCD::Config.driverConfigs.size() > 0)
	{
		if (DisplayName.length() > 0)
		{
			displayNumber = GLCD::Config.GetConfigIndex(DisplayName.c_str());
			if (displayNumber == -1)
			{
				syslog(LOG_ERR, "ERROR: Specified display '%s' not found in config file!\n", DisplayName.c_str());
				return false;
			}
		}
		else
		{
			syslog(LOG_DEBUG, "WARNING: No display specified, using first one.\n");
			displayNumber = 0;
		}
	}
	else
	{
		syslog(LOG_ERR, "ERROR: No displays specified in config file!\n");
		return false;
	}

	if (FontFile.length() == 0)
	{
		syslog(LOG_ERR, "ERROR: No font file specified!\n");
		return false;
	}

	if (m_pDrv)
		delete m_pDrv;
	// override some configuration with ours
	m_pMyDrvConfig = new GLCD::cDriverConfig(GLCD::Config.driverConfigs[displayNumber]);
	m_pMyDrvConfig->upsideDown = bUpsideDown;
	m_pMyDrvConfig->contrast = nContrast;
	m_pMyDrvConfig->backlight = bBacklight;
	// create the driver
	m_pDrv = GLCD::CreateDriver(GLCD::Config.driverConfigs[displayNumber].id, m_pMyDrvConfig);
	m_pDrv->Init();
	syslog(LOG_DEBUG, "cglcdDriver::Init: driver '%s' initialized!\n", DisplayName.c_str());

	if (m_pFont)
		delete m_pFont;
	m_pFont = new GLCD::cFont();

	if (bUseFT)
	{
		if (!m_pFont->LoadFT2(FontFile.c_str(), CharEncoding.c_str(), nPixHeight, false))
		{
			syslog(LOG_ERR, "could not load font file '%s'!\n", FontFile.c_str());
			return false;
		}
	}
	else
	{
		if (!m_pFont->LoadFNT(FontFile.c_str()))
		{
			syslog(LOG_ERR, "could not load font file '%s'!\n", FontFile.c_str());
			return false;
		}
	}

	if (m_pBitmap)
		delete m_pBitmap;
	m_pBitmap = new GLCD::cBitmap(m_pDrv->Width(), m_pDrv->Height());

	CalculateDimensions();
	syslog(LOG_DEBUG, "cglcdDriver::Init: Text Cols x Rows adjusted to %d x %d\n", m_nTextWidth, m_nTextHeight);

	m_nOffsetX = (m_pDrv->Width() - TextWidth() * m_pFont->TotalWidth())/2;
	m_nOffsetY = (m_pDrv->Height() - TextHeight() * m_pFont->TotalHeight())/2;
	syslog(LOG_DEBUG, "cglcdDriver::Init: m_nOffsetX = %d; m_nOffsetY = %d\n", m_nOffsetX, m_nOffsetY);

	if (abs(nPixShiftX) > m_nOffsetX)
	{
		m_nPixShiftX = m_nOffsetX * nPixShiftX / abs(nPixShiftX);
		syslog(
			LOG_ERR,
			"ERROR (non fatal): Specified PixShiftX = %d had to be cut down to %d!\n",
			nPixShiftX,
			m_nPixShiftX);
	}
	else
	{
		m_nPixShiftX = nPixShiftX;
	}

	if (abs(nPixShiftY) > m_nOffsetY)
	{
		m_nPixShiftY = m_nOffsetY * nPixShiftY / abs(nPixShiftY);
		syslog(
			LOG_ERR,
			"ERROR (non fatal): Specified PixShiftY = %d had to be cut down to %d!\n",
			nPixShiftY,
			m_nPixShiftY);
	}
	else
	{
		m_nPixShiftY = nPixShiftY;
	}

	// moved from our own Clear method
	if(m_pDrv)
		m_pDrv->Clear();
	Clear();

	IconsInitialize();

	BigNumsInitialize(FontFile.c_str(), bUseFT);

	syslog(LOG_DEBUG, "cglcdDriver for LCDproc initialized successfully\n");

    return true;
}

void cglcdDriver::DeInit()
{
	if (m_pDrv)
		m_pDrv->DeInit();
	if (m_pBitmap)
		delete m_pBitmap;
	if (m_pFont)
		delete m_pFont;
	if (m_pFntIcons)
		delete m_pFntIcons;
}

void cglcdDriver::Clear()
{
//	if(m_pDrv)
//		m_pDrv->Clear();
	if (m_pBitmap)
	{
		m_pBitmap->Clear();

		// invert display?
		if (m_bInvert)
		{
			m_pBitmap->DrawRectangle(
				0,
				0,
				m_pDrv->Width(),
				m_pDrv->Height(),
				GLCD::cColor::White,
				true);
		}

		// have smaller actual draw area?
		if (		m_nOffsetX > 0
				&&	m_nOffsetY > 0)
		{
			// show border ?
			if (m_bShowBigBorder)
			{
				int i;
				// left vertical border
				for (i = 0; i < m_nOffsetX + m_nPixShiftX; i++)
					m_pBitmap->DrawLine(i, 0, i, m_pDrv->Height(), Inversion(GLCD::cColor::White));
				// right vertical border
				for (i = m_pDrv->Width() - m_nOffsetX + m_nPixShiftX; i < m_pDrv->Width(); i++)
					m_pBitmap->DrawLine(i, 0, i, m_pDrv->Height(), Inversion(GLCD::cColor::White));
				// upper horizontal border
				for (i = 0; i < m_nOffsetY + m_nPixShiftY; i++)
					m_pBitmap->DrawLine(m_nOffsetX + m_nPixShiftX, i, m_pDrv->Width() - m_nOffsetX + m_nPixShiftX - 1, i, Inversion(GLCD::cColor::White));
				// lower horizontal border
				for (i = m_pDrv->Height() - m_nOffsetY + m_nPixShiftY; i < m_pDrv->Height(); i++)
					m_pBitmap->DrawLine(m_nOffsetX + m_nPixShiftX, i, m_pDrv->Width() - m_nOffsetX + m_nPixShiftX - 1, i, Inversion(GLCD::cColor::White));
			}

		}
		// show debugging frame?
		if (m_bShowDebugFrame)
		{
			m_pBitmap->DrawRectangle(
				m_nOffsetX + m_nPixShiftX,
				m_nOffsetY + m_nPixShiftY,
				m_pDrv->Width() - m_nOffsetX + m_nPixShiftX - 1,
				m_pDrv->Height() - m_nOffsetY + m_nPixShiftY - 1,
				Inversion(GLCD::cColor::White),
				false);
		}
	}
}

void cglcdDriver::DrawChar(int x, int y, char data)
{
	if (m_pBitmap && m_pFont)
		m_pBitmap->DrawCharacter(
			m_nOffsetX + m_nPixShiftX + x*m_pFont->TotalWidth(),
			m_nOffsetY + m_nPixShiftY + y*m_pFont->TotalHeight(),
			m_pBitmap->Width() - 1,
			data,
			m_pFont,
			Inversion(GLCD::cColor::White),
			Inversion(GLCD::cColor::Black),
			1
			);
}

void cglcdDriver::DrawString(int x, int y, const std::string text)
{
	if (m_pBitmap && m_pFont)
		m_pBitmap->DrawText(
			m_nOffsetX + m_nPixShiftX + x*m_pFont->TotalWidth(),
			m_nOffsetY + m_nPixShiftY + y*m_pFont->TotalHeight(),
			m_pBitmap->Width() - 1,
			text,
			m_pFont,
			Inversion(GLCD::cColor::White),
			Inversion(GLCD::cColor::Black),
			false,
			1
			);
}

void cglcdDriver::Refresh(bool refreshAll)
{
	if (m_pDrv && m_pBitmap)
	{
		m_pDrv->SetScreen(m_pBitmap->Data(), m_pBitmap->Width(), m_pBitmap->Height()); //, m_pBitmap->LineSize());
		m_pDrv->Refresh(refreshAll);
	}
}

void cglcdDriver::SetBrightness(unsigned int percent)
{
	if (m_pDrv)
	{
		m_nBrightness = percent;
		m_pDrv->SetBrightness(m_nBrightness);
	}
}

unsigned int cglcdDriver::GetBrightness() const
{
	return m_nBrightness;
}

int cglcdDriver::DrawIcon(int x, int y, int icon)
{
	int nLocalIdx = IconIdxLocal(icon);

	if (	m_pBitmap &&
			nLocalIdx != -1)
	{
		m_pBitmap->DrawBitmap(
			m_nOffsetX + m_nPixShiftX + x*m_pFont->TotalWidth(),
			m_nOffsetY + m_nPixShiftY + y*m_pFont->TotalHeight(),
			*m_arrIcons[nLocalIdx]->m_pIconBitmap,
			Inversion(GLCD::cColor::White));
		return 0;
	}
	else
	{
		// all the others
		return -1;
	}
}

void cglcdDriver::IconsInitialize()
{
	if (!m_pFont)
	{
		syslog(LOG_ERR, "ERROR: cglcdDriver::IconsInitialize() --> main font not yet created !!!!\n");
	}
	// alloc memory for the icons
	int arrIndexes[TOTAL_ICONS];
	/* Icons below are one character wide */
	arrIndexes[0] = ICON_BLOCK_FILLED;
	arrIndexes[1] = ICON_HEART_OPEN;
	arrIndexes[2] = ICON_HEART_FILLED;
	arrIndexes[3] = ICON_ARROW_UP;
	arrIndexes[4] = ICON_ARROW_DOWN;
	arrIndexes[5] = ICON_ARROW_LEFT;
	arrIndexes[6] = ICON_ARROW_RIGHT;
	arrIndexes[7] = ICON_CHECKBOX_OFF;
	arrIndexes[8] = ICON_CHECKBOX_ON;
	arrIndexes[9] = ICON_CHECKBOX_GRAY;
	arrIndexes[10] = ICON_SELECTOR_AT_LEFT;
	arrIndexes[11] = ICON_SELECTOR_AT_RIGHT;
	arrIndexes[12] = ICON_ELLIPSIS;
	/* Icons below are two characters wide */
	arrIndexes[13] = ICON_STOP;
	arrIndexes[14] = ICON_PAUSE;
	arrIndexes[15] = ICON_PLAY;
	arrIndexes[16] = ICON_PLAYR;
	arrIndexes[17] = ICON_FF;
	arrIndexes[18] = ICON_FR;
	arrIndexes[19] = ICON_NEXT;
	arrIndexes[20] = ICON_PREV;
	arrIndexes[21] = ICON_REC;

	for (int i = 0; i < TOTAL_ICONS; i++)
	{
		/* two characters wide */
		int nWidthFactor = 1;
		if (	arrIndexes[i] == ICON_STOP ||
				arrIndexes[i] == ICON_PAUSE ||
				arrIndexes[i] == ICON_PLAY ||
				arrIndexes[i] == ICON_PLAYR ||
				arrIndexes[i] == ICON_FF ||
				arrIndexes[i] == ICON_FR ||
				arrIndexes[i] == ICON_NEXT ||
				arrIndexes[i] == ICON_PREV ||
				arrIndexes[i] == ICON_REC )
		{
			nWidthFactor = 2;
		}
		m_arrIcons[i] = new cIcon(	m_pFont->TotalWidth() * nWidthFactor,
									m_pFont->TotalHeight(),
									arrIndexes[i]);
	}

	bool bLeftBottomBorder = true;

	GLCD::cBitmap * pIcon = m_arrIcons[IconIdxLocal(ICON_BLOCK_FILLED)]->m_pIconBitmap;
	int nBlockLeftBottom = bLeftBottomBorder ? 2 : 1;
	pIcon->DrawRectangle( 0, 0, pIcon->Width() - nBlockLeftBottom, pIcon->Height() - nBlockLeftBottom, GLCD::cColor::White, true);

	IconScaleDown(	icon_heart_open, 73, 54,
				m_arrIcons[IconIdxLocal(ICON_HEART_OPEN)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_heart_filled, 73, 54,
				m_arrIcons[IconIdxLocal(ICON_HEART_FILLED)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_arrow_up, 73, 47,
				m_arrIcons[IconIdxLocal(ICON_ARROW_UP)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_arrow_down, 73, 47,
				m_arrIcons[IconIdxLocal(ICON_ARROW_DOWN)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_arrow_left, 73, 58,
				m_arrIcons[IconIdxLocal(ICON_ARROW_LEFT)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_arrow_right, 73, 58,
				m_arrIcons[IconIdxLocal(ICON_ARROW_RIGHT)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_checkbox_off, 73, 58,
				m_arrIcons[IconIdxLocal(ICON_CHECKBOX_OFF)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_checkbox_on, 73, 58,
				m_arrIcons[IconIdxLocal(ICON_CHECKBOX_ON)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_checkbox_gray, 73, 58,
				m_arrIcons[IconIdxLocal(ICON_CHECKBOX_GRAY)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_selector_at_left, 73, 47,
				m_arrIcons[IconIdxLocal(ICON_SELECTOR_AT_LEFT)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_selector_at_right, 73, 47,
				m_arrIcons[IconIdxLocal(ICON_SELECTOR_AT_RIGHT)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_ellipsis, 73, 47,
				m_arrIcons[IconIdxLocal(ICON_ELLIPSIS)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_stop, 73, 41,
				m_arrIcons[IconIdxLocal(ICON_STOP)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_pause, 73, 27,
				m_arrIcons[IconIdxLocal(ICON_PAUSE)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_play, 73, 23,
				m_arrIcons[IconIdxLocal(ICON_PLAY)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_play_reversed, 73, 23,
				m_arrIcons[IconIdxLocal(ICON_PLAYR)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_fast_forward, 73, 48,
				m_arrIcons[IconIdxLocal(ICON_FF)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_fast_rewind, 73, 48,
				m_arrIcons[IconIdxLocal(ICON_FR)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_next, 73, 54,
				m_arrIcons[IconIdxLocal(ICON_NEXT)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_prev, 73, 54,
				m_arrIcons[IconIdxLocal(ICON_PREV)]->m_pIconBitmap, bLeftBottomBorder);

	IconScaleDown(	icon_record, 73, 41,
				m_arrIcons[IconIdxLocal(ICON_REC)]->m_pIconBitmap, bLeftBottomBorder);

}

int cglcdDriver::IconIdxLocal(int nLCDprocIdx)
{
	for (int i = 0; i < TOTAL_ICONS; i++)
	{
		if (m_arrIcons[i]->m_nLCDprocIdx == nLCDprocIdx)
			return i;
	}
	syslog(LOG_ERR, "ERROR: cglcdDriver::IconIdxLocal() --> icon index not found !!!!\n");
	return -1;
}

void cglcdDriver::Invert(bool bInvert)
{
	m_bInvert = bInvert;
}

uint32_t cglcdDriver::Inversion(const uint32_t color) const
{
	if (m_bInvert)
	{
		// invert colours
		if (color == GLCD::cColor::Black)
		{
			return GLCD::cColor::White;
		} else
		{
			return GLCD::cColor::Black;
		} 
	}
	else
	{
		// no inversion
		return color;
	}
}

void cglcdDriver::SetCustomChar(int pos, char *pixeldata)
{
	if (pos < MaxCustomChars())
	{
		GLCD::cBitmap * pCustomChar =
			new GLCD::cBitmap(m_pFont->TotalWidth(), m_pFont->TotalHeight());
		pCustomChar->Clear();
		for (int y = 0; y < pCustomChar->Height(); y++)
		{
			for (int x = 0; x < pCustomChar->Width(); x++)
			{
				if (pixeldata[(y * pCustomChar->Width()) + x] != 0)
				{
					pCustomChar->DrawPixel(x, y, GLCD::cColor::White);
				}
			}
		}
		m_pFont->SetCharacter(pos, pCustomChar);
	}
}

void cglcdDriver::BigNumsInitialize(const std::string FontFile, bool bUseFT)
{
	GLCD::cFont bnFont;
	bool bFontLoaded = false;
	if (bUseFT)
	{
	    bFontLoaded = bnFont.LoadFT2(FontFile.c_str(), "iso8859-1", 2 * m_pFont->TotalHeight(), false);
	}
	else
	{
	    bFontLoaded = bnFont.LoadFNT(FontFile.c_str());
	}
	
	if (!bFontLoaded)
	{
	    syslog(LOG_ERR, "ERROR: cglcdDriver::BigNumsInitialize(...) --> big nums font not loaded !!!!\n");
	    //return;
	}

	char tmp[2];
	GLCD::cBitmap * pNumChar = 0;
	if (!bUseFT)
	{
		pNumChar = new GLCD::cBitmap(2 * m_pFont->TotalWidth(), 2 * m_pFont->TotalHeight());
	}
	int x, y;
	for (int i = 0; i < 10; i++)
	{
		GLCD::cBitmap * pBigNum =
			new GLCD::cBitmap(2 * m_pFont->TotalWidth(), 2 * m_pFont->TotalHeight());
		pBigNum->Clear();
		sprintf(tmp, "%d", i);
		if (bUseFT)
		{
			pNumChar = (GLCD::cBitmap *)bnFont.GetCharacter(tmp[0]);
		}
		else
		{
			// hack: double the pixels on both directions
			pNumChar->Clear();
			for (y = 0; y < m_pFont->TotalHeight(); y++)
			{
				for (x = 0; x < m_pFont->TotalWidth(); x++)
				{
					if (bnFont.GetCharacter(tmp[0])->GetPixel(x,y))
					{
						pNumChar->DrawPixel(2*x, 2*y, GLCD::cColor::White);
						pNumChar->DrawPixel(2*x + 1, 2*y, GLCD::cColor::White);
						pNumChar->DrawPixel(2*x, 2*y + 1, GLCD::cColor::White);
						pNumChar->DrawPixel(2*x + 1, 2*y + 1, GLCD::cColor::White);
					}
				}
			}
		}
		pBigNum->DrawBitmap(0, 0, *pNumChar, GLCD::cColor::White);
		m_arrBigNums[i] = pBigNum;
	}
	if (!bUseFT)
	{
		delete pNumChar;
	}
}

void cglcdDriver::DrawBigNum(int x, int num)
{
	if (m_pBitmap && m_arrBigNums[num])
	{
		int y = (TextHeight() - 2)/2;
		m_pBitmap->DrawBitmap(
			m_nOffsetX + m_nPixShiftX + x*m_pFont->TotalWidth(),
			m_nOffsetY + m_nPixShiftY + y*m_pFont->TotalHeight(),
			*m_arrBigNums[num],
			Inversion(GLCD::cColor::White)
			);
	}
}

void cglcdDriver::DrawBarH(int x, int y, int len, int promille, int options)
{
	// adapted from lcd_lib static lib_hbar_static
	int total_pixels  = ((long) 2 * len * m_pFont->TotalWidth() + 1 ) * promille / 2000;
	int pos;

	if (m_pBitmap)
	{
		for (pos = 0; pos < len; pos ++ )
		{
			int pixels = total_pixels - m_pFont->TotalWidth() * pos;

			if( pixels > 0 )
			{
				// write a partial block...
				m_pBitmap->DrawRectangle(
					m_nOffsetX + m_nPixShiftX + (x + pos)*m_pFont->TotalWidth(),
					m_nOffsetY + m_nPixShiftY + y*m_pFont->TotalHeight(),
					m_nOffsetX + m_nPixShiftX + (x + pos)*m_pFont->TotalWidth() + pixels - 2,
					m_nOffsetY + m_nPixShiftY + (y + 1)*m_pFont->TotalHeight() - 2,
					Inversion(GLCD::cColor::White),
					true);
				break;
			}
		}
	}
}

void cglcdDriver::DrawBarV(int x, int y, int len, int promille, int options)
{
	// adapted from lcd_lib static lib_vbar_static
	int total_pixels  = ((long) 2 * len * m_pFont->TotalHeight() + 1 ) * promille / 2000;
	int pos;

	if (m_pBitmap)
	{
		for (pos = 0; pos < len; pos ++ )
		{
			int pixels = total_pixels - m_pFont->TotalHeight() * pos;

			if( pixels > 0 )
			{
				//write a partial block...
				m_pBitmap->DrawRectangle(
					m_nOffsetX + m_nPixShiftX + x*m_pFont->TotalWidth(),
					m_nOffsetY + m_nPixShiftY + (y - pos + 1)*m_pFont->TotalHeight() - pixels,
					m_nOffsetX + m_nPixShiftX + (x + 1)*m_pFont->TotalWidth() - 2,
					m_nOffsetY + m_nPixShiftY + (y - pos + 1)*m_pFont->TotalHeight() - 2,
					Inversion(GLCD::cColor::White),
					true);
				break;
			}
		}
	}
}

void cglcdDriver::IconScaleDown(	unsigned char * pPrimaryData,
									int nHeight,
									int nWidth,
									GLCD::cBitmap * pIcon,
									bool bLeftBottomBorder )
{
	int nCorrLeftBottom = bLeftBottomBorder ? 1 : 0;
	double dQuant = 1.0/((double)nHeight/(double)(pIcon->Height() - nCorrLeftBottom) * (double)nWidth/(double)(pIcon->Width() - nCorrLeftBottom));
	//syslog(LOG_INFO, "INFO: cglcdDriver::IconScaleDown(): dQuant = %f\n", dQuant);


	double tmp[pIcon->Height()][pIcon->Width()];
	int x, y;
	for (y = 0; y < pIcon->Height(); y++)
	{
		for (x = 0; x < pIcon->Width(); x++)
		{
			tmp[y][x] = 0.0;
		}
	}
	
	for (int j = 0; j < nHeight; j++)
	{
		for (int i = 0; i < nWidth; i++)
		{
			// destination indices
			x = i*(pIcon->Width() - nCorrLeftBottom)/nWidth;
			y = j*(pIcon->Height() - nCorrLeftBottom)/nHeight;
			if (pPrimaryData[(j * nWidth) + i])
			{
				tmp[y][x] += dQuant;
			}
		}
	}

	for (y = 0; y < pIcon->Height(); y++)
	{
		for (x = 0; x < pIcon->Width(); x++)
		{
			//syslog(LOG_INFO, "INFO: cglcdDriver::IconScaleDown(): tmp[%i][%i] = %f\n", y, x, tmp[y][x]);
			if (tmp[y][x] >= 0.5)
			//if (tmp[y][x] != 0.0)
			{
				pIcon->DrawPixel(x, y, GLCD::cColor::White);
			}
		}
	}
}

