
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
   
   A copy of GNU GPL license is in "GNU GPL.txt".

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   
   Contact at vprnt@lycos.com
 */



How to build?
1)Install platform sdk and windows 2000 DDK
2)set PLATFORM_SDK and NTDDK_INCLUDE variables in the prntmrd.mak and prntmrui.mak
3) run nmake -f prntmrui.mak and prntmrd.mak at the command line.
4) prntmrd.dll and prntmrui.dll will created in the .\Release or .|debug directory

How to Install?
1) Make sure there is atleast one driver already installed on the system.
2) Use AddPrinter from Printers folder to install the driver using the prntmr.inf.


How to use?
1) You can choose the Base printer from Printer: list in the Properties sheet and
  a papersize for which the rendering should happen.

  PrintMiror will mimmick the capabilities of the printer you choose in the Printer: and will 
  generate a spool file.




How to reach the author?
You can report bugs to author at vprnt@lycos.com.You can contribute to enhancement of PrintMirror
as per GNU GPL. You can contribute by documenting the current sources.Feel free to reach the me for any suggestions.Also please give me a note when you found the tool useful and how it has helped you.
  
