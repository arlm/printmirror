
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


#include "prntmrui.h"
//-----------------------------------------------------------------------------
/*
 *  Note: Make sure any DrvDeviceCapabilities call from within the driver 
 *        send an incoming devmode
 */
DWORD  PMUIDriver::PMDrvDeviceCapabilities(HANDLE  hPrinter,PWSTR  pDeviceName, WORD  iDevCap,
        VOID  *pvOutput,DEVMODE  *pDevMode)

{
     WCHAR RealDriverName[256];
     LPBYTE pBuffer;   
     /* Real driver name has  3 tries
      *  First devmode,second if no incoming devmode,then the PrinterName
      *  Third from the default Devmode.
      */
     if(pDevMode)
          wcscpy(RealDriverName , (WCHAR *)((LPBYTE)pDevMode + sizeof(DEVMODEW)));
     else if(wcscmp(RealPrinterName , L""))
          wcscpy(RealDriverName , RealPrinterName);
     else
     {
          LPBYTE pBuffer = NULL;
          ClosePrinter(GetPrinterInfo(&pBuffer ,pDeviceName));
          PDEVMODE pdm = NULL;
          if(pBuffer)
               pdm =((PRINTER_INFO_2 *)pBuffer)->pDevMode;
          if(pdm && pdm->dmDriverExtra == sizeof(VPDEVMODE))
          {
               wcscpy(RealDriverName ,(WCHAR *)((LPBYTE)pdm + sizeof(DEVMODEW)));
          }
          else
               GetRealDriverName(hPrinter , RealDriverName);
          free(pBuffer);
     }
     HANDLE hRPrinter = GetPrinterInfo(&pBuffer , RealDriverName);

     /* If there is an incoming devmode, 
        1) Get the real driver's devmode size
        2) Get the real driver's devmode
        3) copy our public part to real driver's devmode
      */
     PDEVMODEW  pdmInput = NULL;
     LPBYTE pBuffer1;
     ClosePrinter(GetPrinterInfo(&pBuffer1 , pDeviceName));
     LONG sz = DocumentProperties(0,hRPrinter , RealDriverName,0,0,0);
     pdmInput = (PDEVMODEW)malloc(sz);
     DocumentProperties(0,hRPrinter , RealDriverName,pdmInput,0,DM_OUT_BUFFER);
     int dmDriverExtra = pdmInput->dmDriverExtra;
     if(pDevMode)
     {
          memcpy(pdmInput,pDevMode,sizeof(DEVMODEW));
          pdmInput->dmDriverExtra = dmDriverExtra;
     }
     else
     {
          memcpy(pdmInput,((PRINTER_INFO_2 *)pBuffer1)->pDevMode,sizeof(DEVMODEW));
          pdmInput->dmDriverExtra = dmDriverExtra;
          free(pBuffer1);
     }

     ClosePrinter(hRPrinter);
     /* The real driver should fix up the devmode and then do the rest */
     DWORD ret =  DeviceCapabilities( RealDriverName, ((PRINTER_INFO_2 *)pBuffer)->pPortName,
             iDevCap, (LPTSTR)pvOutput, pdmInput);
     free(pBuffer);
     return ret;

}

