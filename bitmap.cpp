
/*
   PrintMirror extracts individual page metafiles from Spool File.
   Copyright (C) 2002-2003  V Aravind

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
void SaveAsBitmap(HWND hDlg , OPENFILENAME ofn , PPDEV pPDev)
{
     int PageNbr = GetWindowLongPtr(hDlg , GWLP_USERDATA);
     WCHAR MetaFileList[1024];
     DEVMODE *pDevmode = NULL; 
     GetTempFile(NULL ,L"Meta" , MetaFileList);
     GetMetaFileFromSpoolFile(pPDev->pSpoolFileName ,PageNbr + 1,MetaFileList,
             pPDev, (BYTE **)&pDevmode);
     HENHMETAFILE hemf = GetEnhMetaFile(
             MetaFileList   // file name
             );
     ENHMETAHEADER emf;
     GetEnhMetaFileHeader(hemf , sizeof(ENHMETAHEADER), &emf);
     RECT Rect;
     Rect.left =Rect.top =0;
     Rect.bottom = emf.szlMillimeters.cy;
     Rect.right = emf.szlMillimeters.cx;

     HDC compDC = CreateCompatibleDC(NULL);
     RECT RectRes;
     RectRes.left =RectRes.top =0;
     RectRes.bottom= (GetDeviceCaps(compDC , LOGPIXELSY) * 
             Rect.bottom * 10 )/(254);
     RectRes.right = (GetDeviceCaps(compDC, LOGPIXELSX) * 
             Rect.right * 10) /254;

     BITMAPINFO bmi;
     BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *)&bmi;
     LPBYTE pBits;

     bmih->biSize          = sizeof (BITMAPINFOHEADER) ;
     bmih->biWidth         =  RectRes.right;
     bmih->biHeight        =  RectRes.bottom;
     bmih->biPlanes        = 1 ;
     bmih->biBitCount      = 24 ;
     bmih->biCompression   = BI_RGB ;
     bmih->biSizeImage     = (((RectRes.right * 24 + 31) & ~31)/8)
         * RectRes.bottom ;
     bmih->biXPelsPerMeter = 0 ;
     bmih->biYPelsPerMeter = 0 ;
     bmih->biClrUsed       = 0 ;
     bmih->biClrImportant  = 0 ;
     HBITMAP hbm = CreateDIBSection (compDC, (BITMAPINFO *)  &bmi, 0, (void **)&pBits, NULL, 0) ;

     SelectObject(compDC , hbm);
     PatBlt(compDC , 0,0,bmih->biWidth , bmih->biHeight , WHITENESS);
     PlayEnhMetaFile(compDC , hemf , &RectRes);
     DeleteEnhMetaFile(hemf);

     TCHAR TempBMPFileName[MAX_PATH];
     GetTempFile(NULL ,L"T" , TempBMPFileName);
     BITMAPFILEHEADER hdr;       
     HANDLE hf = CreateFile(TempBMPFileName, 
             GENERIC_READ | GENERIC_WRITE, 
             (DWORD) 0, 
             NULL, 
             CREATE_ALWAYS, 
             FILE_ATTRIBUTE_NORMAL, 
             (HANDLE) NULL); 

     hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
     // Compute the size of the entire file. 
     hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
             bmih->biSize + bmih->biClrUsed 
             * sizeof(RGBQUAD) + bmih->biSizeImage); 
     hdr.bfReserved1 = 0; 
     hdr.bfReserved2 = 0; 

     // Compute the offset to the array of color indices. 
     hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
         bmih->biSize + bmih->biClrUsed 
         * sizeof (RGBQUAD); 

     DWORD dwTmp;
     // Copy the BITMAPFILEHEADER into the .BMP file. 
     WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL);

     WriteFile(hf, (LPVOID) bmih, sizeof(BITMAPINFOHEADER) + bmih->biClrUsed * sizeof (RGBQUAD), 
             (LPDWORD) &dwTmp, ( NULL)); 
     DWORD cb = bmih->biSizeImage;
     WriteFile(hf, (LPSTR)pBits, (int) cb, (LPDWORD) &dwTmp,NULL); 
     CloseHandle(hf);

     CopyFile(TempBMPFileName , ofn.lpstrFile, FALSE);
     DeleteFile(TempBMPFileName);
     DeleteFile(MetaFileList);
     DeleteDC(compDC);
     DeleteObject(hbm);
}
