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


#define PRNTMRUI  1
#include "prntmrui.h"
PMUIDriver *pmui = NULL;
HMODULE hModDLL;
//-----------------------------------------------------------------------------
    BOOL __stdcall 
DllMain (HMODULE hmod, DWORD dwReason, LPVOID lpRes)
{
     switch (dwReason)
     {
     case DLL_PROCESS_ATTACH:
         {
              hModDLL = hmod;
              /* keep the dll loaded */
              WCHAR wName[MAX_PATH];
              if (GetModuleFileName(hmod, wName, MAX_PATH)) 
                   LoadLibrary(wName);
              BOOL bIsExplorer =  IsExplorer();
              pmui = new PMUIDriver(bIsExplorer);              
              return TRUE;
         }
     case DLL_THREAD_ATTACH:
         break;
     case DLL_THREAD_DETACH:
         break;
     }
     return TRUE;
}
DWORD DrvDeviceCapabilities(HANDLE  hPrinter,PWSTR  pDeviceName, WORD  iDevCap,
        VOID  *pvOutput,DEVMODE  *pDevMode)
{

    return pmui->PMDrvDeviceCapabilities(hPrinter,pDeviceName,iDevCap,pvOutput,pDevMode);
  
}
INT DrvDocumentEvent( HANDLE  hPrinter, HDC  hdc, int  iEsc, ULONG  cbIn, PULONG  pbIn,
        ULONG  cbOut, PULONG  pbOut)
{
     return pmui->DrvDocumentEvent(hPrinter,hdc,iEsc,cbIn,pbIn,cbOut,pbOut);

}
LONG  DrvDocumentPropertySheets(PPROPSHEETUI_INFO  pPSUIInfo, LPARAM  lParam)
{

    return pmui->DrvDocumentPropertySheets(pPSUIInfo,lParam);
}
BOOL  DrvPrinterEvent(LPWSTR  pPrinterName, INT  DriverEvent, DWORD  Flags, LPARAM  lParam)
{
    return pmui->DrvPrinterEvent(pPrinterName,DriverEvent,Flags,lParam);

}
LONG  DrvDevicePropertySheets(PPROPSHEETUI_INFO  pPSUIInfo, LPARAM  lParam)
{
    return pmui->DrvDevicePropertySheets(pPSUIInfo,lParam);
}
BOOL  DevQueryPrintEx(PDEVQUERYPRINT_INFO  pDQPInfo)
{

    return pmui->DevQueryPrintEx(pDQPInfo);

}
BOOL  DrvConvertDevMode(LPTSTR  pPrinterName, PDEVMODE  pdmIn, PDEVMODE  pdmOut, PLONG  pcbNeeded,DWORD  fMode)
{
   return pmui->DrvConvertDevMode(pPrinterName, pdmIn, pdmOut,pcbNeeded,fMode);
}
