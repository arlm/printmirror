
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

#ifdef _DEBUG
#ifdef USERMODE_DRIVER
#define DUMPMSG(msg) { if(msg) {OutputDebugStringA(msg); \
     OutputDebugStringA("\r\n");} }  // I  am doing  this   to  flush.
#else
#define DUMPMSG(msg)   
#endif
#else
#define DUMPMSG(msg)   
#endif
#ifndef USERMODE_DRIVER
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>	
#include <limits.h>
#include <windef.h>
#include <wingdi.h>
#include <winerror.h>
     extern "C" {
#include <winddi.h>
     }
typedef BOOL   (APIENTRY *PFN_DrvEnableDriver)(ULONG,ULONG,PDRVENABLEDATA);
typedef DHPDEV (APIENTRY *PFN_DrvEnablePDEV) (PDEVMODEW,LPWSTR,ULONG,HSURF*,ULONG,GDIINFO*,ULONG,PDEVINFO,HDEV,LPWSTR,HANDLE);
#define COPYMEMORY(a,b,c)  {LPBYTE ptr = (LPBYTE)a; LPBYTE ptr1 = (LPBYTE)b;\
     for (unsigned int i = 0 ; i < c; i++) *ptr++=*ptr1++;}
#define ZEROMEMORY(a,b) { LPBYTE ptr = (LPBYTE)a; for(unsigned int i = 0 ;i < b ; i++) *ptr++ = 0;}
#define FREE(X)  EngFreeMem(X)
#define MALLOC(x) EngAllocMem(0,x,ALLOC_TAG);
#define GETPRINTERDATA EngGetPrinterData
#define GETPRINTERDRIVER EngGetPrinterDriver
#else
#define UNICODE
#include   <stdlib.h>
#include   <windows.h>
#include   <winspool.h>
#include   <stdio.h>
#include   <wingdi.h>
#include   <winddi.h>
#define COPYMEMORY(a,b,c) CopyMemory(a,b,c)
#define ZEROMEMORY(a,b) ZeroMemory(a,b)
#define FREE(X)  free(X)
#define MALLOC(x) malloc(x)
#define GETPRINTERDATA GetPrinterData
#define GETPRINTERDRIVER GetPrinterDriver
#endif
#include "prntmrd.h"


     HANDLE hModDll;

     struct DEVDATA{
          HPALETTE hPalDefault ;
          HDEV hEngineDevice;
          HANDLE hPDriver;
          HSURF  hSurf;
          BOOL  binDocument;
          DWORD dwJobId;
          int Pages;
          GDIINFO DevCaps;
          DRVENABLEDATA pReal;
          DHPDEV dhRPdev; 
          HMODULE hRealDriver;
          int iDitherFormat; 
     };
#define  PPDEV  DEVDATA*
/*      Helper functions */

PFN GetRealFunc (ULONG  index , DRVENABLEDATA *pded)
{
     DRVFN  *func_ptr  = pded->pdrvfn;
     int   numfuncs  = pded->c;
     for(int i = 0 ; i <  numfuncs ; i++)
     {
          if(func_ptr->iFunc == index)
               return func_ptr->pfn;
          func_ptr++;
     }
     return  NULL;
}

BOOL
FillDevInfo(
        PPDEV   pPDev,
        DEVINFO *pDevInfo,
        int numcolors,
        ULONG *palette,
        LOGFONT *lf
        
        )

