
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

static UINT CALLBACK PropSheetPageProc(
        HWND hwnd,
        UINT uMsg,
        LPPROPSHEETPAGE ppsp
        )
{
     UINT toReturn = 0;
     switch (uMsg)
     {
     case PSPCB_RELEASE:
         toReturn = 0;
         break;

     case PSPCB_CREATE:
         toReturn = 1;
         break;

     default:
         break;
     }

     return toReturn;
}

/*
 *  Fill the PROPSHEETPAGE which will be passed to CPSUI
 */
void FillInPropertyPage( PROPSHEETPAGE* psp, int idDlg, LPTSTR pszProc, DLGPROC pfnDlgProc, 
        VPrinterSettings *ps)
{
     if(psp)
     {
          psp->dwSize = sizeof(PROPSHEETPAGE);
          psp->dwFlags = PSP_USECALLBACK;
          psp->hInstance = hModDLL ;
          psp->pszTemplate = MAKEINTRESOURCE(idDlg);
          psp->pfnDlgProc = pfnDlgProc;
          psp->pszTitle = (LPTSTR)"";
          psp->lParam = (LPARAM)ps;
          psp->pfnCallback = PropSheetPageProc;
     }

}
static BOOL CALLBACK DevicePropertiesDialog(
        HWND hDlg,
        UINT message,
        UINT wParam,
        LONG lParam)
{
     switch (message)
     {
     case WM_NOTIFY:
         switch (((NMHDR FAR *) lParam)->code) 
         {
         case PSN_APPLY:
             {
                  WCHAR PrintMirrorName[256];
                  GetPrintMirrorName(PrintMirrorName);
                  DonotSharePrinterNT(PrintMirrorName,NULL,FALSE);
             }
             break;
         }
     default:
         return FALSE;
     }
     return TRUE;   
}
POINT GetPaperSizeFromIndex(HANDLE hPrinter , int curSel , PDEVMODEW pDevmode , VPrinterSettings *ps)
{
     int match = 0;
     VDEVMODE *pDevMode = (VDEVMODE *)malloc(sizeof(VDEVMODE));
     memcpy(pDevMode , pDevmode , sizeof(VDEVMODE));

     wcscpy((WCHAR *)((LPBYTE)pDevMode + sizeof(DEVMODEW)), ps->PrinterName);
     DWORD dwSize = DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERSIZE, 
             NULL,(PDEVMODE)pDevMode);
     LPTSTR pvOutput = (LPTSTR)malloc(sizeof(POINT) * dwSize);    
     DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERSIZE, pvOutput,
             (PDEVMODE)pDevMode);
     POINT *PointList = (POINT *)pvOutput;
     POINT PaperSize =  PointList[curSel];
     free(pvOutput);
     free(pDevMode);
     return PaperSize;
}
WORD GetPaperFromIndex(HANDLE hPrinter , int curSel , PDEVMODEW pDevmode , VPrinterSettings *ps)
{

     int match = 0;
     VDEVMODE *pDevMode = (VDEVMODE *)malloc(sizeof(VDEVMODE));
     memcpy(pDevMode , pDevmode , sizeof(VDEVMODE));

     wcscpy((WCHAR *)((LPBYTE)pDevMode + sizeof(DEVMODEW)), ps->PrinterName);
     DWORD dwSize = DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERS, 
             NULL,(PDEVMODE)pDevMode);
     LPTSTR pvOutput = (LPTSTR)malloc(sizeof(WORD) * dwSize);    
     DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERS, pvOutput,
             (PDEVMODE)pDevMode);
     WORD *PointList = (WORD *)pvOutput;
     WORD PaperSize =  PointList[curSel];
     free(pvOutput);
     free(pDevMode);
     return PaperSize;


}
int GetPaperIndex(HANDLE hPrinter , short PaperSize , PDEVMODEW pDevmode, VPrinterSettings *ps)
{
     int match = 0;
     VDEVMODE *pDevMode = (VDEVMODE *)malloc(sizeof(VDEVMODE));
     memcpy(pDevMode , pDevmode , sizeof(VDEVMODE));

     wcscpy((WCHAR *)((LPBYTE)pDevMode + sizeof(DEVMODEW)), ps->PrinterName);
     DWORD dwSize = DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERS, 
             NULL,(PDEVMODE)pDevMode);
     LPTSTR pvOutput = (LPTSTR)malloc(sizeof(WORD) * dwSize);    
     DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERS, pvOutput,
             (PDEVMODE)pDevMode);
     WORD *PointList = (WORD *)pvOutput;
     for(DWORD i = 0 ; i < dwSize ; i++)
          if(PaperSize == PointList[i])
          {
               match = i;
               break;
          }
     free(pvOutput);
     free(pDevMode);
     return match;
}

void SetPaperDimensions (HANDLE hPrinter ,HWND hDlg, int curSel , 
        PDEVMODEW pDevmode , VPrinterSettings *ps)
{

     POINT pt = GetPaperSizeFromIndex(hPrinter,curSel, (PDEVMODEW)(ps->ValidDevMode),ps);
     //we just make this inches;
     WCHAR Units[16];
     if(/*IsInchDimensions()*/!(ps->DimensionUnits))
     {
          pt.x = (pt.x * 100)/2540;
          pt.y = (pt.y * 100)/2540;
          wcscpy(Units, L"inches");
     }
     else
          wcscpy(Units,L"mms");

     WCHAR PaperSize[1024];
     if(ps->PaperOrient == DMORIENT_LANDSCAPE)
     {
          int temp = pt.x;
          pt.x = pt.y;
          pt.y = temp;
     }
     if(pt.x%10 && pt.y%10)
          wsprintf(PaperSize,L"%d.%d x %d.%d %s",pt.x/10,pt.x%10,pt.y/10,pt.y%10,Units);
     else if(pt.y%10)
          wsprintf(PaperSize,L"%d x %d.%d %s",pt.x/10,pt.y/10,pt.y%10,Units);
     else if(pt.x%10)
          wsprintf(PaperSize,L"%d.%d x %d %s",pt.x/10,pt.x%10,pt.y/10,Units);
     else
          wsprintf(PaperSize,L"%d x %d %s",pt.x/10,pt.y/10,Units);

     SetDlgItemText(hDlg , IDC_PAPERSIZE ,PaperSize); 
}

