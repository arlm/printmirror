
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
   
   A copy of GNU GPL license is in "GNU GPL.txt".

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   
   Contact at vprnt@lycos.com
 */


It is mandatory that you pack the README file when you redistribute the utility.

How to build?

1)Install platform sdk and windows 2000 DDK
2)set PLATFORM_SDK and NTDDK_INCLUDE variables in the prntmrd.mak and prntmrui.mak
3) run nmake -f prntmrui.mak and prntmrd.mak at the command line.
4) prntmrd.dll and prntmrui.dll will created in the .\Release or .\debug directory

How to Install?

1) Make sure there is atleast one driver already installed on the system. NOTE: Results 
are unexpected if you don't meet this precondition.
2) Use AddPrinter from Printers folder to install the driver using the prntmr.inf an printmirror
   will be installed with the default printer mimicking after install. You may choose to attach
   printmirror to any other printer on the system using the "printing preferences".


How to use?

1) You can choose the Base printer from Printer: list in the Properties sheet and
  a papersize for which the rendering should happen.

  PrintMiror will mimmick the capabilities of the printer you choose in the Printer: and will 
  generate a spool file which it will ultimately use to preview, print redirection and saving to
  image formats (EMF and 24bpp BMP).

PrintMirror:

When you print from a windows application to PrintMirror, it
shows a dialog which contains the buttons:

1) Extract, for extracting the page as bmp or emf file
2) Previous, for traversing the pages in the print(useful for previewing)
3) Next, for going to the next page.
4) Print, for printing to a real Printer.
5) Preview, displays the preview of the current active page of the printed document.
6) License, indicates its a GNU GPL program

Version 1.2:

1)Defect Crash Fix:- When PrintMirror was mimicking PrinterA on a system with 2 printers, PrinterA and PrinterB and PrinterA is deleted, PrintMirror needs to pick up PrinterB to mimick in its future use.


Version 1.1:

1)You can save printing to .bmp file(useful for sending someone an image)
2)can print to any printer on the system.

Version 1.0:
It can extract metafiles from the spool file.

TODO:

Based on the usage of the tool, I had received the following requests from people:

1) Relaying the bitmap on to a webserver, useful for logging.
2) PrintMirror should stand independently, an option of being standalone( in non-mimick mode)


How to reach the author?
You can report bugs to author at vprnt@lycos.com.You can contribute to enhancement of PrintMirror
as per GNU GPL. You can contribute by documenting the current sources.Feel free to reach me for any suggestions.Also please give me a note when you found the tool useful and how it has helped you.
  
