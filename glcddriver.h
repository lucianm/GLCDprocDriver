/*
 * cglcdDriver, a generic driver based on graphlcd-base, to be used
 * in the LCDproc server
 *
 * (c) 2005 - 2010 Lucian Muresan <lucianm AT users.sourceforge.net>
 */

#ifndef _GLCD_DRIVER_H_
#define _GLCD_DRIVER_H_

#include <glcdgraphics/font.h>
#include <glcddrivers/config.h>
#include <glcddrivers/driver.h>
#include <glcddrivers/drivers.h>

#define TOTAL_ICONS 22

class cglcdDriver
{
/*
 * Classes & enums
 */
protected:
	class cIcon
	{
	public:
		cIcon(int nWidth, int nHeight, int nLCDproIdx)
		{
			m_pIconBitmap = new GLCD::cBitmap(nWidth, nHeight);
			m_pIconBitmap->Clear();
			m_nLCDprocIdx = nLCDproIdx;
		};
		~cIcon()
		{
			if (m_pIconBitmap)
				delete m_pIconBitmap;
		};
		int m_nLCDprocIdx;
		GLCD::cBitmap * m_pIconBitmap;
	protected:
	};

/*
 * Attributes
 */
public:
protected:
	GLCD::cDriver * m_pDrv;
	GLCD::cFont * m_pFont;
	GLCD::cBitmap * m_pBitmap;
	GLCD::cFont * m_pFntIcons;
	cIcon * m_arrIcons[TOTAL_ICONS];
	int m_nOffsetX;
	int m_nOffsetY;
	bool m_bShowBigBorder;
	bool m_bShowThinBorder;
	bool m_bShowDebugFrame;
	int m_nPixShiftX;
	int m_nPixShiftY;
	int m_nTextWidth;
	int m_nTextHeight;
	int m_nBrightness;
	int m_bInvert;
	GLCD::cDriverConfig * m_pMyDrvConfig;
	GLCD::cBitmap * m_arrBigNums[10];

private:

/*
 * Methods
 */
public:
	cglcdDriver();
	~cglcdDriver();

	int TextWidth() const;
	int TextHeight() const;
	int CellWidth() const;
	int CellHeight() const;
	bool Init(	const std::string DisplayName,
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
				int nContrast);
	void DeInit();
	void Clear();
	void DrawChar(int x, int y, char data);
	void DrawString(int x, int y, const std::string text);
	void Refresh(bool refreshAll = false);
	void SetBrightness(unsigned int percent);
	unsigned int GetBrightness() const;
	int DrawIcon(int x, int y, int icon);
	int MaxCustomChars() const;
	void Invert(bool bInvert = true);
	void SetCustomChar(int pos, char *pixeldata);
	void DrawBigNum(int x, int num);
	void DrawBarH(int x, int y, int len, int promille, int options);
	void DrawBarV(int x, int y, int len, int promille, int options);
protected:
	void IconsInitialize();
	int IconIdxLocal(int nLCDprocIdx);
	void IconScaleDown(	unsigned char * pPrimaryData,
									int nHeight,
									int nWidth,
									GLCD::cBitmap * pIcon,
									bool bLeftBottomBorder = false);
	void CalculateDimensions();
	uint32_t Inversion(const uint32_t color) const;
	void BigNumsInitialize(const std::string FontFile, bool bUseFT);
private:
};


#endif