void FillPaperCombo(HANDLE hPrinter ,HWND hDlg ,VDEVMODE *pDevmode , VPrinterSettings *ps)
{

     VDEVMODE *pDevMode = (VDEVMODE *)malloc(sizeof(VDEVMODE));
     memcpy(pDevMode , pDevmode , sizeof(VDEVMODE));

     wcscpy((WCHAR *)((LPBYTE)pDevMode + sizeof(DEVMODEW)), ps->PrinterName);
     DWORD dwSize = DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERNAMES, 
             NULL,(PDEVMODE)pDevMode);
     LPTSTR pvOutput = (LPTSTR)malloc(dwSize * sizeof(WCHAR) * 64);    
     DrvDeviceCapabilities(hPrinter,ps->pszPrinterName, DC_PAPERNAMES, pvOutput,
             (PDEVMODE)pDevMode);
     HWND hwndPaper = GetDlgItem(hDlg ,IDC_PAPERCOMBO);

     SendMessage(hwndPaper , CB_RESETCONTENT, (WPARAM)0, (LPARAM) 0 );
     for(DWORD papers = 0 ; papers < dwSize ; papers++)
     {
          SendMessage( (HWND) hwndPaper,  CB_ADDSTRING, (WPARAM)0, 
                  (LPARAM)((papers * 64) + pvOutput));
     }
     free(pvOutput);
     int match = GetPaperIndex(hPrinter ,ps->PaperSize, (PDEVMODEW)pDevMode,ps);  
     SendMessage( (HWND) hwndPaper, CB_SETCURSEL, (WPARAM)match, (LPARAM)0);
     ps->PaperSize = GetPaperFromIndex(hPrinter , match , (PDEVMODEW)pDevmode ,ps);
     SetPaperDimensions(hPrinter,hDlg,match, (PDEVMODEW)(ps->ValidDevMode),ps);
     free(pDevMode);

}


