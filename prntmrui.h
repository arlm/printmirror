
/*
   PrintMirror extracts individual page metafiles from Spool File.
   Copyright (C) 2002  V Aravind

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   
   Contact at vprnt@lycos.com
 */

#define WINVER 0x0500
#define _WIN32_WINNT  0x0500
#define _X86_
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdio.h>
#include <winddiui.h>
#ifdef _DEBUG 
#define DUMPMSG(msg) { if(msg) {OutputDebugStringA(msg); \
     OutputDebugStringA("\r\n"); } }  // I  am doing  this   to  flush.
#else
#define DUMPMSG(msg)   
#endif
#include <prsht.h>
#include "resourceui.h"
#include <malloc.h>
#define PDEV_ESCAPE 0x303eb8efU
#define GDIINFO_ESCAPE 0x303eb9efU
#define MALLOC malloc
DWORD  DrvDeviceCapabilities(HANDLE  hPrinter,PWSTR  pDeviceName, WORD  iDevCap,
        VOID  *pvOutput,DEVMODE  *pDevMode);
enum Layout { UP1 ,UP2,UP4,UP6,UP9,BOOKLET};
typedef LONG  LDECI4;
typedef struct _CIECHROMA
{
     LDECI4   x;
     LDECI4   y;
     LDECI4   Y;
}CIECHROMA;
struct PageSize{
     WORD dmPaperSize;
     POINT pt;
};
typedef struct _COLORINFO
{
     CIECHROMA  Red;
     CIECHROMA  Green;
     CIECHROMA  Blue;
     CIECHROMA  Cyan;
     CIECHROMA  Magenta;
     CIECHROMA  Yellow;
     CIECHROMA  AlignmentWhite;

     LDECI4  RedGamma;
     LDECI4  GreenGamma;
     LDECI4  BlueGamma;

     LDECI4  MagentaInCyanDye;
     LDECI4  YellowInCyanDye;
     LDECI4  CyanInMagentaDye;
     LDECI4  YellowInMagentaDye;
     LDECI4  CyanInYellowDye;
     LDECI4  MagentaInYellowDye;
}COLORINFO, *PCOLORINFO;

typedef struct _GDIINFO
{
    ULONG ulVersion;
    ULONG ulTechnology;
    ULONG ulHorzSize;
    ULONG ulVertSize;
    ULONG ulHorzRes;
    ULONG ulVertRes;
    ULONG cBitsPixel;
    ULONG cPlanes;
    ULONG ulNumColors;
    ULONG flRaster;
    ULONG ulLogPixelsX;
    ULONG ulLogPixelsY;
    ULONG flTextCaps;

    ULONG ulDACRed;
    ULONG ulDACGreen;
    ULONG ulDACBlue;

    ULONG ulAspectX;
    ULONG ulAspectY;
    ULONG ulAspectXY;

    LONG  xStyleStep;
    LONG  yStyleStep;
    LONG  denStyleStep;

    POINTL ptlPhysOffset;
    SIZEL  szlPhysSize;

    ULONG ulNumPalReg;

// These fields are for halftone initialization.

    COLORINFO ciDevice;
    ULONG     ulDevicePelsDPI;
    ULONG     ulPrimaryOrder;
    ULONG     ulHTPatternSize;
    ULONG     ulHTOutputFormat;
    ULONG     flHTFlags;

    ULONG ulVRefresh;
    ULONG ulBltAlignment;

    ULONG ulPanningHorzRes;
    ULONG ulPanningVertRes;
    ULONG xPanningAlignment;
    ULONG yPanningAlignment;

// The following fields are for user defined halftone dither patterns.  These
// fields are only checked if ulHTPatternSize is eqaul to HT_PATSIZE_USER.
//
// The user defined pHTPatA, pHTPatB, pHTPatC pointers correspond to the primary
// color order defined in ulPrimaryOrder as PRIMARY_ORDER_xxx.
//
// The size of halftone dither pattern must range from 4 to 256.  For each
// dither pattern, pHTPatA, pHTPatB, pHTPatC must point to a
// valid byte array of (cxHTPat x cyHTPat) size.  pHTPatA, pHTPatB and
// pHTPatC may point to the same dither pattern array.
//
// Each byte threshold within the dither pattern defines the additive
// intensity threshold of pixels.  A zero threshold value indicates the pixel
// location is ignored (always black), while 1 to 255 threshold values give the
// dither pattern 255 level of grays.

    ULONG   cxHTPat;    // cxHTPat must range from 4-256
    ULONG   cyHTPat;    // cyHTPat must range from 4-256
    LPBYTE  pHTPatA;    // for Primary Color Order A
    LPBYTE  pHTPatB;    // for Primary Color Order B
    LPBYTE  pHTPatC;    // for Primary Color Order C

// Shade and blend caps

    ULONG   flShadeBlend;

    ULONG   ulPhysicalPixelCharacteristics;
    ULONG   ulPhysicalPixelGamma;

} GDIINFO, *PGDIINFO;
struct VPDEVMODE{
     WCHAR PrinterName[256];
     Layout lyt;
     WORD PaperSize;
     GDIINFO  gi;
     int numcolors;
     ULONG Palette[256];
     LOGFONT lf;
	 BOOL Preview;
	 BOOL PrintToPaper;
     BYTE CompLevel;
};
struct VDEVMODE{
     DEVMODEW dm;
     VPDEVMODE pdm;
};
     struct DEVDATA{
          DWORD dwJobId;
          int Pages;
          WCHAR *pSpoolFileName;
          HANDLE hPDriver;
          BOOL *pResetDC;
		  HBITMAP hBitmap;
		  LPVOID pvBits;
          VDEVMODE *pCurDevmode;
          LPTSTR pDocument;
     };
