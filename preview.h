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

#include <windows.h>
class PMPreview{
     int ScrollPosX;
     int ScrollPosY;
     int maxScrollX;
     int maxScrollY;
     WCHAR *MetaFileName;
     void GetMetaDimensions(HDC ,HENHMETAFILE hemf  , int &ResX , int &ResY);
     void ResizePreviewWindow(HWND hDlg,HWND hwnd , HENHMETAFILE hemf);
     void DrawPreview(HWND hDlg , LPDRAWITEMSTRUCT pdis , HENHMETAFILE hemf);
    public:
     int GetScrollPosX(){ return ScrollPosX;}
     int GetScrollPosY(){ return ScrollPosY;}
     void SetScrollPosX(int x){ ScrollPosX = x;}
     void SetScrollPosY(int y){ ScrollPosY = y;}
     int GetmaxScrollX(){ return maxScrollX;}
     int GetmaxScrollY(){ return maxScrollY;}
     void DrawOnOwnerDrawButton(HWND hDlg);
     PMPreview(WCHAR *FileName) { ScrollPosX = 0;ScrollPosY = 0; maxScrollX = 0; maxScrollY = 0;
          MetaFileName = FileName; };
          ~PMPreview() { DeleteFile(MetaFileName);}
          static BOOL APIENTRY PreviewMetafile( HWND hDlg, UINT message, UINT wParam, LONG lParam);
};

