

#   PrintMirror extracts individual page metafiles from Spool File.
#   Copyright (C) 2002-2004  Vipin Aravind
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#   
#   Contact at vprnt@lycos.com

#
# Microsoft Developer Studio Generated NMAKE File, Based on prntmrd.dsp
NTDDK_INCLUDE=d:\ntddk\inc
NTDDK_LIB=d:\ntddk\libfre\i386 
#CFG=prntmrd - Win32 Debug
!IF "$(CFG)" == ""
CFG=prntmrd - Win32 Release
!MESSAGE No configuration specified. Defaulting to prntmrd - Win32 Release.
!ENDIF 


!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "prntmrd - Win32 Release"
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\prntmrd.dll"


CLEAN :
	-@erase "$(INTDIR)\prntmrd.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prntmrd.dll"
	-@erase "$(OUTDIR)\prntmrd.exp"
	-@erase "$(OUTDIR)\prntmrd.lib"
	-@erase "$(OUTDIR)\prntmrd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe

CPP_PROJ=/nologo  /MD /O2 /W3  /GX   /I $(NTDDK_INCLUDE) /D WINNT=1 /D _WIN32_WINNT=0x500 /D "USERMODE_DRIVER" /D "WIN32"  /D "_WINDOWS" /D "_USRDLL"  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c  

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\prntmrd.res" /d "NDEBUG" 
	
LINK32=link.exe
LINK32_FLAGS= comctl32.lib umpdddi.lib  comdlg32.lib  user32.lib gdi32.lib kernel32.lib advapi32.lib winspool.lib /nologo /dll /incremental:no   /machine:I386  /EXPORT:DrvEnableDriver /EXPORT:DrvDisableDriver /EXPORT:DllMain /EXPORT:DrvQueryDriverInfo  /out:"$(OUTDIR)\prntmrd.dll" /implib:"$(OUTDIR)\prntmrd.lib" /libpath:$(NTDDK_LIB)

RC_FLAGS= -l 409 -z "MS Sans Serif,Helv/MS Shell Dlg"  -r  -D_X86_=1 -Di386=1  -DSTD_CALL -DCONDITION_HANDLING=1 -DNT_UP=1  -DNT_INST=0 -DWIN32=100 -D_NT1X_=100 -DWINNT=1 -D_WIN32_WINNT=0x0500 -DWINVER=0x0500 -D_WIN32_IE=0x0501    -DWIN32_LEAN_AND_MEAN=1  -DDEVL=1 -DFPO=0    -DNDEBUG -D_DLL=1 -D_MT=1  -DSTRICT -DUNICODE -DUSERMODE_DRIVER   
LINK32_OBJS= \
	"$(INTDIR)\prntmrd.obj" \
    "$(INTDIR)\prntmrd.res"

#"$(OUTDIR)\prntmrd.dll" : "$(OUTDIR)"  $(LINK32_OBJS)
#    $(LINK32) @<<
#  $(LINK32_FLAGS) $(LINK32_OBJS)
#<<

!ELSEIF "$(CFG)" == "prntmrd - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\prntmrd.dll"

CLEAN :
	-@erase "$(INTDIR)\prntmrd.obj"
	-@erase "$(INTDIR)\.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prntmrd.dll"
	-@erase "$(OUTDIR)\prntmrd.exp"
	-@erase "$(OUTDIR)\prntmrd.lib"
	-@erase "$(OUTDIR)\prntmrd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I $(NTDDK_INCLUDE) /D "USERMODE_DRIVER" /D "WIN32" /D "_DEBUG"  /D "_WINDOWS" /D "_USRDLL"  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /D WINNT=1 /D _WIN32_WINNT=0x500 
!ENDIF

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\prntmrd.res"  /d "_DEBUG"
	
LINK32=link.exe

LINK32_FLAGS= comctl32.lib umpdddi.lib  comdlg32.lib  user32.lib gdi32.lib kernel32.lib advapi32.lib winspool.lib /nologo /dll /incremental:no /debug  /pdb:prntmrd.pdb  /machine:I386  /EXPORT:DrvEnableDriver /EXPORT:DrvDisableDriver /EXPORT:DllMain /EXPORT:DrvQueryDriverInfo  /out:"$(OUTDIR)\prntmrd.dll" /implib:"$(OUTDIR)\prntmrd.lib" /libpath:$(NTDDK_LIB) 

RC_FLAGS= -l 409 -z "MS Sans Serif,Helv/MS Shell Dlg"  -r -fo "$(INTDIR)\prntmrd.res" -D_X86_=1 -Di386=1  -DSTD_CALL -DCONDITION_HANDLING=1 -DNT_UP=1  -DNT_INST=0 -DWIN32=100 -D_NT1X_=100 -DWINNT=1 -D_WIN32_WINNT=0x0500 -DWINVER=0x0500 -D_WIN32_IE=0x0501    -DWIN32_LEAN_AND_MEAN=1 -DDBG=1 -DDEVL=1 -DFPO=0   -D_DLL=1 -D_MT=1  -DSTRICT -DUNICODE -DUSERMODE_DRIVER   /I$(PLATFORM_SDK)
LINK32_OBJS= \
	"$(INTDIR)\prntmrd.obj" \
    "$(INTDIR)\prntmrd.res"

"$(OUTDIR)\prntmrd.dll" : "$(OUTDIR)"  $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!IF "$(CFG)" == "prntmrd - Win32 Release" || "$(CFG)" == "prntmrd - Win32 Debug"
SOURCE=prntmrd.cpp

"$(INTDIR)\prntmrd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\prntmrd.rc
"$(INTDIR)\prntmrd.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)





!ENDIF 