{
     /* In case of any probs uncomment this
      */
     //
     // Start with a clean slate.
     //
#ifdef USERMODE_DRIVER
     ZEROMEMORY(pDevInfo, sizeof(DEVINFO));

     pDevInfo->flGraphicsCaps = GCAPS_ALTERNATEFILL  |
         GCAPS_HORIZSTRIKE    |
         GCAPS_VERTSTRIKE     |
         GCAPS_VECTORFONT | GCAPS_HALFTONE | GCAPS_OPAQUERECT |
         GCAPS_BEZIERS |
         GCAPS_WINDINGFILL;
     pDevInfo->cxDither = 0;
     pDevInfo->cyDither = 0;
     switch(numcolors)
     {
     case 2:
         pDevInfo->iDitherFormat = BMF_1BPP;
         break;
     case 16:
         pDevInfo->iDitherFormat = BMF_4BPP;
         break;
     case 256:
         pDevInfo->iDitherFormat = BMF_8BPP;
         break;
     case 0:
         pDevInfo->iDitherFormat = BMF_24BPP;
         break;
     }
     wcscpy((PWSTR)pDevInfo->lfDefaultFont.lfFaceName, lf->lfFaceName);
     pDevInfo->lfDefaultFont.lfEscapement = lf->lfEscapement;
     pDevInfo->lfDefaultFont.lfOrientation = lf->lfOrientation;
     pDevInfo->lfDefaultFont.lfHeight = lf->lfHeight;
     pDevInfo->lfDefaultFont.lfWidth = lf->lfWidth;
     pDevInfo->lfDefaultFont.lfWeight = lf->lfWeight;
     pDevInfo->lfDefaultFont.lfItalic = lf->lfItalic;
     pDevInfo->lfDefaultFont.lfUnderline = lf->lfUnderline;
     pDevInfo->lfDefaultFont.lfStrikeOut = lf->lfStrikeOut;
     pDevInfo->lfDefaultFont.lfPitchAndFamily = lf->lfPitchAndFamily;
     memcpy((void *)&(pDevInfo->lfAnsiVarFont), lf, sizeof(LOGFONT));
     memcpy((void *)&(pDevInfo->lfAnsiFixFont), lf, sizeof(LOGFONT));
     /*
     if(numcolors)
          pDevInfo->hpalDefault = EngCreatePalette(PAL_INDEXED, numcolors,palette, 0, 0, 0);
     else
     */
          pDevInfo->hpalDefault = EngCreatePalette(PAL_BGR, 0, 0, 0, 0, 0);
#else
     pDevInfo->cFonts = 0;
     pDevInfo->flGraphicsCaps2 = 0;
     pDevInfo->cxDither = 0;
     pDevInfo->cyDither = 0;
#endif
     pPDev->iDitherFormat = pDevInfo->iDitherFormat ;
     pPDev->hPalDefault = pDevInfo->hpalDefault;

     return(TRUE);
}


void GetRealDriverName(HANDLE hDriver,WCHAR RealDriverName[])
{
     DWORD cbNeeded;
     if(GETPRINTERDATA(
                 hDriver,    // handle to printer or print server
                 L"RealDriverName",  // value name
                 NULL,      // data type
                 (LPBYTE)RealDriverName,       // configuration data buffer
                 256,        // size of configuration data buffer
                 &cbNeeded   // bytes received or required 
                 ) == ERROR_SUCCESS)
          DUMPMSG("EnablePdev GetPrinter success");
}