static BOOL CALLBACK DocumentProperty(
        HWND hDlg,
        UINT message,
        UINT wParam,
        LONG lParam)
{
     static BOOL fBlocking = FALSE;
     static HBRUSH hBrushStatic = NULL;

     switch (message)
     {
     case WM_INITDIALOG:
         {
              VPrinterSettings * ps = (VPrinterSettings *)(((PROPSHEETPAGE *)lParam)->lParam);
              SetWindowLong(hDlg , DWL_USER , (LONG)lParam);
              HWND hwnd = GetDlgItem(hDlg , IDC_PRINTERCOMBO);
              /* 
               * Get the List of Printers to fill up the Printer combo and then set the 
               * printer name to the one in PrinterSettings 
               */
              DWORD dwReturned,dwNeeded;
              EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 4, NULL,
                      0, &dwNeeded, &dwReturned) ;

              LPBYTE pinfo4 = (LPBYTE)malloc (dwNeeded) ;

              EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 4, (PBYTE) pinfo4,
                      dwNeeded, &dwNeeded, &dwReturned) ;
              for(DWORD printers = 0 ; printers < dwReturned ; printers++)
              {
                   LPTSTR PrinterName = ((PRINTER_INFO_4 *)pinfo4)[printers].pPrinterName;
                   if(wcscmp(ps->pszPrinterName ,PrinterName)) 
                        SendMessage( (HWND) hwnd,  CB_ADDSTRING, (WPARAM)0, (LPARAM)PrinterName );
              }

              WCHAR *RealDriverName;
              LPBYTE pBuffer;
              HANDLE hPrinter = GetPrinterInfo(&pBuffer , ps->pszPrinterName );
              RealDriverName = (WCHAR *)((LPBYTE)(ps->ValidDevMode) + sizeof(DEVMODEW));
              WCHAR RealDriverName1[256];
              wcscpy(RealDriverName1 , RealDriverName);
              //     if(!IsSpooler())
              ValidateSetRealDriver(RealDriverName1);
              int match = SendMessage( (HWND) hwnd, CB_FINDSTRINGEXACT, (WPARAM)-1 ,  (LPARAM)RealDriverName1);
              wcscpy(ps->PrinterName , RealDriverName1);

              SendMessage( (HWND) hwnd, CB_SETCURSEL, (WPARAM)match, (LPARAM)0);
              free(pinfo4);
              /* Fill the Paper combo and set with the value from PrinterSettings */
              VDEVMODE * pDevMode; 
              pDevMode = ps->ValidDevMode;
              FillPaperCombo(hPrinter , hDlg , pDevMode , ps);
              /* Layout */
              VPDEVMODE *PrivateDevmode = &((VDEVMODE *)(pDevMode))->pdm;
              free(pBuffer);
              ClosePrinter(hPrinter);
              DUMPMSG("initdialog");
         }          
         break;
     case WM_CTLCOLORSTATIC:
         {
              HWND hwnd = (HWND)lParam;
              if(GetWindowLong(hwnd ,GWL_ID) == IDC_PAPERSIZE)
              {
                   SetTextColor((HDC)wParam,RGB(0,0,255));
                   SetBkColor((HDC)wParam , GetSysColor(COLOR_BTNFACE));
                   hBrushStatic = CreateSolidBrush ( GetSysColor (COLOR_BTNFACE)) ;
                   return (LRESULT)hBrushStatic;


              }
         }
         break;
     case WM_LBUTTONDOWN:
     case WM_MOUSEMOVE:
     case WM_LBUTTONUP:
         {

              RECT rect;
              RECT window;
              GetWindowRect(hDlg, &window);
              GetWindowRect(GetDlgItem(hDlg,IDC_PAPERSIZE) , &rect);
              POINT pt;
              pt.x = GET_X_LPARAM(lParam);
              pt.y = GET_Y_LPARAM(lParam);
              rect.left -= window.left;
              rect.top -= window.top;
              rect.right -= window.left;
              rect.bottom -= window.top;
              if(PtInRect(&rect , pt))
              {
                   SetCursor(LoadCursor(NULL,IDC_HAND));     

                   if(message == WM_LBUTTONDOWN)
                   {
                        PROPSHEETPAGE*  pPage = (PROPSHEETPAGE*)GetWindowLong( hDlg, DWL_USER );
                        VPrinterSettings * ps = (VPrinterSettings *)(pPage->lParam);
                        ps->DimensionUnits = ps->DimensionUnits ? FALSE : TRUE;
                        LPBYTE pBuffer = NULL;
                        HANDLE hPrinter = GetPrinterInfo(&pBuffer, 
                                ps->pszPrinterName);
                        int curSel = SendMessage( GetDlgItem(hDlg, IDC_PAPERCOMBO),
                                CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                        SetPaperDimensions(hPrinter,hDlg,curSel, (PDEVMODEW)(ps->ValidDevMode),ps);
                        ClosePrinter(hPrinter);
                        free(pBuffer);
                   }
              }
              else
              {
                   SetCursor(LoadCursor(NULL,IDC_ARROW));
              }
         }
         return FALSE;
     case WM_COMMAND:
         {
              PROPSHEETPAGE*  pPage = (PROPSHEETPAGE*)GetWindowLong( hDlg, DWL_USER );
              VPrinterSettings * ps = (VPrinterSettings *)(pPage->lParam);
              switch(LOWORD(wParam))
              {
              case IDC_PRINTERCOMBO:
                  {
                       if(HIWORD(wParam) == CBN_SELCHANGE)
                       {
                            LPBYTE pBuffer;
                            HANDLE hPrinter = GetPrinterInfo(&pBuffer , ps->pszPrinterName);
                            VDEVMODE *pDevMode;
                            pDevMode = ps->ValidDevMode;
                            int curSel = SendMessage( GetDlgItem(hDlg, IDC_PRINTERCOMBO),
                                    CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                            SendMessage( GetDlgItem(hDlg, IDC_PRINTERCOMBO),
                                    CB_GETLBTEXT, (WPARAM)curSel, (LPARAM)ps->PrinterName);
                            FillPaperCombo(hPrinter , hDlg , pDevMode , ps);
                            //PaperPreview(hDlg , ps);
                            free(pBuffer);
                            ClosePrinter(hPrinter);

                       }
                  }
                  break;

              case IDC_PAPERCOMBO:
                  {
                       if(HIWORD(wParam) == CBN_SELCHANGE)
                       {
                            LPBYTE pBuffer = NULL;
                            HANDLE hPrinter = GetPrinterInfo(&pBuffer , ps->pszPrinterName);
                            int curSel = SendMessage( GetDlgItem(hDlg, IDC_PAPERCOMBO),
                                    CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                            ps->PaperSize =
                                GetPaperFromIndex(hPrinter,curSel, (PDEVMODEW)(ps->ValidDevMode),ps);
                            SetPaperDimensions(hPrinter,hDlg,curSel, (PDEVMODEW)(ps->ValidDevMode),ps);
                            //PaperPreview(hDlg , ps);
                            ClosePrinter(hPrinter);
                            free(pBuffer);
                       }
                  }
                  break;
              }
         }
         break;
     case WM_NOTIFY:
         switch (((NMHDR FAR *) lParam)->code) 
         {
         case PSN_APPLY:
             {
                  PROPSHEETPAGE*  pPage = (PROPSHEETPAGE*)GetWindowLong( hDlg, DWL_USER );
                  VPrinterSettings * ps = (VPrinterSettings *)(pPage->lParam);
                  DWORD           cpsResult;
                  cpsResult = ps->pfnComPropSheet( ps->hComPropSheet,
                          CPSFUNC_SET_RESULT, (LPARAM)ps->handle, CPSUI_OK );
                  break;
             }

         case PSN_RESET:
             break;

         case PSN_SETACTIVE:
             break;

         default:
             return FALSE;

         }
         break;
     case WM_DESTROY:
         if(hBrushStatic)
              DeleteObject(hBrushStatic);
         break;
     default:
         return FALSE;
     }
     return TRUE;   
}

void ValidateSetRealDriver(WCHAR *RealDriverName, VDEVMODE *pdm)
{
     PRINTER_DEFAULTS  defaults = {NULL,NULL,PRINTER_ACCESS_USE};
     HANDLE hPrinter;
     //OutputDebugString(L"Enter ValidateSetRealDriverName");
     if(!OpenPrinter(RealDriverName , &hPrinter , &defaults))
     {
          DWORD dwReturned,dwNeeded;
          EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL,
                  0, &dwNeeded, &dwReturned) ;

          LPBYTE pinfo4 = (LPBYTE)malloc (dwNeeded) ;

          EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
                  NULL, 2, (PBYTE) pinfo4,
                  dwNeeded, &dwNeeded, &dwReturned) ;
          PRINTER_INFO_2 *pi = (PRINTER_INFO_2 *)pinfo4;
          WCHAR PrintMirrorName[256];
          int count = 0;
          BOOL inprintmirror = FALSE,inRealDriver = FALSE;
          for(DWORD i = 0 ; i < dwReturned ; i++)
          {
               //OutputDebugString(pi[i].pDriverName);
               if(!wcscmp(pi[i].pDriverName , L"PrintMirror"))
               {
                    wcscpy(PrintMirrorName , pi[i].pPrinterName);
                    inprintmirror = TRUE;
               }
               else
               {
                    wcscpy(RealDriverName , pi[i].pPrinterName);
                    inRealDriver = TRUE;
               }
               if(inprintmirror == TRUE && inRealDriver == TRUE)
                    break;
          }
          free(pi);

          // Fill in the PRINTER_DEFAULTS struct to get full permissions.
          PRINTER_DEFAULTS pd;
          ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
          pd.DesiredAccess = PRINTER_ALL_ACCESS;
          OpenPrinter( PrintMirrorName, &hPrinter, &pd );
          if(dwReturned == 1)
          {
               DeletePrinter(hPrinter);
               return;
          }
          for(i = 2/*8*/ ; i < 9/*10*/ ; i++)
          {
               if(i == 8)
               {
                    GetPrinter( hPrinter, i, NULL, 0, &dwNeeded );
                    LPBYTE pi2 = (LPBYTE)malloc( dwNeeded );
                    GetPrinter( hPrinter, i, (LPBYTE)pi2, dwNeeded, &dwNeeded );
                    wcscpy((WCHAR *)((LPBYTE)(((PRINTER_INFO_8 *)pi2)->pDevMode)
                                + sizeof(DEVMODE)) 
                            , RealDriverName);
                    if(!SetPrinter( hPrinter, i, (LPBYTE)pi2, 0 ))
                    {
                         int k = GetLastError();
                    }

                    free(pi2);
               }
               /*
               else if(i == 2)
               {
                    GetPrinter( hPrinter, i, NULL, 0, &dwNeeded );
                    LPBYTE pi2 = (LPBYTE)malloc( dwNeeded );
                    GetPrinter( hPrinter, i, (LPBYTE)pi2, dwNeeded, &dwNeeded );
                    wcscpy((WCHAR *)((LPBYTE)(((PRINTER_INFO_2 *)pi2)->pDevMode)
                                + sizeof(DEVMODE)) 
                            , RealDriverName);
                    SetPrinter( hPrinter, i, (LPBYTE)pi2, 0);
                    free(pi2);
               }
               */
          }
          if(pdm)
          {
               VPDEVMODE *PrivateDevmode = (VPDEVMODE *)((LPBYTE)pdm + sizeof(DEVMODEW));
               wcscpy(PrivateDevmode->PrinterName, RealDriverName);
          }
          SetRealDriverName(PrintMirrorName, RealDriverName);
          ClosePrinter(hPrinter);
     }
     else
          ClosePrinter(hPrinter);
     //OutputDebugString(L"Leave ValidateSetRealDriverName");

}