#define PPDEV DEVDATA*

struct VPrinterSettings {
     WCHAR PrinterName[256];
     VDEVMODE *inDevmode;
     VDEVMODE *ValidDevMode;
     VDEVMODE *outDevmode;
     PFNCOMPROPSHEET  pfnComPropSheet;
     HANDLE  hComPropSheet;
     HANDLE handle;
     HANDLE hPrinter;
     LPTSTR pszPrinterName; 
     DWORD fMode;
     Layout lyt;
     BYTE  Render;
     WORD PaperSize;
     WCHAR *Test;
	 BOOL DimensionUnits;
	 short PaperOrient;
	 BOOL Preview;
	 BOOL PrintToPaper;
     BYTE CompLevel;
};


struct Token{
     int TokenStart;
     int TokenEnd;
     Token *NextToken;
};

BOOL  IsXP();
#include "pmversion.h"
#include "windows.h"
#include "windowsx.h"
#include "vfw.h"
#include <process.h>
#include <tlhelp32.h>
#include <shlobj.h>
#ifndef PRNTMRUI
extern HMODULE hModDLL;
#endif


 
BOOL DonotSharePrinterNT( LPTSTR szPrinterName, LPTSTR szShareName, BOOL bShare );
void ValidateSetRealDriver(WCHAR *RealDriverName);
void GetPrintMirrorName(WCHAR PrintMirrorName[]);
BOOL IsSpooler();
BOOL IsExplorer();
VOID FillDeviceCaps(HDC hDC, GDIINFO *pGDIInfo , VDEVMODE *pbIn);
HANDLE GetPrinterInfo(LPBYTE *pPrinterInfo , WCHAR PrinterName[],BOOL check = FALSE);
DWORD GetRealDriverName(WCHAR PrinterName[], WCHAR RealDriverName[]);
DWORD GetRealDriverName(HANDLE hPrinter, WCHAR RealDriverName[]);
DWORD SetRealDriverName(WCHAR PrinterName[], WCHAR RealDriverName[]);
TCHAR *GetTempFile(TCHAR *TempPath , TCHAR *Prefix,TCHAR *TempFileName);
void InstalledPrinter(WCHAR *pInitialDriverName ,LPTSTR pPrinterName );
BOOL IsInchDimensions();



void GetMetaFileFromSpoolFile(TCHAR *SpoolFileName , int PageNbr , TCHAR *MetaFileName, PPDEV pPDev,LPBYTE *pDevmode);
void GetSpoolFileName(DWORD JobId, TCHAR SpoolFileName[],HANDLE hDriver);
void SaveAsBitmap(HWND hDlg , OPENFILENAME ofn , PPDEV pPDev);
void PrintToPaper(PPDEV pPDev);

class PMUIDriver{

     BOOL bIsExplorer;
     BOOL FirstTime;
     VDEVMODE DllDevmode;
     PPDEV pPDevG;
     TCHAR PrinterName[MAX_PATH]; 
     WCHAR RealPrinterName[256];
     LONG DrvDocumentProperties(HWND hwnd, HANDLE hPrinter, PWSTR lpszDeviceName,
             PDEVMODEW pdmOutput,PDEVMODEW pdmInput, DWORD fMode,BOOL fromApp = FALSE);
     void FixUpDevmode(IN HANDLE hPrinter , IN DEVMODE *pbIn, IN OUT PULONG pbOut);
    public:
     PMUIDriver(BOOL bIsExp){ bIsExplorer = bIsExp; wcscpy(RealPrinterName , L"");}
     DWORD  PMDrvDeviceCapabilities(HANDLE  hPrinter,PWSTR  pDeviceName, WORD  iDevCap,
             VOID  *pvOutput,DEVMODE  *pDevMode);
     INT DrvDocumentEvent( HANDLE  hPrinter, HDC  hdc, int  iEsc, ULONG  cbIn, PULONG  pbIn,
             ULONG  cbOut, PULONG  pbOut); 
     LONG  DrvDocumentPropertySheets(PPROPSHEETUI_INFO  pPSUIInfo, LPARAM  lParam);
     BOOL  DrvPrinterEvent(LPWSTR  pPrinterName, INT  DriverEvent, DWORD  Flags, LPARAM  lParam);
     LONG  DrvDevicePropertySheets(PPROPSHEETUI_INFO  pPSUIInfo, LPARAM  lParam);
     BOOL  DevQueryPrintEx(PDEVQUERYPRINT_INFO  pDQPInfo);
     BOOL  DrvConvertDevMode(LPTSTR  pPrinterName, PDEVMODE  pdmIn, PDEVMODE  pdmOut, PLONG  pcbNeeded,DWORD  fMode);
};
