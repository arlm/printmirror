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
#include "preview.h"
void PMPreview::DrawPreview(HWND hDlg , LPDRAWITEMSTRUCT pdis , HENHMETAFILE hemf)
{
     HDC     screenHDC = pdis->hDC;
     FillRect(
             screenHDC,           // handle to DC
             &(pdis->rcItem),  // rectangle
             (HBRUSH)GetStockObject(WHITE_BRUSH)
             );
     RECT rect;
     rect.left = rect.top = 0;
     rect.right = pdis->rcItem.right;
     rect.bottom = pdis->rcItem.bottom ;

     HRGN hrgn = CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
     SelectClipRgn(screenHDC , hrgn);

     ENHMETAHEADER emf;
     GetEnhMetaFileHeader(hemf , sizeof(ENHMETAHEADER), &emf);
     RECT Rect;
     Rect.left =Rect.top =0;
     Rect.bottom = emf.szlMillimeters.cy;
     Rect.right = emf.szlMillimeters.cx;

     RECT RectRes;
     RectRes.left = -GetScrollPosX(); RectRes.top = -GetScrollPosY();
     RectRes.bottom= (GetDeviceCaps(screenHDC , LOGPIXELSY) * 
             Rect.bottom * 10 )/(254) + RectRes.top;
     RectRes.right = (GetDeviceCaps(screenHDC, LOGPIXELSX) * 
             Rect.right * 10) /254 + RectRes.left;
     PlayEnhMetaFile(
             screenHDC,            // handle to DC
             hemf,  // handle to an enhanced metafile
             &RectRes // &(pdis->rcItem)  // bounding rectangle
             );

     SelectClipRgn(screenHDC,NULL);
     DeleteObject(hrgn);

}