//   I  have   put   34  functions    here   which  hold  relevance   for
//    printer   drivers.
static DRVFN  RasterFuncs[]  = 
{
     //--------------------------------------------------------------------
     // Document handling (general printing stuff)  [REQUIRED]
     //--------------------------------------------------------------------
     {INDEX_DrvEnablePDEV        ,   (PFN)DrvEnablePDEV          },
     {INDEX_DrvCompletePDEV      ,   (PFN)DrvCompletePDEV        },
     {INDEX_DrvDisablePDEV       ,   (PFN)DrvDisablePDEV         },
     {INDEX_DrvEnableSurface     ,   (PFN)DrvEnableSurface       },
     {INDEX_DrvDisableSurface    ,   (PFN)DrvDisableSurface      },
     //     {INDEX_DrvAssertMode        ,   (PFN)DrvAssertMode          },
     {INDEX_DrvResetPDEV         ,   (PFN)DrvResetPDEV           },
#ifdef USERMODE_DRIVER
     {INDEX_DrvDisableDriver     ,   (PFN)DrvDisableDriver       },
#endif
     //--------------------------------------------------------------------
     // Brush
     //--------------------------------------------------------------------
     {INDEX_DrvRealizeBrush      ,   (PFN)DrvRealizeBrush        },
     // Support for dithering of brushes
     //     {INDEX_DrvDitherColor       ,   (PFN)DrvDitherColor            },
     //--------------------------------------------------------------------
     // Vector
     //--------------------------------------------------------------------
     {INDEX_DrvStrokePath        ,   (PFN)DrvStrokePath          },
     {INDEX_DrvFillPath          ,   (PFN)DrvFillPath            },
     {INDEX_DrvStrokeAndFillPath ,   (PFN)DrvStrokeAndFillPath   },
     {INDEX_DrvPaint             ,   (PFN)DrvPaint               },
     {INDEX_DrvLineTo ,              (PFN)DrvLineTo              },
     //--------------------------------------------------------------------
     // Bitmap
     //--------------------------------------------------------------------
     {INDEX_DrvBitBlt            ,   (PFN)DrvBitBlt              },
     {INDEX_DrvCopyBits          ,   (PFN)DrvCopyBits            },
     {INDEX_DrvStretchBlt        ,   (PFN)DrvStretchBlt          },
#ifdef USERMODE_DRIVER
     {INDEX_DrvStretchBltROP     ,   (PFN)DrvStretchBltROP       },
#endif
     //     {INDEX_DrvCreateDeviceBitmap ,  (PFN)DrvCreateDeviceBitmap  },
     //     {INDEX_DrvDeleteDeviceBitmap ,  (PFN)DrvDeleteDeviceBitmap  },
     //--------------------------------------------------------------------
     // Text
     //--------------------------------------------------------------------
     {INDEX_DrvTextOut           ,   (PFN)DrvTextOut             },

     //--------------------------------------------------------------------
     // Escapes
     //--------------------------------------------------------------------
     {INDEX_DrvEscape            ,   (PFN)DrvEscape              },
     {INDEX_DrvDrawEscape        ,   (PFN)DrvDrawEscape          },



     //--------------------------------------------------------------------
     // Font
     //--------------------------------------------------------------------
     /*
        {INDEX_DrvDestroyFont       ,   (PFN)DrvDestroyFont         },
        {INDEX_DrvQueryFont,        (PFN)DrvQueryFont       },
        {INDEX_DrvQueryFontTree,    (PFN)DrvQueryFontTree   },
        {INDEX_DrvQueryFontData,    (PFN)DrvQueryFontData   },
        {INDEX_DrvFontManagement    ,   (PFN)DrvFontManagement      },
        {INDEX_DrvQueryAdvanceWidths, (PFN)DrvQueryAdvanceWidths},
      */


     //--------------------------------------------------------------------
     // Banding Support
     //--------------------------------------------------------------------

     {INDEX_DrvStartBanding      ,   (PFN)DrvStartBanding        },
     {INDEX_DrvNextBand          ,   (PFN)DrvNextBand            },
     {INDEX_DrvStartDoc          ,   (PFN)DrvStartDoc            },
     {INDEX_DrvEndDoc            ,   (PFN)DrvEndDoc              },

     {INDEX_DrvStartPage         ,   (PFN)DrvStartPage           },
     {INDEX_DrvSendPage          ,   (PFN)DrvSendPage            },


};


/***************************Export   Functions   by  Dll   ************************/
BOOL  CALLBACK
DllMain(
        HANDLE      hModule,
        ULONG       ulReason,
        PCONTEXT    pContext
       )
{
     switch (ulReason)
     {
     case DLL_PROCESS_ATTACH:
         {
              hModDll = hModule;
              /* keep the dll loaded */
              WCHAR wName[MAX_PATH];
              if (GetModuleFileName((HINSTANCE)hModule, wName, MAX_PATH)) 
                   LoadLibrary(wName);
              break;
         }

     case DLL_PROCESS_DETACH:
         break;
     }

     return TRUE;
}



