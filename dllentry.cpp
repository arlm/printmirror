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