void PMPreview::ResizePreviewWindow(HWND hDlg,HWND hwnd , HENHMETAFILE hemf)
{
     RECT ScrollRect;
     int HideX = FALSE;
     int HideY = FALSE;
     GetWindowRect(GetDlgItem(hDlg,IDC_SCROLLX) , &ScrollRect);
     RECT rect;
     GetWindowRect(hDlg , &rect);
     RECT Clientrect;
     GetClientRect(hDlg, &Clientrect);
     int Menuheight = (rect.bottom - rect.top) - 
         (Clientrect.bottom - Clientrect.top);
     rect.bottom = rect.bottom - rect.top;
     rect.right = rect.right - rect.left;
     rect.top = rect.left = 0;
     UINT cbNeeded = GetEnhMetaFileHeader( hemf, 0,  NULL);
     LPENHMETAHEADER lpemh = (LPENHMETAHEADER)MALLOC(cbNeeded);
     GetEnhMetaFileHeader( hemf, cbNeeded, lpemh);
     HDC ScreenDC = GetDC(NULL);
     rect.left += GetDeviceCaps(ScreenDC , LOGPIXELSX);
     rect.right -= GetDeviceCaps(ScreenDC , LOGPIXELSX);
     rect.bottom -= Menuheight + ScrollRect.bottom - ScrollRect.top;

     int ResX,ResY;
     ResY = (GetDeviceCaps(ScreenDC , LOGPIXELSY) * 
             lpemh->szlMillimeters.cy * 10 )/(254);
     ResX = (GetDeviceCaps(ScreenDC, LOGPIXELSX) * 
             lpemh->szlMillimeters.cx* 10) /254;
     RECT PreviewRect;
     if(ResX >  rect.right - rect.left)
     {
          PreviewRect.left = rect.left;
          PreviewRect.right = rect.right;
     }
     else
     {
          PreviewRect.left = rect.left + (rect.right - rect.left - ResX)/2;
          PreviewRect.right = rect.right - (rect.right - rect.left - ResX)/2;
          HideX = TRUE;
     }
     if(ResY >  rect.bottom - rect.top)
     {
          PreviewRect.bottom = rect.bottom;
          PreviewRect.top = rect.top;

     }
     else
     {
          PreviewRect.top = rect.top + (rect.bottom - rect.top - ResY)/2;
          PreviewRect.bottom = rect.bottom - (rect.bottom - rect.top - ResY)/2;
          HideY = TRUE;
     }
     MoveWindow(hwnd , PreviewRect.left ,PreviewRect.top, 
             PreviewRect.right - PreviewRect.left, PreviewRect.bottom - PreviewRect.top 
             ,TRUE);
     ShowWindow(GetDlgItem(hDlg,IDC_SCROLLY), SW_SHOW);
     ShowWindow(GetDlgItem(hDlg,IDC_SCROLLX), SW_SHOW);

     UpdateWindow(GetDlgItem(hDlg,IDC_SCROLLY));
     UpdateWindow(GetDlgItem(hDlg,IDC_SCROLLX));

     GetWindowRect(GetDlgItem(hDlg,IDC_SCROLLY) , &ScrollRect);
     GetWindowRect(GetDlgItem(hDlg,IDC_SCROLLY) , &ScrollRect);
     MoveWindow(GetDlgItem(hDlg,IDC_SCROLLY)  , PreviewRect.right ,PreviewRect.top, 
             ScrollRect.right - ScrollRect.left , PreviewRect.bottom - PreviewRect.top ,TRUE);
     GetWindowRect(GetDlgItem(hDlg,IDC_SCROLLX) , &ScrollRect);
     MoveWindow(GetDlgItem(hDlg,IDC_SCROLLX), PreviewRect.left,PreviewRect.bottom,
             PreviewRect.right - PreviewRect.left ,
             ScrollRect.bottom - ScrollRect.top ,TRUE);
     maxScrollX =  ResX - (PreviewRect.right - PreviewRect.left);
     maxScrollY =  ResY - (PreviewRect.bottom - PreviewRect.top);
     SetScrollRange(GetDlgItem(hDlg,IDC_SCROLLX) , SB_CTL, 0, maxScrollX,TRUE);
     SetScrollRange(GetDlgItem(hDlg,IDC_SCROLLY) , SB_CTL, 0, maxScrollY,TRUE);
     SetScrollPos(GetDlgItem(hDlg,IDC_SCROLLX), SB_CTL, ScrollPosX, TRUE);
     SetScrollPos(GetDlgItem(hDlg,IDC_SCROLLY), SB_CTL, ScrollPosY , TRUE);
     if(HideY)
          ShowWindow(GetDlgItem(hDlg,IDC_SCROLLY), SW_HIDE);
     else
          ShowWindow(GetDlgItem(hDlg,IDC_SCROLLY), SW_SHOW);
     if(HideX)
          ShowWindow(GetDlgItem(hDlg,IDC_SCROLLX), SW_HIDE);
     else
          ShowWindow(GetDlgItem(hDlg,IDC_SCROLLX), SW_SHOW);
     ReleaseDC(NULL,ScreenDC);

}

