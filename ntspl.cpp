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
void GetMetaFileFromSpoolFile(TCHAR *SpoolFileName , int PageNbr , TCHAR *MetaFileName, PPDEV pPDev,LPBYTE *pDevmode)
{
     HANDLE   hFile = CreateFile( SpoolFileName,
             GENERIC_READ,              // open for reading 
             FILE_SHARE_READ,           // share for reading 
             NULL,                      // no security 
             OPEN_EXISTING,             // existing file only 
             FILE_ATTRIBUTE_NORMAL,     // normal file 
             NULL);                     // no attr. template 
     HANDLE hMapFile = CreateFileMapping(
             hFile,                       // handle to file
             NULL, // security
             PAGE_READONLY,                    // protection
             0,            // high-order DWORD of size
             0,             // low-order DWORD of size
             NULL                      // object name
             );
     LPBYTE pMapFile = (LPBYTE)MapViewOfFileEx(
             hMapFile,   // handle to file-mapping object
             FILE_MAP_READ,       // access mode
             0,      // high-order DWORD of offset
             0,       // low-order DWORD of offset
             0, // number of bytes to map
             NULL         // starting address
             );
     DWORD granularity = *((DWORD *)pMapFile);
     pMapFile +=  sizeof(DWORD);
     DWORD splheader = *((DWORD *)pMapFile);
     pMapFile +=  splheader;
     DWORD metafilelen = *((DWORD *)pMapFile);
     pMapFile +=  sizeof(DWORD); // This is hack after comparison with win9x.


     for(int Nbr = 1 ; Nbr < PageNbr ; Nbr++)
     {
          pMapFile += metafilelen;
          if(pPDev->pResetDC[Nbr - 1] == FALSE)
          {
               pMapFile += 20; 
          }
          else
          {
               /* skip the reset devmode here */
               pMapFile += 4; // This marker is the same as the one after Devmode
               DWORD offset = *((DWORD *)pMapFile); //this is multiple of 4bytes.
               pMapFile += offset + 4; // devmode + devmode-length
               pMapFile += 16 + 4; //Regular 20 bytes seperator(marker,...,metalen-tillhere,0000
               //,startpagemarker)
          }
          metafilelen = *((DWORD *)pMapFile);
          pMapFile += 4 ; //This has the metafile length!!!
          //keep incrementing till we are on the last page.
     }

     HANDLE   hMetaFile = CreateFile( MetaFileName,
             GENERIC_READ | GENERIC_WRITE,              // open for reading 
             FILE_SHARE_READ,           // share for reading 
             NULL,                      // no security 
             CREATE_ALWAYS,             // existing file only 
             FILE_ATTRIBUTE_NORMAL,     // normal file 
             NULL);                     // no attr. template 

     DWORD numWritten;
     WriteFile(
             hMetaFile,                                       // handle to output file
             pMapFile,                                   // data buffer
             metafilelen,                        // number of bytes to write
             &numWritten,
             NULL                          // overlapped buffer
             );

     if(pDevmode)
     {
          LPBYTE ptr = pMapFile + metafilelen;
          if(pPDev->pResetDC[PageNbr - 1] == TRUE)
          {
               ptr += 4; // This marker is the same as the one after Devmode
               DWORD offset = *((DWORD *)ptr); //this is multiple of 4bytes.
               *pDevmode = (LPBYTE)MALLOC(offset); 
               memcpy(*pDevmode , ptr + 4 , offset);
          }
     }
     CloseHandle(hMetaFile);

     UnmapViewOfFile( pMapFile   // starting address
             );
     CloseHandle(hMapFile);
     CloseHandle(hFile);
}

/*
 *   retrieve current job's spool file
 */

void GetSpoolFileName(DWORD JobId, TCHAR SpoolFileName[],HANDLE hDriver)
{

     DWORD cbNeeded;   
     DWORD dwType = REG_SZ;      // data type
     GetPrinterData(
             hDriver,    // handle to printer or print server
             SPLREG_DEFAULT_SPOOL_DIRECTORY,
             &dwType,      // data type
             NULL,       // configuration data buffer
             0,        // size of configuration data buffer
             &cbNeeded   // bytes received or required 
             );
     LPBYTE  pSpoolDirectory = (LPBYTE)MALLOC(cbNeeded); 
     GetPrinterData(
             hDriver,    // handle to printer or print server
             SPLREG_DEFAULT_SPOOL_DIRECTORY,
             &dwType,      // data type
             pSpoolDirectory,       // configuration data buffer
             cbNeeded,        // size of configuration data buffer
             &cbNeeded   // bytes received or required 
             );
     TCHAR TempSpoolFileName[MAX_PATH];
     {
          wsprintf(TempSpoolFileName , L"%s\\",pSpoolDirectory); 
          TCHAR JobIdName[256];
          wsprintf(JobIdName , L"%d",JobId);
          int zeros = 5 - wcslen(JobIdName);
          for(; zeros > 0; zeros--)
          {
               wsprintf(SpoolFileName , L"%s0",TempSpoolFileName); 
               wcscpy(TempSpoolFileName,SpoolFileName);
          }

          wsprintf(SpoolFileName , L"%s%d.spl",TempSpoolFileName , JobId); 
          WIN32_FIND_DATA FindFileData;
          FindFirstFile(
                  SpoolFileName,               // file name
                  &FindFileData  // data buffer
                  );
          wcscpy(SpoolFileName , (TCHAR *)pSpoolDirectory);
          wcscat(SpoolFileName , L"\\");
          wcscat(SpoolFileName , FindFileData.cFileName);
     }
     //RegCloseKey(hSpoolDirectory);
     free(pSpoolDirectory);
}