//-----------------------------------------------------------------------------
LONG  PMUIDriver::DrvDocumentProperties(HWND hwnd, HANDLE hPrinter, PWSTR lpszDeviceName,
        PDEVMODEW pdmOutput,PDEVMODEW pdmInput, DWORD fMode,BOOL fromApp)
{
     /*
      *  Take the RealDriver Name and then keep it
      *  else look the other way
      */
     if(FirstTime)
     {
          FirstTime = FALSE;
          memset(&DllDevmode , 0 , sizeof(VDEVMODE));
     }
     WCHAR RealDriverName[256];
     HANDLE hRPrinter;
     LPBYTE pBuffer;
     BOOL GetPrinterFail = FALSE;
     /*
      * If there is an incoming devmode, then take the DriverName from the
      * the private  part
      * else wherever it is stored.
      */
     WORD PaperSize = DMPAPER_LETTER;
     short Orient = DMORIENT_PORTRAIT;
     WORD PaperLength; 
     WORD PaperWidth;
     if(pdmInput && (fMode & DM_IN_BUFFER) && pdmInput->dmSize == sizeof(DEVMODEW)
             && pdmInput->dmDriverExtra == sizeof(VPDEVMODE))
     {
          VPDEVMODE *PrivateDevmode = (VPDEVMODE *)((LPBYTE)pdmInput + sizeof(DEVMODEW));
          if(pdmInput->dmFields & DM_PAPERSIZE)
               PaperSize = pdmInput->dmPaperSize;
          if(pdmInput->dmFields & DM_ORIENTATION)
               Orient = pdmInput->dmOrientation;
          if(pdmInput->dmFields & (DM_PAPERWIDTH | DM_PAPERLENGTH))
          {
               PaperLength = pdmInput->dmPaperLength;
               PaperWidth = pdmInput->dmPaperWidth;
          }
          wcscpy(RealDriverName ,PrivateDevmode->PrinterName);
          /* we maintain our internal dll devmode,modify as per the incoming*/
          if(fromApp)
               memcpy(&DllDevmode , pdmInput , sizeof(VDEVMODE));
     }
     else
     {
          /*
           * It happens that during installation,DrvDocumentProperties call
           * results in a 0 size for the private part,in which case get to 
           * the helper function GetRealDriverName.Its beleived that 
           * GetRealDriverName(...) won't be called again and the GetPrinter's
           * private part should get the RealDriverName.
           */
          LPBYTE pBuffer = NULL;
          HANDLE hPrinter = NULL;
          PDEVMODE pdm = NULL;

          if(DllDevmode.dm.dmSize == 0)
          {
               PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ALL_ACCESS };
               OpenPrinter(lpszDeviceName,&hPrinter,&defaults);
               DWORD cbNeeded , dwret = -1;   
               if(GetPrinterData(hPrinter,L"PrinterSettings",NULL,
                           (LPBYTE)pBuffer,0,&cbNeeded) == ERROR_MORE_DATA)
               {
                    pBuffer = (LPBYTE)malloc(cbNeeded);

                    dwret = GetPrinterData(hPrinter,L"PrinterSettings",NULL,
                            (LPBYTE)pBuffer,cbNeeded,&cbNeeded);
               }
               if(dwret == ERROR_SUCCESS)
                    pdm = (PDEVMODE)pBuffer;
               else
               {
                    ClosePrinter(hPrinter);
                    hPrinter = GetPrinterInfo(&pBuffer , lpszDeviceName);
                    if(pBuffer)
                         pdm = ((PRINTER_INFO_2 *)pBuffer)->pDevMode;
               }
               if(fromApp && pdm)
                    memcpy(&DllDevmode , pdm , sizeof(VDEVMODE));

          }
          else
               pdm = (PDEVMODE)&DllDevmode;

          if(pdm && pdm->dmDriverExtra == sizeof(VPDEVMODE))
          {
               VPDEVMODE *PrivateDevmode = (VPDEVMODE *)((LPBYTE)pdm + sizeof(DEVMODEW));
               wcscpy(RealDriverName ,PrivateDevmode->PrinterName);
               //OutputDebugString(PrivateDevmode->PrinterName);
               PaperSize = pdm->dmPaperSize;
               Orient = pdm->dmOrientation;
               PaperLength = pdm->dmPaperLength;
               PaperWidth = pdm->dmPaperWidth;
          }
          else
          {
               GetPrinterFail  = TRUE;
               GetRealDriverName(hPrinter,RealDriverName);
          }
          if(hPrinter)
          {
               ClosePrinter(hPrinter);
               free(pBuffer);
          }
     }
     /*
      * The incoming devmode is ours. 
      * 1)Get the handle to the Real Printer 
      * 2) Get the real driver's devmode size.
      * 3) Allocate memory and get the  real driver's devmode
      * 4) Copy the public part of the incoming devmode to the 
      *    public part of the devmode in (3)
      * 5) Allocate memory for the output devmode if there is output
      *    devmode buffer that comes in
      * 6) Call the Real Driver's DocumentProperties with the hack above.
      * 7) Copy the validated public part of the devmode  into out public part.
      * 8) Copy the RealDriverName to out private part.
      * 9) Clean up hRPrinter and the dummy devmode's for the real printer.
      */

     VDEVMODE  *pdm  = &(DllDevmode);