BOOL CALLBACK
DrvEnableDriver(ULONG          iEngineVersion,
        ULONG          cj,
        DRVENABLEDATA *pded)
{

     DUMPMSG("DrvEnableDriver");
     if (iEngineVersion < DDI_DRIVER_VERSION) {

          return(FALSE);
     }

     if (cj < sizeof(DRVENABLEDATA)) {

          return(FALSE);
     }
     pded->iDriverVersion = DDI_DRIVER_VERSION_NT5;
     pded->c = sizeof(RasterFuncs)/sizeof(DRVFN);
     pded->pdrvfn = RasterFuncs;

     return TRUE;

}


/*****I    have   seen    this   getting   called   at   the
  first  print for a  spooler   session   and never   after******************/
#ifdef USERMODE_DRIVER
BOOL CALLBACK DrvQueryDriverInfo(DWORD dwMode, PVOID pBuffer, DWORD cbBuf, PDWORD pcbNeeded)
{
     DUMPMSG("DrvQueryDriverInfo");

     switch (dwMode)
     {
     case DRVQUERY_USERMODE:
         *pcbNeeded = sizeof(DWORD);

         if (pBuffer == NULL || cbBuf < sizeof(DWORD))
         {
              SetLastError(ERROR_INSUFFICIENT_BUFFER);
              return FALSE;
         }

         *((PDWORD) pBuffer) = TRUE;
         return TRUE;

     default:
         SetLastError(ERROR_INVALID_PARAMETER);
         return FALSE;
     }
}
#endif

BOOL CALLBACK DrvStartBanding( SURFOBJ *pso, POINTL *pptl )
{
     DUMPMSG("DrvStartBanding");
     return TRUE;
}

BOOL CALLBACK DrvNextBand( SURFOBJ *pso, POINTL *pptl )
{
     DUMPMSG("DrvNextBand");
     return TRUE;
}
/***************Document  Handling***********************************************/

ULONG palette[256];
DHPDEV CALLBACK
DrvEnablePDEV(
        DEVMODEW *pdm,
        LPWSTR    pwszLogAddress,
        ULONG     cPat,
        HSURF    *phsurfPatterns,
        ULONG     cjCaps,
        ULONG    *pdevcaps,
        ULONG     cjDevInfo,
        DEVINFO  *pdi,
        HDEV      pwszDataFile,   // PWSTR
        LPWSTR    pwszDeviceName,
        HANDLE    hDriver)
{
     DHPDEV dhpdev = NULL;
     DUMPMSG("DrvEnablePdev");
     PPDEV pPDev = (PPDEV)MALLOC(sizeof(DEVDATA));
     ZEROMEMORY(pPDev, sizeof(DEVDATA));
     pPDev->hPDriver = hDriver;
     pPDev->dwJobId = 9999; //REVISIT  here
     
     int  numcolors = 0;
     VDEVMODE *pDevMode = (VDEVMODE *)pdm;
     COPYMEMORY(pdevcaps , &(pDevMode->pdm.gi) , sizeof(GDIINFO));
     numcolors = pDevMode->pdm.numcolors;
     COPYMEMORY(palette , pDevMode->pdm.Palette , sizeof(ULONG) * 256);
     LOGFONT lf;
     COPYMEMORY(&lf , &(pDevMode->pdm.lf), sizeof(LOGFONT));

     COPYMEMORY(
             PVOID(&(pPDev->DevCaps)),(PVOID)pdevcaps, sizeof(GDIINFO));
     FillDevInfo(
             pPDev,
             pdi,numcolors,palette,&lf
             );
     pdi->flGraphicsCaps &= ~GCAPS_DONTJOURNAL;
     return((DHPDEV)pPDev);
}


    VOID CALLBACK
DrvCompletePDEV(DHPDEV dhpdev,HDEV hdev)
{
     DUMPMSG("DrvCompletePDEV");
     ((DEVDATA *)dhpdev)->hEngineDevice = hdev;
}


    VOID CALLBACK
DrvDisablePDEV(DHPDEV dhpdev)
{
     DUMPMSG("DisablePDev");
     EngDeletePalette(((DEVDATA *)dhpdev)->hPalDefault); 
     FREE((PPDEV)dhpdev);
     return;
}


    HSURF CALLBACK
