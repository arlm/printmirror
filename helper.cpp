
/*
   PrintMirror extracts individual page metafiles from Spool File.
   Copyright (C) 2002-2004  Vipin Aravind

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


#include "prntmrui.h"
void GetPrintMirrorName(WCHAR PrintMirrorName[])
{
     DWORD dwReturned,dwNeeded;
     EnumPrinters (PRINTER_ENUM_LOCAL|PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL,
             0, &dwNeeded, &dwReturned) ;

     LPBYTE pinfo4 = (LPBYTE)malloc (dwNeeded) ;

     EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
             NULL, 2, (PBYTE) pinfo4,
             dwNeeded, &dwNeeded, &dwReturned) ;
     PRINTER_INFO_2 *pi = (PRINTER_INFO_2 *)pinfo4;
     for(DWORD i = 0 ; i < dwReturned ; i++)
     {
          if(!wcscmp(pi[i].pDriverName , L"PrintMirror"))
          {
               wcscpy(PrintMirrorName , pi[i].pPrinterName);
               break;
          }
     }
     free(pi);
}

BOOL IsSpooler()
{
     WCHAR FileName[256];  
     GetModuleFileName( NULL,FileName, 256);
     if(wcsstr(FileName , L"spoolsv.exe"))
          return TRUE;
     return FALSE;

}
BOOL IsExplorer()
{
     WCHAR FileName[256];  
     GetModuleFileName( NULL,FileName, 256);
     if(wcsstr(FileName , L"explorer.exe"))
          return TRUE;
     return FALSE;

}


VOID FillDeviceCaps(HDC hDC, GDIINFO *pGDIInfo , VDEVMODE *pbIn)
{

     ZeroMemory(pGDIInfo, sizeof(GDIINFO));

     int logpixelsx , logpixelsy;
     HDC memdc = GetDC(NULL);
     pGDIInfo->ulLogPixelsX =logpixelsx = GetDeviceCaps(hDC , LOGPIXELSX);
     pGDIInfo->ulLogPixelsY =logpixelsy = GetDeviceCaps(hDC , LOGPIXELSY);

     pGDIInfo->ulVersion    = GetDeviceCaps(hDC , DRIVERVERSION) ;
     pGDIInfo->ulTechnology = GetDeviceCaps(hDC , TECHNOLOGY);
     /* In mms */
     pGDIInfo->ulHorzSize = GetDeviceCaps(hDC , HORZSIZE);
     pGDIInfo->ulVertSize = GetDeviceCaps(hDC , VERTSIZE);

     pGDIInfo->ulHorzRes  = GetDeviceCaps(hDC , HORZRES);
     pGDIInfo->ulVertRes  = GetDeviceCaps(hDC , VERTRES);
     pGDIInfo->szlPhysSize.cx  =  pGDIInfo->ulHorzRes + 2 * GetDeviceCaps(hDC , PHYSICALOFFSETX);
     pGDIInfo->szlPhysSize.cy  = pGDIInfo->ulVertRes + 2 * GetDeviceCaps(hDC , PHYSICALOFFSETY);

     pGDIInfo->ptlPhysOffset.x = GetDeviceCaps(hDC , PHYSICALOFFSETX);
     pGDIInfo->ptlPhysOffset.y = GetDeviceCaps(hDC , PHYSICALOFFSETY);


     //
     // Assume the device has a 1:1 aspect ratio
     //


     pGDIInfo->ulAspectX    = 10;//GetDeviceCaps(memdc , ASPECTX);
     pGDIInfo->ulAspectY    = 10;//GetDeviceCaps(memdc , ASPECTY);
     pGDIInfo->ulAspectXY   = 14;//GetDeviceCaps(memdc , ASPECTXY);

     COLORINFO ciDevice= {
          { 6810, 3050,     0 },  // xr, yr, Yr
          { 2260, 6550,     0 },  // xg, yg, Yg
          { 1810,  500,     0 },  // xb, yb, Yb
          { 2000, 2450,     0 },  // xc, yc, Yc
          { 5210, 2100,     0 },  // xm, ym, Ym
          { 4750, 5100,     0 },  // xy, yy, Yy
          { 3324, 3474, 10000 },  // xw, yw, Yw

          10000,                  // R gamma
          10000,                  // G gamma
          10000,                  // B gamma

          1422,  952,             // M/C, Y/C
          787,  495,             // C/M, Y/M
          324,  248              // C/Y, M/Y
     };

     pGDIInfo->ciDevice        = ciDevice;
     pGDIInfo->ulDevicePelsDPI   =   pGDIInfo->ulLogPixelsX;
     pGDIInfo->ulNumPalReg= 0;
#define HT_FORMAT_24BPP         6
#define HT_PATSIZE_16x16_M      15
#define HT_FLAG_HAS_BLACK_DYE  0x00000002
#define PRIMARY_ORDER_ABC       0
     pGDIInfo->ulHTOutputFormat= HT_FORMAT_24BPP;

     pGDIInfo->ulHTPatternSize   =  HT_PATSIZE_16x16_M;
     pGDIInfo->flHTFlags       = HT_FLAG_HAS_BLACK_DYE;
     pGDIInfo->ulPrimaryOrder  = PRIMARY_ORDER_ABC;

     pGDIInfo->ulNumColors =  GetDeviceCaps(hDC , NUMCOLORS);
     pGDIInfo->cBitsPixel = GetDeviceCaps(hDC , BITSPIXEL);
     pGDIInfo->cPlanes    = GetDeviceCaps(hDC , PLANES);
     //
     // Some other information the Engine expects us to fill in.
     //

     pGDIInfo->ulDACRed     = 0;
     pGDIInfo->ulDACGreen   = 0;
     pGDIInfo->ulDACBlue    = 0;
     pGDIInfo->flRaster     = 0;
     pGDIInfo->flTextCaps   = 0;//GetDeviceCaps(hDC,TEXTCAPS);
     pGDIInfo->xStyleStep   = 1;
     pGDIInfo->yStyleStep   = 1;
     pGDIInfo->denStyleStep  =   pGDIInfo->ulDevicePelsDPI/ 25;
     ReleaseDC(NULL,memdc);

}
/*
 *   Get the PRINTER_INFO_2 in pPrinterInfo  and return the hPrinter
 */