//     if(!IsSpooler())
          ValidateSetRealDriver(RealDriverName,pdm);
     hRPrinter = GetPrinterInfo(&pBuffer , RealDriverName);
     free(pBuffer);
     if(hRPrinter == NULL)
          return 0;   // fail gracefully
     LONG sz = DocumentProperties(0,hRPrinter , RealDriverName,0,0,0);
     PDEVMODEW  pdmInput1 = NULL;
     PDEVMODEW  pdmOutput1 = NULL;
     pdmInput1 = (PDEVMODEW)malloc(sz);
     DocumentProperties(0,hRPrinter , RealDriverName,pdmInput1,0,DM_OUT_DEFAULT);
     int dmDriverExtra = pdmInput1->dmDriverExtra;
     if(pdmInput && (fMode & DM_IN_BUFFER))
     {
          memcpy(pdmInput1,pdmInput,sizeof(DEVMODEW));
          pdmInput1->dmDriverExtra = dmDriverExtra;
     }
     else if(GetPrinterFail == FALSE)
     {
          LPBYTE pBuffer;
          ClosePrinter(GetPrinterInfo(&pBuffer , lpszDeviceName));
          memcpy(pdmInput1,((PRINTER_INFO_2 *)pBuffer)->pDevMode,sizeof(DEVMODEW));
          pdmInput1->dmDriverExtra = dmDriverExtra;
          free(pBuffer);
     }

     if(pdmOutput)
     {
          pdmOutput1 = (PDEVMODEW)malloc(sz);
     }

     LONG ret = DocumentProperties(0,hRPrinter,
             RealDriverName,
             pdmOutput1,pdmInput1,fMode & (~DM_IN_PROMPT));
     if((fMode & DM_OUT_BUFFER) && pdmOutput)
     {
          memset(pdmOutput , 0 , sizeof(VDEVMODE));
          memcpy(pdmOutput,pdmOutput1,sizeof(DEVMODEW));
          pdmOutput->dmDriverExtra = sizeof(VPDEVMODE);
          //   if(PaperSize != -1)
          if(!pdmInput && (GetPrinterFail == FALSE))
          { 
               pdmOutput->dmPaperSize = PaperSize;
               pdmOutput->dmFields |= DM_PAPERSIZE;

               pdmOutput->dmOrientation = Orient;
               pdmOutput->dmFields |= DM_ORIENTATION;
          }
          VPDEVMODE *PrivateDevmode = &(((VDEVMODE *)pdmOutput)->pdm);
          /* 
             Keep adding the members to comment here for clarity
             1)RealDriverName
             2)Layout
             Any future ...
           */   
          wcscpy(PrivateDevmode->PrinterName , RealDriverName);
          wcsncpy(pdmOutput->dmDeviceName , lpszDeviceName,
                  wcslen(lpszDeviceName) > CCHDEVICENAME 
                  ? CCHDEVICENAME :wcslen(lpszDeviceName)  + 1);
     }
     ClosePrinter(hRPrinter);
     if(pdmInput1)
          free(pdmInput1);
     if(pdmOutput1)
          free(pdmOutput1);
     return ret;
}
//-----------------------------------------------------------------------------
BOOL  PMUIDriver::DevQueryPrintEx(PDEVQUERYPRINT_INFO  pDQPInfo)
{
     return TRUE;
}