DrvEnableSurface(DHPDEV dhpdev)
{

     HSURF hSurf = NULL;
     long  hooks;
     //----------------------------------------------------------------
     // Setup size of device
     //----------------------------------------------------------------
     SIZEL       deviceSize;

     DUMPMSG("DrvEnableSurface");

     deviceSize.cx =  ((DEVDATA *)dhpdev)->DevCaps.ulHorzRes;
     deviceSize.cy = ((DEVDATA *)dhpdev)->DevCaps.ulVertRes;


     hSurf = EngCreateDeviceSurface((DHSURF)dhpdev, deviceSize, ((DEVDATA *)dhpdev)->iDitherFormat);
     if(hSurf == NULL)
          ;          //OutputDebugStringW(L"surface failure");
     hooks= HOOK_BITBLT | HOOK_STRETCHBLT | HOOK_TEXTOUT |
         HOOK_STROKEPATH | HOOK_FILLPATH |  HOOK_COPYBITS | HOOK_STRETCHBLTROP|
         HOOK_STROKEANDFILLPATH;
     EngAssociateSurface( hSurf, ((DEVDATA *)dhpdev)->hEngineDevice, hooks );
     ((DEVDATA*)dhpdev)->hSurf = hSurf;
     return hSurf;
}


    VOID CALLBACK
DrvDisableSurface(DHPDEV dhpdev)
{
     /*  delete the bitmap now */
     DUMPMSG("DrvDisableSurface");
     EngDeleteSurface(((DEVDATA*)dhpdev)->hSurf);
     ((DEVDATA*)dhpdev)->hSurf = NULL;
     return;
}



/*
 * No real resource swapping here.
 * DrvRestPdev is visited in the following order
 * oldpdev = DrvEnablePdev(...)
 * DrvCompletepdev(oldpdev,...)
 * newpdev = DrvEnablepdev(...)
 * DrvResetpdev(old,new)
 * DrvDisablepdev(oldpdev)
 * we return  TRUE as we don't have much to do here
 * Plan to keep the devmode here or we could take from EMF
 */
BOOL  CALLBACK DrvResetPDEV(DHPDEV dhpdevOld, DHPDEV dhpdevNew)
{
     DUMPMSG("DrvResetPDEV");
     PPDEV pPDevNew = (PPDEV)dhpdevNew;
     PPDEV pPDevOld = (PPDEV)dhpdevOld;

     pPDevNew->dwJobId = pPDevOld->dwJobId;
     pPDevNew->Pages = pPDevOld->Pages;
     pPDevNew->binDocument = pPDevOld->binDocument;
     return TRUE;
}
//  DrvDisableDriver    doesn't   seemed    to  be   called  anytime  in  2k.
//  Still  we  implement  and  keep.
VOID CALLBACK DrvDisableDriver()
{
}

/***************Brush   Entries***********************************************/

BOOL CALLBACK DrvRealizeBrush(BRUSHOBJ *pbo,SURFOBJ  *psoTarget,SURFOBJ  *psoPattern,SURFOBJ  *psoMask,XLATEOBJ *pxlo,ULONG    iHatch)
{
     return TRUE;
}



/****************Vector   Enties**************************************************/
BOOL CALLBACK DrvStrokePath(SURFOBJ   *pso,PATHOBJ   *ppo,CLIPOBJ   *pco,XFORMOBJ  *pxo,BRUSHOBJ  *pbo,POINTL    *pptlBrushOrg,LINEATTRS *plineattrs,MIX        mix)
{

     DUMPMSG("StrokePath");
     return TRUE;
}


BOOL CALLBACK DrvFillPath(SURFOBJ  *pso,PATHOBJ  *ppo,CLIPOBJ  *pco,BRUSHOBJ *pbo,POINTL   *pptlBrushOrg,MIX       mix,FLONG     flOptions)
{

     DUMPMSG("DrvFillPath");
     return TRUE;
}


