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
#include "preview.h"
int GetColorOrganisation(HDC hRealDC , DEVMODE *pbIn , ULONG palette[])
{

     int ret = 0;
     HDC compDC = CreateCompatibleDC(hRealDC);
     HBITMAP hBitmap = CreateCompatibleBitmap(hRealDC , 1 , 1);
     SelectObject(compDC , hBitmap);
     BITMAP   bmp;
     GetObject(hBitmap ,sizeof(BITMAP), &bmp);
     if(bmp.bmBitsPixel <= 8)
     {
          switch(bmp.bmBitsPixel)
          {
          case 8:
              {
                   BYTE bits = 0;
                   for(int i = 0 ; i < 256 ; i++)
                   {
                        bits = i;
                        SetBitmapBits(hBitmap , 1  , &bits);
                        palette[i] =  GetPixel(compDC , 0 , 0 );
                   }
                   ret = 256;
              }
              break;
          case 1:
              {
                   BYTE bits = 0;
                   for(int i = 0 ; i < 2 ; i++)
                   {
                        SetBitmapBits(hBitmap , 1  , &bits);
                        bits = ~bits;
                        palette[i] =  GetPixel(compDC , 0 , 0 );
                   }
                   ret = 2;
              }
              break;
          case 4:
              {
                   BYTE bits = 0;
                   for(int i = 0 ; i < 16 ; i++)
                   {
                        bits = (i << 4);
                        SetBitmapBits(hBitmap , 1  , &bits);
                        palette[i] =  GetPixel(compDC , 0 , 0 );
                   }
                   ret = 16;
              }
              break;
          }
     }
     DeleteDC(compDC);
     DeleteObject(hBitmap);
     return ret;
}
void CreateGDIInfo(HANDLE hPrinter,VDEVMODE *pbIn)
{
     WCHAR RealDriverName[256];
     wcscpy(RealDriverName ,pbIn->pdm.PrinterName);
     /* Get the real driver's devmode and hack with our's */
     HANDLE hRPrinter;   
     OpenPrinter(RealDriverName, &hRPrinter, NULL);
     LONG sz = DocumentProperties(0,hRPrinter , RealDriverName,0,0,0);

     PDEVMODEW  pdmInput1 = NULL;
     PDEVMODEW  pdmOutput1 = NULL;
     pdmInput1 = (PDEVMODEW)malloc(sz);
     pdmOutput1 = (PDEVMODEW)malloc(sz);
     DocumentProperties(0,hRPrinter , RealDriverName,pdmInput1,0,DM_OUT_DEFAULT);
     int dmDriverExtra = pdmInput1->dmDriverExtra;
     memcpy(pdmInput1,pbIn,sizeof(DEVMODEW));
     pdmInput1->dmDriverExtra = dmDriverExtra;
     DocumentProperties(0,hRPrinter , RealDriverName,pdmOutput1,pdmInput1,DM_OUT_BUFFER |
             DM_IN_BUFFER);

     ClosePrinter(hRPrinter);
     HDC hRealDC =  CreateDC(L"WINSPOOL",RealDriverName,NULL , pdmOutput1);
     free(pdmInput1);
     free(pdmOutput1);

     FillDeviceCaps(hRealDC,&(pbIn->pdm.gi),pbIn);
     ULONG palette[256];
     int numcolors = GetColorOrganisation(hRealDC , (DEVMODE *)pbIn, palette );
     pbIn->pdm.numcolors = numcolors;
     memcpy(pbIn->pdm.Palette , palette , sizeof(ULONG) * 256);
     LOGFONT lf;
     HGDIOBJ hFont = GetCurrentObject(hRealDC,OBJ_FONT);
     GetObject(hFont , sizeof(LOGFONT), &lf);
     memcpy(&(pbIn->pdm.lf) , &lf , sizeof(LOGFONT));

     DeleteDC(hRealDC);
}