BOOL DonotSharePrinterNT( LPTSTR szPrinterName, LPTSTR szShareName, BOOL
        bShare )
{

     HANDLE            hPrinter;
     PRINTER_DEFAULTS   pd;
     DWORD            dwNeeded;
     PRINTER_INFO_2      *pi2;

     // Fill in the PRINTER_DEFAULTS struct to get full permissions.
     ZeroMemory( &pd, sizeof(PRINTER_DEFAULTS) );
     pd.DesiredAccess = PRINTER_ALL_ACCESS;
     if( ! OpenPrinter( szPrinterName, &hPrinter, &pd ) )
     {
          // OpenPrinter() has failed - bail out.
          return FALSE;
     }
     // See how big a PRINTER_INFO_2 structure is.
     if( ! GetPrinter( hPrinter, 2, NULL, 0, &dwNeeded ) )
     {
          if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
          {
               // GetPrinter() has failed - bail out.
               ClosePrinter( hPrinter );
               return FALSE;
          }
     }
     // Allocate enough memory for a PRINTER_INFO_2 and populate it.
     pi2 = (PRINTER_INFO_2 *)malloc( dwNeeded );
     if( pi2 == NULL )
     {
          // malloc() has failed - bail out.
          ClosePrinter( hPrinter );
          return FALSE;
     }
     if( ! GetPrinter( hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded ) )
     {
          // Second call to GetPrinter() has failed - bail out.
          free( pi2 );
          ClosePrinter( hPrinter );
          return FALSE;
     }
     // We won't mess with the security on the printer.
     pi2->pSecurityDescriptor = NULL;
     BOOL set = TRUE;
     if(!(pi2->Attributes & PRINTER_ATTRIBUTE_SHARED))
          set = FALSE;
     // If you want to share the printer, set the bit and the name.
     if( bShare )
     {
          pi2->pShareName = szShareName;
          pi2->Attributes |= PRINTER_ATTRIBUTE_SHARED;
     }
     else // Otherwise, clear the bit.
     {
          pi2->Attributes = pi2->Attributes & (~PRINTER_ATTRIBUTE_SHARED);
     }
     // Make the change.
     if(set && ! SetPrinter( hPrinter, 2, (LPBYTE)pi2, 0 ) )
     {
          // SetPrinter() has failed - bail out
          free( pi2 );
          ClosePrinter( hPrinter );
          return FALSE;
     }
     // Clean up.
     free( pi2 );
     ClosePrinter( hPrinter );
     return TRUE;
} 

//-----------------------------------------------------------------------------
LONG  PMUIDriver::DrvDevicePropertySheets(PPROPSHEETUI_INFO  pPSUIInfo, LPARAM  lParam)
{

     DUMPMSG("DrvDevicePropertySheets");
     PDEVICEPROPERTYHEADER pDPHdr;
     if ((!pPSUIInfo) ||
             (!(pDPHdr = (PDEVICEPROPERTYHEADER)pPSUIInfo->lParamInit))) {

          SetLastError(ERROR_INVALID_DATA);
          return(ERR_CPSUI_GETLASTERROR);
     }
     switch (pPSUIInfo->Reason)
     {
     case PROPSHEETUI_REASON_INIT:
         {


              DUMPMSG("DrvDevicePropertySheets Init ");
              PROPSHEETPAGE *psp = (PROPSHEETPAGE *)malloc(sizeof(PROPSHEETPAGE));
              memset(psp ,0, sizeof(PROPSHEETPAGE)); 
              FillInPropertyPage( psp, IDD_DEVICEDIALOG, TEXT(""), DevicePropertiesDialog,NULL);
              pPSUIInfo->UserData = NULL;

              if (pPSUIInfo->pfnComPropSheet(pPSUIInfo->hComPropSheet,
                          CPSFUNC_ADD_PROPSHEETPAGE,
                          (LPARAM)psp,
                          (LPARAM)0))
                   pPSUIInfo->Result = CPSUI_CANCEL;
         }
         return 1;

     case PROPSHEETUI_REASON_DESTROY:
         {
              HANDLE hPrinter =  pDPHdr->hPrinter;
              DWORD dwNeeded;
              GetPrinter( hPrinter, 2, NULL, 0, &dwNeeded );
              LPBYTE pi2 = (LPBYTE)malloc( dwNeeded );
              GetPrinter( hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded );
              //OutputDebugStringW(((PRINTER_INFO_2 *)pi2)->pPrinterName);   
              DonotSharePrinterNT(((PRINTER_INFO_2 *)pi2)->pPrinterName,NULL,FALSE);
              free(pi2);
              return 1;

         }
     }

     return 1;
}