void PMPreview::DrawOnOwnerDrawButton(HWND hDlg)
{
     DRAWITEMSTRUCT dis;
     HWND hwnd = GetDlgItem(hDlg , IDC_PREVIEW);

     HENHMETAFILE hemf = GetEnhMetaFile( MetaFileName);
     ResizePreviewWindow(hDlg, hwnd,hemf); 
     GetClientRect(hwnd , &(dis.rcItem));
     dis.hDC = GetDC(hwnd);
     DrawPreview(hDlg,&dis,hemf);
     DeleteEnhMetaFile(hemf);
     ReleaseDC(hwnd , dis.hDC);
}
BOOL APIENTRY PMPreview :: PreviewMetafile(
        HWND hDlg,
        UINT message,
        UINT wParam,
        LONG lParam)
{
     static PMPreview *pmp = NULL;
     switch (message)
     {

     case WM_DRAWITEM:
         {
              LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;

              if ((pdis->itemAction == ODA_DRAWENTIRE) && 
                      (wParam  == IDC_PREVIEW))
                   pmp->DrawOnOwnerDrawButton(hDlg);
         }
         break;
     case WM_INITDIALOG:
         {
              pmp = (PMPreview *)lParam;

              SetForegroundWindow(hDlg);
              HDC hDC = GetDC(NULL);

              RECT ScreenRect;
              SystemParametersInfo( SPI_GETWORKAREA, 0,((PVOID)&ScreenRect),0);
              int ScreenWidth = ScreenRect.right - ScreenRect.left;
              int ScreenHeight = ScreenRect.bottom - ScreenRect.top;
              MoveWindow(hDlg , ScreenRect.left,ScreenRect.top, ScreenWidth, ScreenHeight,FALSE);

              HWND hwnd = GetDlgItem(hDlg , IDC_PREVIEW);
              RECT PreviewRect;
              GetWindowRect(hwnd , &PreviewRect);
              MoveWindow(hwnd , ScreenWidth /4 ,0,ScreenWidth/2,ScreenHeight,FALSE); 
              int PreviewRight = (ScreenWidth * 3) /4;
              ReleaseDC(NULL,hDC);
              return TRUE;
         }
     case WM_VSCROLL:
         {
              switch(LOWORD(wParam))
              {
              case SB_PAGEUP:
                  pmp->SetScrollPosY(max(0,pmp->GetScrollPosY() - 10));
                  break;
              case SB_LINEUP:
                  pmp->SetScrollPosY(max(0,pmp->GetScrollPosY() - 1));
                  break;
              case SB_PAGEDOWN:
                 // pmp->SetScrollPosY(max(0,pmp->GetScrollPosY()  + 10));
                 pmp->SetScrollPosY( min(pmp->GetmaxScrollY(),pmp->GetScrollPosY() + 10));
                  break;
              case SB_LINEDOWN:
//                  pmp->SetScrollPosY(max(0,pmp->GetScrollPosY()  + 1));
                 pmp->SetScrollPosY( min(pmp->GetmaxScrollY(),pmp->GetScrollPosY() + 1));
                  break;
              case SB_THUMBTRACK:
              case SB_THUMBPOSITION:
                  pmp->SetScrollPosY(HIWORD(wParam));
                  break;
              }
              UpdateWindow(GetDlgItem(hDlg,IDC_SCROLLY));
              UpdateWindow(GetDlgItem(hDlg,IDC_SCROLLX));
              SetScrollPos(GetDlgItem(hDlg,IDC_SCROLLY), SB_CTL, pmp->GetScrollPosY(), TRUE);
              pmp->DrawOnOwnerDrawButton(hDlg);
         }
         break;
     case WM_HSCROLL:
         {
              switch(LOWORD(wParam))
              {
              case SB_PAGELEFT:
                  pmp->SetScrollPosX(max(0,pmp->GetScrollPosX() - 10));
                  break;
              case SB_LINELEFT:
                  pmp->SetScrollPosX(max(0,pmp->GetScrollPosX() - 1));
                  break;
              case SB_PAGERIGHT:
                  //pmp->SetScrollPosX(max(0,pmp->GetScrollPosX() + 10));
                  pmp->SetScrollPosX(min(pmp->GetmaxScrollX(),pmp->GetScrollPosX() + 10));
                  break;
              case SB_LINERIGHT:
//                  pmp->SetScrollPosX(max(0,pmp->GetScrollPosX() + 1));
                  pmp->SetScrollPosX(min(pmp->GetmaxScrollX(),pmp->GetScrollPosX() + 1));
                  break;
              case SB_THUMBTRACK:
              case SB_THUMBPOSITION:
                  pmp->SetScrollPosX(HIWORD(wParam));
                  break;
              }
              UpdateWindow(GetDlgItem(hDlg,IDC_SCROLLY));
              UpdateWindow(GetDlgItem(hDlg,IDC_SCROLLX));
              SetScrollPos(GetDlgItem(hDlg,IDC_SCROLLX), SB_CTL, pmp->GetScrollPosX(), TRUE);
              pmp->DrawOnOwnerDrawButton(hDlg);
         }
         break;
     case WM_CLOSE:
         EndDialog(hDlg , 0);
         break;
     default:
         return FALSE;
     }
     return TRUE;
}