void CreateWin2kcompatibleSplFile(HANDLE hPrinter)
{

     LPBYTE pPInfo;
     DWORD cbNeeded;
     GetPrinter( hPrinter, 2, NULL , 0, &cbNeeded);
     pPInfo = (LPBYTE)malloc(cbNeeded);
     GetPrinter( hPrinter, 2, pPInfo, cbNeeded, &cbNeeded );
     if(!(((PRINTER_INFO_2 *)pPInfo)->Attributes & PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS)
             ||
             (((PRINTER_INFO_2 *)pPInfo)->Attributes & PRINTER_ATTRIBUTE_DIRECT))
     {
          ((PRINTER_INFO_2 *)pPInfo)->Attributes |=PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS;
          ((PRINTER_INFO_2 *)pPInfo)->Attributes &= ~PRINTER_ATTRIBUTE_DIRECT;
          PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ACCESS_ADMINISTER};
          HANDLE hDriver;
          OpenPrinter( ((PRINTER_INFO_2 *)pPInfo)->pPrinterName, &hDriver, &defaults);
          if(!SetPrinter( hDriver, 2, pPInfo , 0 ))
          {
          }
          ClosePrinter(hDriver);
     }
     free(pPInfo);
}


unsigned long __stdcall ThreadFunc( LPVOID lpParam ) 
{ 
     HANDLE hPrinter = *((HANDLE *)lpParam);
     DWORD dwJobId = *(DWORD *)((LPBYTE)lpParam + sizeof(HANDLE));
     Sleep(5000);
     SetJob(hPrinter , dwJobId,0,0,JOB_CONTROL_DELETE);
     ClosePrinter(hPrinter); 
     free(lpParam);
     return 0; 
} 

BOOL PopFileSaveDlg(HWND hwnd , LPTSTR pstrFileName , LPTSTR pstrTitleName , OPENFILENAME &ofn)
{
     static TCHAR szFilter[] = TEXT ("EMF File (*.emf)\0*.emf\0") \
         TEXT ("24-Bit Bitmap (*.bmp)\0*.bmp\0\0") ;

     WCHAR szPath[1024];
     SHGetSpecialFolderPath(hwnd,szPath,CSIDL_PERSONAL,0);

     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = szPath;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = L"emf" ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL; 
     ofn.hwndOwner         = GetDesktopWindow(); //hwnd ; commented in0.85v
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_OVERWRITEPROMPT ;

     return GetSaveFileName (&ofn) ;

}
BOOL APIENTRY LicenseDialog(
        HWND hDlg,
        UINT message,
        UINT wParam,
        LONG lParam)
{
     switch(message)
     {
     case WM_COMMAND: 
         switch (LOWORD(wParam)) 
         { 
         case  IDC_CLOSE:
             {
                  EndDialog(hDlg , 0);
                  return TRUE;
             }
         }
     }
     return FALSE;
}