void ValidateDevMode(VDEVMODE * ValidDevMode , VDEVMODE * inDevMode)
{
     PDEVMODE indm = (PDEVMODE)(inDevMode);
     PDEVMODE validdm = (PDEVMODE)ValidDevMode;
     if(indm)
     {
          if(indm->dmFields & DM_ORIENTATION)
          {
               validdm->dmOrientation = indm->dmOrientation;
               validdm->dmFields |= DM_ORIENTATION;
          }
          if(indm->dmFields & DM_PAPERSIZE)
          {
               validdm->dmPaperSize = indm->dmPaperSize;
               validdm->dmFields |= DM_PAPERSIZE;
          }
          if(indm->dmFields & DM_PRINTQUALITY)
          {
               validdm->dmPrintQuality = indm->dmPrintQuality;
               validdm->dmFields |= DM_PRINTQUALITY;
          }
          if(indm->dmFields & DM_COLOR)
          {
               validdm->dmColor = indm->dmColor;
               validdm->dmFields |= DM_COLOR;
          }

          if(indm->dmFields & DM_DUPLEX)
          {
               validdm->dmDuplex = indm->dmDuplex;
               validdm->dmFields |= DM_DUPLEX;
          }

          if(indm->dmFields & DM_COLLATE)
          {
               validdm->dmCollate = indm->dmCollate;
               validdm->dmFields |= DM_COLLATE;
          }
          if(indm->dmFields & DM_COPIES)
          {
               validdm->dmCollate = indm->dmCopies;
               validdm->dmFields |= DM_COPIES;
          }
          memcpy((void *)&(ValidDevMode->pdm) ,(void *)&(inDevMode->pdm) , sizeof(VPDEVMODE));
     }
}







