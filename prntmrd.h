
/*
   PrintMirror extracts individual page metafiles from Spool File.
   Copyright (C) 2002  V Aravind

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

enum Layout { UP1 ,UP2,UP4,UP6,UP9,BOOKLET};
struct VPDEVMODE{
     WCHAR PrinterName[256];
     Layout lyt;
     WORD PaperSize;
     GDIINFO  gi;
     int numcolors;
     ULONG Palette[256];
     LOGFONT lf;
	 BOOL Preview;
	 BOOL PrintToPaper;
         BYTE Complevel;
};
struct VDEVMODE{
     DEVMODEW dm;
     VPDEVMODE pdm;
};