BOOL APIENTRY PMDialog(
        HWND hDlg,
        UINT message,
        UINT wParam,
        LONG lParam)
{
     static PPDEV pPDev = NULL;
     switch (message)
     {
     case WM_INITDIALOG:
         {
              pPDev = (PPDEV)lParam;
              WCHAR PageStatus[100];
              wsprintf(PageStatus , L"Click [Extract] to Extract Page 1 of %d", pPDev->Pages );
              SetDlgItemText(hDlg,IDC_SAVEAS,PageStatus);
              HWND hwnd;
              if(pPDev->Pages == 1)
              {
                   hwnd = GetDlgItem(hDlg , IDNEXT);
                   EnableWindow(hwnd , FALSE);
              }
              hwnd = GetDlgItem(hDlg , IDPREV);
              EnableWindow(hwnd , FALSE);
              RECT ScreenRect;
              SystemParametersInfo( SPI_GETWORKAREA, 0,((PVOID)&ScreenRect),0);
              RECT rect;
              GetWindowRect(hDlg, &rect);
              MoveWindow(hDlg,(ScreenRect.right - ScreenRect.left)/2 - (rect.right -rect.left)/2,
                      (ScreenRect.bottom - ScreenRect.top)/2 - (rect.bottom - rect.top)/2 ,
                      rect.right - rect.left ,
                      rect.bottom - rect.top,FALSE);
              SetForegroundWindow(hDlg);
              break;
         }
         break;
     case WM_COMMAND: 
         switch (LOWORD(wParam)) 
         { 
         case IDLICENSE:
             DialogBoxParam(
                     (HINSTANCE)hModDLL,  // handle to module
                     MAKEINTRESOURCE(IDD_LICENSEDIALOG),
                     hDlg,      // handle to owner window
                     LicenseDialog,  // dialog box procedure
                     (LPARAM)0 );
             return TRUE;
         case IDPREVIEW:
             {
                  DEVMODE *pDevmode = NULL; 
                  int PageNbr = GetWindowLongPtr(hDlg , GWLP_USERDATA);
                  WCHAR TempFileName[1024];
                  GetTempFile(NULL , L"PM",TempFileName);
                  PMPreview pmpreview(TempFileName);
                  OutputDebugString(TempFileName);
                  GetMetaFileFromSpoolFile(pPDev->pSpoolFileName ,PageNbr + 1,TempFileName,
                          pPDev, (BYTE **)&pDevmode);
                  if(pDevmode)
                       free(pDevmode);
                  DialogBoxParam(
                          (HINSTANCE)hModDLL,  // handle to module
                          MAKEINTRESOURCE(IDD_PREVIEWER),
                          hDlg,      // handle to owner window
                          PMPreview::PreviewMetafile,  // dialog box procedure
                          (LPARAM)&pmpreview );
             }
             return TRUE;
         case  IDSAVE:
             {
                  WCHAR szFileName[MAX_PATH] = L"untitled";
                  WCHAR szTitleName[MAX_PATH] = L""; 
                  OPENFILENAME ofn;

                  WCHAR DirName[MAX_PATH];
                  GetCurrentDirectory( MAX_PATH, DirName);
                  PopFileSaveDlg (hDlg, szFileName, szTitleName,ofn);
                  WCHAR *ptr =ofn.lpstrFile + ofn.nFileExtension; 
                  if(!wcscmp(ptr,L"emf"))
                  {
                       DEVMODE *pDevmode = NULL; 
                       int PageNbr = GetWindowLongPtr(hDlg , GWLP_USERDATA);
                       GetMetaFileFromSpoolFile(pPDev->pSpoolFileName ,PageNbr + 1,ofn.lpstrFile,
                               pPDev, (BYTE **)&pDevmode);
                       if(pDevmode)
                            free(pDevmode);
                  }
                  else 
                       SaveAsBitmap(hDlg,ofn,pPDev);
             }
             break;
         case IDPRINT:
             {
                  PrintToPaper(pPDev);
             }
             break;
         case IDNEXT:
             {
                  int PageNbr = GetWindowLongPtr(hDlg , GWLP_USERDATA);
                  PageNbr++;
                  if(PageNbr == (pPDev->Pages - 1) )
                  {
                       HWND hwnd = GetDlgItem(hDlg , IDNEXT);
                       EnableWindow(hwnd , FALSE);
                  }
                  else if(PageNbr >= pPDev->Pages)
                       break;
                  SetWindowLongPtr(hDlg ,GWLP_USERDATA , PageNbr);
                  if(PageNbr > 0 &&  PageNbr < pPDev->Pages )
                  {
                       HWND hwnd = GetDlgItem(hDlg , IDPREV);
                       EnableWindow(hwnd , TRUE);
                  }
                  WCHAR PageStatus[100];
                  wsprintf(PageStatus , L"Click [Extract] to Extract Page %d of %d",PageNbr + 1, pPDev->Pages );
                  SetDlgItemText(hDlg,IDC_SAVEAS,PageStatus);
             }
             break;

         case IDPREV:
             {
                  int PageNbr = GetWindowLongPtr(hDlg , GWLP_USERDATA);

                  PageNbr--;
                  if(PageNbr == 0 )
                  {
                       HWND hwnd = GetDlgItem(hDlg , IDPREV);
                       EnableWindow(hwnd , FALSE);
                  }
                  else if(PageNbr < 0)
                       break;

                  SetWindowLongPtr(hDlg ,GWLP_USERDATA , PageNbr);

                  if(PageNbr < (pPDev->Pages - 1) && PageNbr >= 0)
                  {
                       HWND hwnd = GetDlgItem(hDlg , IDNEXT);
                       EnableWindow(hwnd , TRUE);
                  }
                  WCHAR PageStatus[100];
                  wsprintf(PageStatus , L"Click [Extract] to extract Page %d of %d",PageNbr + 1, pPDev->Pages );
                  SetDlgItemText(hDlg,IDC_SAVEAS,PageStatus);
             }
             break;
         default:
             return FALSE;

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

void PMUIDriver::FixUpDevmode(IN HANDLE hPrinter , IN DEVMODE *pbIn, IN OUT PULONG pbOut)
{
     LPBYTE pPrinterInfo;
     DWORD cbNeeded;
     GetPrinter(hPrinter,2,NULL ,0,&cbNeeded);
     pPrinterInfo = (LPBYTE)malloc(cbNeeded);
     GetPrinter(hPrinter,2,pPrinterInfo,cbNeeded,&cbNeeded);
     size_t sz;
     VDEVMODE *pdm = 
         (VDEVMODE *)malloc(sz = (sizeof(VDEVMODE)));
     PDEVMODE pInDevmode = (PDEVMODE)pbIn;
     if(pInDevmode->dmSize == sizeof(DEVMODEW) 
             && pInDevmode->dmDriverExtra  == sizeof(VPDEVMODE))
          memcpy((void *)pdm,(void *)pInDevmode , sz);
     else
     {
          if(!IsSpooler())
               DrvDocumentProperties(0,hPrinter , 
                       ((PRINTER_INFO_2 *)pPrinterInfo)->pPrinterName
                       ,(PDEVMODE)pdm,(PDEVMODE)pbIn,
                       DM_IN_BUFFER | DM_OUT_BUFFER);
     }
     *pbOut = (ULONG)pdm;
     CreateGDIInfo(hPrinter,pdm);
     free(pPrinterInfo);
}
INT PMUIDriver::DrvDocumentEvent(
        HANDLE  hPrinter,
        HDC  hdc,
        int  iEsc,
        ULONG  cbIn,
        PULONG  pbIn,
        ULONG  cbOut,
        PULONG  pbOut
        )
{
     DUMPMSG("DrvDocumentEvent");
     static DWORD Pages = 0;
     static BOOL *pResetDC = NULL;
     static DWORD dwJobId;
     switch(iEsc)
     {

     case DOCUMENTEVENT_RESETDCPOST: 
     case DOCUMENTEVENT_CREATEDCPOST:
         {
              free((void *)pbIn[0]);
         }
         break;
     case DOCUMENTEVENT_RESETDCPRE:
         {
              DUMPMSG("DrvDocumentEventreset");
              if(!pResetDC)
                   pResetDC = (BOOL *)malloc(sizeof(BOOL) * (Pages + 1));    
              pResetDC[Pages] = TRUE;
         }
     case DOCUMENTEVENT_CREATEDCPRE:
         {
              CreateWin2kcompatibleSplFile(hPrinter);

              if(iEsc == DOCUMENTEVENT_CREATEDCPRE)
                   FixUpDevmode(hPrinter , (PDEVMODE)pbIn[2], pbOut);
              else 
                   FixUpDevmode(hPrinter , (PDEVMODE)pbIn[0], pbOut);

         }
         break;
     case DOCUMENTEVENT_STARTPAGE:
         DUMPMSG("DrvDocumentEventstartpage");
         if(!pResetDC)
         {
              pResetDC = (BOOL *)malloc(sizeof(BOOL));    
              pResetDC[0] =FALSE;
         }
         Pages++;
         break;
     case DOCUMENTEVENT_ENDPAGE:
         {
              DUMPMSG("DrvDocumentEventendpage");
              BOOL *pTemp = (BOOL *)malloc(sizeof(BOOL) * (Pages + 1));    
              if(pResetDC)
              {
                   memcpy(pTemp , pResetDC , sizeof(BOOL) * Pages );
                   free(pResetDC);
              }
              pResetDC = pTemp;
              pResetDC[Pages] = FALSE;
         }
         break;
     case DOCUMENTEVENT_ENDDOC:
         {
              /* You need administrator access to the spool directory for the below to work */
              if(IsSpooler())
              {
                   DWORD dwSize = sizeof(DEVDATA) - sizeof(VDEVMODE *) - sizeof(LPTSTR);

                   DEVDATA PDEV;
                   ZeroMemory(&PDEV , sizeof(DEVDATA));
                   ExtEscape(hdc,PDEV_ESCAPE, 0 , NULL , dwSize, (LPSTR)&PDEV); 
                   PDEV.pResetDC = pResetDC;
                   TCHAR SpoolFileName[MAX_PATH];
                   GetSpoolFileName(PDEV.dwJobId , SpoolFileName , hPrinter);
                   PDEV.hPDriver = hPrinter;
                   TCHAR TempSpoolFileName[MAX_PATH];
                   GetTempFile(NULL ,L"PM" , TempSpoolFileName);
                   CopyFile(SpoolFileName , TempSpoolFileName, FALSE);
                   PDEV.pSpoolFileName = TempSpoolFileName;
                   DWORD cbNeeded;
                   GetJob( PDEV.hPDriver, PDEV.dwJobId, 2, 0, 0, &cbNeeded);
                   LPBYTE pJobInfo = (LPBYTE)MALLOC(cbNeeded);
                   GetJob( PDEV.hPDriver, PDEV.dwJobId, 2, pJobInfo, cbNeeded, &cbNeeded);
                   PDEV.pCurDevmode = (VDEVMODE *)(((JOB_INFO_2 *)pJobInfo)->pDevMode);
                   PDEV.pDocument = (((JOB_INFO_2 *)pJobInfo)->pDocument);

                   DialogBoxParam( (HINSTANCE)hModDLL, MAKEINTRESOURCE(IDD_SAVEDIALOG),
                           GetDesktopWindow(), PMDialog, (LPARAM)(&PDEV));
                   dwJobId = PDEV.dwJobId;
                   free(pJobInfo);
                   DeleteFile(TempSpoolFileName);
              }
              Pages  = 0;
              if(pResetDC)
              {
                   free(pResetDC);
                   pResetDC = NULL;
              }

         }
         break;
     case DOCUMENTEVENT_DELETEDC:
         break;
     case DOCUMENTEVENT_STARTDOC:
         break;
     case DOCUMENTEVENT_ENDDOCPOST:
         {
              if(IsSpooler())
              {
                   PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ALL_ACCESS};
                   HANDLE hGPrinter;

                   WCHAR PrintMirrorName[256];
                   GetPrintMirrorName(PrintMirrorName);
                   OpenPrinter(PrintMirrorName, &hGPrinter, NULL);
                   LPVOID lpv = malloc(sizeof(HANDLE) + sizeof(DWORD));
                   *((HANDLE *)lpv) = hGPrinter;
                   *(DWORD *)((LPBYTE)lpv + sizeof(HANDLE)) = dwJobId;
                   HANDLE hThread =  CreateThread( NULL,   0, ThreadFunc, lpv, 0, NULL);     
              }
              //_CrtDumpMemoryLeaks();
         }
         break;
     }

     return DOCUMENTEVENT_SUCCESS;
}