BOOL CALLBACK DrvStrokeAndFillPath(SURFOBJ   *pso,PATHOBJ   *ppo,CLIPOBJ   *pco,XFORMOBJ  *pxo,BRUSHOBJ  *pboStroke,LINEATTRS *plineattrs,BRUSHOBJ  *pboFill,POINTL    *pptlBrushOrg,MIX        mixFill,FLONG      flOptions)
{

     DUMPMSG("DrvStrokeAndFillPath");
     return TRUE;
}


BOOL CALLBACK DrvPaint(SURFOBJ  *pso,CLIPOBJ  *pco,BRUSHOBJ *pbo,POINTL   *pptlBrushOrg,MIX       mix)
{

     DUMPMSG("DrvPaint");
     return TRUE;
}

BOOL CALLBACK DrvLineTo(SURFOBJ   *pso,CLIPOBJ   *pco,BRUSHOBJ  *pbo,LONG       x1,LONG       y1,LONG       x2,LONG       y2,RECTL     *prclBounds,MIX        mix)
{
     DUMPMSG("LineTo");

     return TRUE;
}
/****************************Raster   Entries*************************************/

HBITMAP CALLBACK DrvCreateDeviceBitmap(
        DHPDEV  dhpdev,
        SIZEL  sizl,
        ULONG  iFormat
        )
{
     DUMPMSG("DrvCreateDeviceBitmap");
     return 0;

}

VOID CALLBACK DrvDeleteDeviceBitmap(
        DHSURF  dhsurf
        )
{
     DUMPMSG("DrvDeleteDeviceBitmap");
}
BOOL CALLBACK DrvBitBlt(SURFOBJ  *psoTrg,SURFOBJ  *psoSrc,SURFOBJ  *psoMask,CLIPOBJ  *pco,XLATEOBJ *pxlo,RECTL    *prclTrg,POINTL   *pptlSrc,POINTL   *pptlMask,BRUSHOBJ *pbo,POINTL   *pptlBrush,ROP4      rop4)
{
     DUMPMSG("DrvBitBlt");

     return TRUE;
}


BOOL CALLBACK DrvCopyBits(SURFOBJ  *psoDest,SURFOBJ  *psoSrc,CLIPOBJ  *pco,XLATEOBJ *pxlo,RECTL    *prclDest,POINTL   *pptlSrc)
{
     DUMPMSG("DrvCopyBits");

     return TRUE;
}



BOOL CALLBACK DrvStretchBlt(SURFOBJ * psoDest, SURFOBJ * psoSrc,
        SURFOBJ * psoMask, CLIPOBJ * pco,
        XLATEOBJ * pxlo, COLORADJUSTMENT * pca,
        POINTL * pptlHTOrg, RECTL * prclDest,
        RECTL * prclSrc, POINTL * pptlMask,
        ULONG iMode)
{
     DUMPMSG("DrvStretchBlt");

     return TRUE;

}

BOOL CALLBACK DrvStretchBltROP( SURFOBJ  *pTargetSurfObj, SURFOBJ   *pSourceSurfObj,
        SURFOBJ                 *pMaskSurfObj, CLIPOBJ   *pClipObj,
        XLATEOBJ                *pXlateObj, COLORADJUSTMENT    *pColorAdjustment,
        POINTL                  *pHalfToneBrushOriginPointl, RECTL     *pTargetRectl,
        RECTL                   *pSourceRectl, POINTL     *pMaskOffsetPointl,
        ULONG                   mode, BRUSHOBJ    *pBrushObj,
        ROP4                    rop4 //  some  places   this   also  referred  as  mix.
        )
{

     return TRUE;
}