//-----------------------------------------------------------------------------
LONG  PMUIDriver::DrvDocumentPropertySheets(PPROPSHEETUI_INFO  pPSUIInfo, LPARAM  lParam)
{
     DUMPMSG("DrvDocumentPropertySheets");
     /*
      * Info without a dialog box
      */
     if (pPSUIInfo == NULL)
     {
          PDOCUMENTPROPERTYHEADER pDPH;
          pDPH = (PDOCUMENTPROPERTYHEADER)lParam;
          if (pDPH == NULL)
          {
               return -1;
          }
          /* return the devmode size */
          if (pDPH->fMode == 0 || pDPH->pdmOut == NULL)
          {
               pDPH->cbOut = sizeof(VDEVMODE);
               return pDPH->cbOut;
          }
          /* call the master function, DrvDocumentProperties for other processing */
          if (pDPH->fMode)
          {
               LONG pcbNeeded = pDPH->cbOut;
               if(DrvDocumentProperties(NULL, pDPH->hPrinter, pDPH->pszPrinterName, 
                       pDPH->pdmOut, pDPH->pdmIn, pDPH->fMode,TRUE) == 0)
                    return 0;
          }
          return 1;
     }

     switch (pPSUIInfo->Reason)
     {
     case PROPSHEETUI_REASON_INIT:
         {
              /*
               * Initialize the Property sheet
               */
              WCHAR RealDriverName1[256];
              PROPSHEETPAGE *psp = (PROPSHEETPAGE *)malloc(sizeof(PROPSHEETPAGE));
              PDOCUMENTPROPERTYHEADER pDPH = (PDOCUMENTPROPERTYHEADER)lParam;
              WCHAR *RealDriverName;
              LPBYTE pBuffer = NULL;

              PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ALL_ACCESS };
              HANDLE hPrinter;
              OpenPrinter(pDPH->pszPrinterName,&hPrinter,&defaults);
              DWORD cbNeeded = 0;   
              BOOL Fail = FALSE;
              PDEVMODE pdm = NULL;
              if(GetPrinterData(hPrinter,L"PrinterSettings",NULL,
                          (LPBYTE)pBuffer,0,&cbNeeded) == ERROR_MORE_DATA)
              {
                   pBuffer = (LPBYTE)malloc(cbNeeded);

                   DWORD dwret = GetPrinterData(hPrinter,L"PrinterSettings",NULL,
                           (LPBYTE)pBuffer,cbNeeded,&cbNeeded);
                   if(dwret == ERROR_SUCCESS)
                        pdm = (PDEVMODE)pBuffer;
                   else
                        Fail = TRUE;
              }
              else
                   Fail = TRUE;

              if(Fail == TRUE)
              {
                   free(pBuffer);
                   ClosePrinter(hPrinter);
                   hPrinter = GetPrinterInfo(&pBuffer , pDPH->pszPrinterName);
                   if(pBuffer)
                        pdm = ((PRINTER_INFO_2 *)pBuffer)->pDevMode;
              }
              RealDriverName = (WCHAR *)((LPBYTE)(pdm) + sizeof(DEVMODEW));

              wcscpy(RealDriverName1 , RealDriverName);
              ClosePrinter(hPrinter);
              VDEVMODE  *pdm1  = &(DllDevmode);
              //     if(!IsSpooler())
              ValidateSetRealDriver(RealDriverName1,pdm1);

              memset(psp ,0, sizeof(PROPSHEETPAGE)); 
              /* Prepare the PrinterSettings which will be used by the PropetySheet Dialog */
              VPrinterSettings *ps = (VPrinterSettings *)malloc(sizeof(VPrinterSettings));
              memset(ps, 0 , sizeof(VPrinterSettings));
              /* Intialize the PrinterSettings ValidDevMode with the default and then later 
               * with the inDevmode and then ValidateDevMode
               */
              if(pDPH->fMode & DM_IN_BUFFER)
                   ps->inDevmode = (VDEVMODE *)(pDPH->pdmIn);

              ps->ValidDevMode = (VDEVMODE *)malloc(sizeof(VDEVMODE));
              if(Fail  == TRUE)
              {
                   //OutputDebugString(L"failure in propertysheet");
                   memcpy(ps->ValidDevMode , ((PRINTER_INFO_2 *)pBuffer)->pDevMode , 
                           sizeof(VDEVMODE));
              }
              else
                   memcpy(ps->ValidDevMode , pBuffer , sizeof(VDEVMODE));
              if(ps->inDevmode)
                   ValidateDevMode(ps->ValidDevMode , ps->inDevmode);
              free(pBuffer);

              /* Hold the pointer to outDevmode to be used in _SET_RESULT */
              if(pDPH->fMode & DM_OUT_BUFFER)
                   ps->outDevmode = (VDEVMODE *)(pDPH->pdmOut);

              /* Fill the PROPSHEETPAGE */
              FillInPropertyPage( psp, IDD_PRINTDIALOG, TEXT(""), DocumentProperty,ps);
              wcscpy(PrinterName , pDPH->pszPrinterName);
              /* This so that WM_INITDIALOG gets as lparam the PrinterSettings */
              pPSUIInfo->UserData = (ULONG_PTR)ps;
              DWORD result;
              /* Fill up the other members */
              ps->pfnComPropSheet = pPSUIInfo->pfnComPropSheet;
              ps->hComPropSheet = pPSUIInfo->hComPropSheet;
              ps->fMode = pDPH->fMode;
              ps->hPrinter = pDPH->hPrinter;
              ps->PaperSize = (((PDEVMODE)(ps->ValidDevMode))->dmFields & DM_PAPERSIZE) ? 
                  ((PDEVMODE)(ps->ValidDevMode))->dmPaperSize : DMPAPER_LETTER;
              ps->DimensionUnits = FALSE;
              ps->PaperOrient = (((PDEVMODE)(ps->ValidDevMode))->dmFields & DM_ORIENTATION) ? 
                  ((PDEVMODE)(ps->ValidDevMode))->dmOrientation : DMORIENT_PORTRAIT;
              if(pDPH->pszPrinterName)
              {
                   ps->pszPrinterName = (LPTSTR)malloc(sizeof(WCHAR) * (wcslen(pDPH->pszPrinterName)+ 1));
                   wcscpy(ps->pszPrinterName , pDPH->pszPrinterName);
              }
              if (result = pPSUIInfo->pfnComPropSheet(pPSUIInfo->hComPropSheet,
                          CPSFUNC_ADD_PROPSHEETPAGE,
                          (LPARAM)psp, 
                          (LPARAM)0))
              {
                   ps->handle = (HPROPSHEETPAGE)result;
                   pPSUIInfo->Result = CPSUI_CANCEL;
              }
              free(psp);

         }
         return 1;

     case PROPSHEETUI_REASON_SET_RESULT:
         {
              PSETRESULT_INFO     pSRInfo;

              pSRInfo = (PSETRESULT_INFO) lParam;

              if (pSRInfo->Result == CPSUI_OK)
              {
                   /* User pressed ok, modify ValidDevmode  and send it to DrvDocumentProperties */
                   PDOCUMENTPROPERTYHEADER pDPH = (PDOCUMENTPROPERTYHEADER)lParam;
                   VPrinterSettings *ps = (VPrinterSettings *)(pPSUIInfo->UserData);
                   PDEVMODE pdmValid = (PDEVMODE)(ps->ValidDevMode);
                   pdmValid->dmPaperSize = ps->PaperSize;
                   pdmValid->dmFields |= DM_PAPERSIZE;
                   pdmValid->dmFields |= DM_ORIENTATION;
                   pdmValid->dmOrientation = ps->PaperOrient;
                   DrvDocumentProperties(NULL, ps->hPrinter, ps->pszPrinterName, 
                           (DEVMODE *)(ps->outDevmode), (DEVMODE *)(ps->ValidDevMode), (ps->fMode & ~DM_PROMPT));
                   /* 
                    * Set the private part of the outDevmode with the corresponding members from
                    * PrinterSettings
                    */
                   VDEVMODE *pdmOutput = (VDEVMODE*)(ps->outDevmode);
                   VPDEVMODE *PrivateDevmode = &(pdmOutput->pdm);
                   /* We don't use private PaperSize yet */
                   PrivateDevmode->PaperSize = ps->PaperSize;
                   wcscpy(PrivateDevmode->PrinterName ,ps->PrinterName );
                   DrvDocumentProperties(NULL, ps->hPrinter, ps->pszPrinterName, 
                           NULL, (DEVMODE *)(ps->outDevmode), DM_IN_BUFFER,TRUE);
                   if(bIsExplorer)
                   {
                        HANDLE hPrinter;
                        PRINTER_DEFAULTS defaults = { NULL, NULL, PRINTER_ALL_ACCESS };

                        OpenPrinter(
                                ps->pszPrinterName,         // printer or server name
                                &hPrinter,          // printer or server handle
                                &defaults   // printer defaults
                                );
                        DWORD dwret = SetPrinterData(
                                hPrinter,    // handle to printer or print server
                                L"PrinterSettings",  // data to set
                                REG_BINARY,         // data type
                                (LPBYTE)(ps->outDevmode),       // configuration data buffer
                                sizeof(VDEVMODE)
                                );
                        SetRealDriverName(ps->pszPrinterName , PrivateDevmode->PrinterName);
                        ClosePrinter(hPrinter);
                   }
                   wcscpy(RealPrinterName , ps->PrinterName);
              }
         }
         break;
     case PROPSHEETUI_REASON_DESTROY:
         {
              /* Cleanup */
              VPrinterSettings *ps = (VPrinterSettings *)(pPSUIInfo->UserData);
              free(ps->pszPrinterName);
              free(ps->ValidDevMode);
              free((void *)(ps));
              pPSUIInfo->UserData = NULL;
         }
         return 1;
     }

     return 1;
}

//-----------------------------------------------------------------------------
BOOL  PMUIDriver::DrvPrinterEvent(LPWSTR  pPrinterName, INT  DriverEvent, DWORD  Flags, LPARAM  lParam)
{
     if(DriverEvent == PRINTER_EVENT_INITIALIZE)
     {
          WCHAR RealDriverName[256];
          memset(RealDriverName , 0 , sizeof(WCHAR) * 256);
          InstalledPrinter(RealDriverName , pPrinterName);
          SetRealDriverName(pPrinterName , RealDriverName);
     }
     return TRUE;
}
