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
#include <winspool.h>

PDEVMODE GetRealPrinterDevmode(HANDLE hRPrinter, WCHAR *RealDriverName, VDEVMODE *pCurDevmode)
{
     LONG sz = DocumentProperties(0,hRPrinter , RealDriverName,0,0,0);
     PDEVMODE pdmOutput = (PDEVMODE)malloc(sz);
     DocumentProperties(0,hRPrinter , RealDriverName,pdmOutput,0,DM_OUT_BUFFER);
     int dmDriverExtra =  pdmOutput->dmDriverExtra;
     memcpy(pdmOutput , pCurDevmode , sizeof(DEVMODEW));
     pdmOutput->dmDriverExtra = dmDriverExtra;
     DocumentProperties(0,hRPrinter , RealDriverName,pdmOutput,pdmOutput,
             DM_IN_BUFFER | DM_OUT_BUFFER);
     return pdmOutput;
}
void PrintToPaper(PPDEV pPDev)
{

     DOCINFO di;
     LPBYTE pDevmode;
     pDevmode = NULL;
     VDEVMODE *pCurDevmode = pPDev->pCurDevmode;
     WCHAR *RealDriverName = (WCHAR *)(((LPBYTE)pCurDevmode) + sizeof(DEVMODEW));

     HANDLE hRPrinter;   
     OpenPrinter(RealDriverName, &hRPrinter, NULL);
     PDEVMODE pdmOutput = GetRealPrinterDevmode(hRPrinter,RealDriverName,pCurDevmode);
     HDC hRealDC = CreateDC(L"WINSPOOL", RealDriverName, NULL, pdmOutput);
     free(pdmOutput);
     HDC hWorkingDC = hRealDC;
     memset(&di , 0 , sizeof(DOCINFO));
     di.lpszDocName = pPDev->pDocument;
     di.lpszDatatype = L"raw";
     if(StartDoc(hWorkingDC , &di) > 0)
     {

          for(int i = 1 ; i <= pPDev->Pages ; i++)
          {
               pDevmode = NULL;
               TCHAR MetaFileList[MAX_PATH];
               GetTempFile(NULL ,L"Meta" , MetaFileList);
               GetMetaFileFromSpoolFile(pPDev->pSpoolFileName ,i,MetaFileList,
                       pPDev, &pDevmode);
               if(pDevmode != NULL)
               {
                    PDEVMODE pdmOutput = GetRealPrinterDevmode(hRPrinter,RealDriverName,
                            (VDEVMODE *)pDevmode);
                    hWorkingDC = ResetDC(hWorkingDC , (DEVMODE *)pdmOutput);
                    free(pDevmode);
                    free(pdmOutput);
               }
               StartPage(hWorkingDC);
               HENHMETAFILE hemf = GetEnhMetaFile(
                       MetaFileList   // file name
                       );
               RECT rect;
               rect.left = rect.top = 0;
               rect.bottom = GetDeviceCaps(hWorkingDC , VERTRES);
               rect.right = GetDeviceCaps(hWorkingDC , HORZRES);
               PlayEnhMetaFile( hWorkingDC, hemf, &rect );
               EndPage(hWorkingDC);

               DeleteEnhMetaFile(hemf);
               DeleteFile(MetaFileList);
          }

     }
     EndDoc(hWorkingDC);
     ClosePrinter(hRPrinter);
     DeleteDC(hWorkingDC);
}