/***********************TextOut  Entry***************************************/
BOOL CALLBACK
DrvTextOut(SURFOBJ  *pso,
        STROBJ   *pstro,
        FONTOBJ  *pfo,
        CLIPOBJ  *pco,
        RECTL    *prclExtra,
        RECTL    *prclOpaque,
        BRUSHOBJ *pboFore,
        BRUSHOBJ *pboOpaque,
        POINTL   *pptlOrg,
        MIX       mix)
{
     DUMPMSG("DrvTextOut");

     /*
     ULONG cChars = FONTOBJ_cGetAllGlyphHandles( pfo, NULL );
     HGLYPH *pGlyph = (HGLYPH *)malloc(sizeof(HGLYPH) * cChars);
     FONTOBJ_cGetAllGlyphHandles( pfo, pGlyph);
     WCHAR  str[256];
     wsprintf(str , L"glyph %d",cChars);
     OutputDebugString(str);
     //MOVE
     if(pstro->pwszOrg)
     {
          LPWSTR ptr = (LPWSTR)malloc(sizeof(WCHAR) * (pstro->cGlyphs + 1));
          ZEROMEMORY(ptr,sizeof(WCHAR) * (pstro->cGlyphs + 1));
          memcpy(ptr , pstro->pwszOrg , sizeof(WCHAR) * (pstro->cGlyphs));
          OutputDebugStringW(ptr);
          free(ptr);
     }
     */


     return TRUE;

}

/************************Escape    Entry*********************************/
#define PDEV_ESCAPE 0x303eb8efU  //this is for the job id.
ULONG CALLBACK
DrvEscape(SURFOBJ *pso,
        ULONG    iEsc,
        ULONG    cjIn,
        PVOID    pvIn,
        ULONG    cjOut,
        PVOID    pvOut)
{
     ULONG uRetVal = 0;
     DUMPMSG("DrvEscape");
     struct DEVDATAUI{
          DWORD dwJobId;
          int Pages;
          WCHAR *pSpoolFileName;
          HANDLE hPDriver;
          BOOL *pResetDC;
     };
     switch(iEsc)
     {
          /* while playing back on the spooler we don't get jobid */
     case PDEV_ESCAPE: 
         {
              PPDEV pPDEV = (DEVDATA *)pso->dhpdev;
              DEVDATAUI *pDevUI = (DEVDATAUI *)pvOut;
              pDevUI->dwJobId = pPDEV->dwJobId;
              pDevUI->Pages = pPDEV->Pages;
              return TRUE; 
         }
         break;
     }
     return uRetVal;
}


ULONG CALLBACK DrvDrawEscape(SURFOBJ *pso,ULONG    iEsc,CLIPOBJ *pco,RECTL   *prcl,ULONG    cjIn,PVOID    pvIn)
{
     return 0;
}



BOOL CALLBACK DrvStartDoc(SURFOBJ *pso, LPWSTR pwszDocName, DWORD dwJobId)
{
     DUMPMSG("DrvStartDoc");

     //REVISIT for 9999, we swap anyway in ResetPdev for ResetDc
     if(((DEVDATA *)pso->dhpdev)->dwJobId == 9999)
     {
          ((DEVDATA *)(pso->dhpdev))->Pages = 0;
          ((DEVDATA *)pso->dhpdev)->dwJobId = dwJobId;
          ((DEVDATA *)pso->dhpdev)->binDocument = TRUE;
     }
     //wsprintf(DbgString , L"JobId%d\r\n",dwJobId); 
     //OutputDebugStringW(DbgString);
     return TRUE;
}

BOOL  CALLBACK DrvEndDoc(SURFOBJ *pso, FLONG fl)
{
     DUMPMSG("DrvEndDoc");
     ((DEVDATA *)(pso->dhpdev))->dwJobId = 9999; 
     ((DEVDATA *)(pso->dhpdev))->Pages = 0; 
     return TRUE;

}

BOOL  CALLBACK DrvStartPage(SURFOBJ *pso)
{
     DUMPMSG("DrvStartPage");
     return TRUE;
}

//Doesn't get called when we have a banding surface, we will never be so.
BOOL  CALLBACK DrvSendPage(SURFOBJ *pso)
{
     DUMPMSG("DrvSendPage");
     ((DEVDATA *)(pso->dhpdev))->Pages++;
     return TRUE;
}