HANDLE GetPrinterInfo(LPBYTE *pPrinterInfo , WCHAR PrinterName[]) {
     HANDLE hPrinter;
     DWORD cbNeeded;
     //PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ALL_ACCESS };
     PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ACCESS_ADMINISTER };
     OpenPrinter(PrinterName,&hPrinter,NULL);
     GetPrinter(hPrinter,2,NULL ,0,&cbNeeded);
     *pPrinterInfo = (LPBYTE)malloc(cbNeeded);
     if(GetPrinter(hPrinter,2,*pPrinterInfo,cbNeeded,&cbNeeded) == FALSE)
          OutputDebugString(L"failure");
     return hPrinter;
}


DWORD GetRealDriverName(WCHAR PrinterName[], WCHAR RealDriverName[])
{

     HANDLE hPrinter;
     PRINTER_DEFAULTS defaults = { NULL, NULL, READ_CONTROL };
     OpenPrinter(PrinterName,&hPrinter,&defaults);
     DWORD cbNeeded;   
     DWORD dwret = GetPrinterData(hPrinter,L"RealDriverName",NULL,
             (LPBYTE)RealDriverName,256,&cbNeeded);
     ClosePrinter(hPrinter);
     return dwret;
}

DWORD GetRealDriverName(HANDLE hPrinter, WCHAR RealDriverName[])
{
     DWORD cbNeeded;
     return GetPrinterData(
             hPrinter,    // handle to printer or print server
             L"RealDriverName",  // value name
             NULL,      // data type
             (LPBYTE)RealDriverName,       // configuration data buffer
             256,        // size of configuration data buffer
             &cbNeeded   // bytes received or required 
             );
}

DWORD SetRealDriverName(WCHAR PrinterName[], WCHAR RealDriverName[])
{
     HANDLE hPrinter;
     PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ALL_ACCESS };

     OpenPrinter(
             PrinterName,         // printer or server name
             &hPrinter,          // printer or server handle
             &defaults   // printer defaults
             );
     DWORD dwret = SetPrinterData(
             hPrinter,    // handle to printer or print server
             L"RealDriverName",  // data to set
             REG_SZ,         // data type
             (LPBYTE)RealDriverName,       // configuration data buffer
             (wcslen(RealDriverName) + 1) * sizeof(WCHAR)// size of buffer
             );
     ClosePrinter(hPrinter);
     return dwret;

}


TCHAR *GetTempFile(TCHAR *TempPath , TCHAR *Prefix,TCHAR *TempFileName)
{
     BOOL isTempPathNull = FALSE;
     if(TempPath == NULL)
     {
          TempPath = (TCHAR *)MALLOC(sizeof(TCHAR) * MAX_PATH);
          GetTempPath(
                  MAX_PATH,  // size of buffer
                  TempPath        // path buffer
                  );

          isTempPathNull = TRUE;
     }
     GetTempFileName(
             TempPath,      // directory name
             Prefix,  // file name prefix
             0,            // integer
             TempFileName    // file name buffer
             );

     if(isTempPathNull == TRUE)
          free(TempPath);
     return TempFileName;
}
/*
 *  Get Back a driver to start with during initialization
 */
void InstalledPrinter(WCHAR *pInitialDriverName ,LPTSTR pPrinterName )
{
     WCHAR ach[256];
     PTSTR pPrinter;

     memset(ach , 0 ,sizeof(WCHAR) * 256);
     GetProfileString (L"windows", L"device", NULL, ach, 256);
     pPrinter = (PTSTR)&ach[0];
     WCHAR *ptr = wcschr(pPrinter , ',');
     if(ptr)
          *ptr = 0;
     wcscpy(pInitialDriverName , pPrinter);
}

BOOL IsInchDimensions()

{

     HKEY    hKey;
     LONG    CountryCode = CTRY_UNITED_STATES;
     WCHAR   wszStr[16];
     const WCHAR  wszCountryKey[]   = L"Control Panel\\International";
     const WCHAR  wszCountryValue[] = L"iCountry";


     if (RegOpenKey(HKEY_CURRENT_USER, wszCountryKey, &hKey) == ERROR_SUCCESS) {

          DWORD   Type   = REG_SZ;
          DWORD   RetVal = sizeof(wszStr);

          if (RegQueryValueEx(hKey,
                      (LPTSTR)wszCountryValue,
                      NULL,
                      &Type,
                      (LPBYTE)wszStr,
                      &RetVal) == ERROR_SUCCESS) {

               LPWSTR  pwStop;

               CountryCode = wcstoul(wszStr, &pwStop, 10);

          } 
          RegCloseKey(hKey);

     } 
     RegCloseKey(HKEY_CURRENT_USER);

     if ((CountryCode == CTRY_UNITED_STATES)             ||
             (CountryCode == CTRY_CANADA)                    ||
             ((CountryCode >= 50) && (CountryCode < 60))     ||
             ((CountryCode >= 500) && (CountryCode < 600))) 
          return(FALSE);
     else 
          return(TRUE);
}


