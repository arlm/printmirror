

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



//-----------------------------------------------------------------------------
#include "prntmrui.h"
BOOL  
PMUIDriver::DrvConvertDevMode(LPTSTR  pPrinterName, PDEVMODE  pdmIn, PDEVMODE  pdmOut, 
        PLONG  pcbNeeded,DWORD  fMode)
{
     HANDLE hPrinter;
     {
          wcscpy(PrinterName ,pPrinterName);
          LONG sz = sizeof(VDEVMODE);
          if(!pdmOut || sz > *pcbNeeded)
          {
               SetLastError(ERROR_INSUFFICIENT_BUFFER);
               return FALSE;
          }
          OpenPrinter(pPrinterName, &hPrinter, NULL);
          if(hPrinter)
          {
               DrvDocumentProperties(NULL, hPrinter, pPrinterName, pdmOut, NULL, DM_OUT_BUFFER);
               ClosePrinter(hPrinter);
          }
     }

     return TRUE; 
}
