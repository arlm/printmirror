

#   PrintMirror extracts individual page metafiles from Spool File.
#   Copyright (C) 2002  V Aravind
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

PLATFORM_SDK = "d:\prntmr\inc"
NTDDK_INCLUDE = d:\ntddk\inc

# Microsoft Developer Studio Generated NMAKE File, Based on prntmrui.dsp
#CFG=prntmrui - Win32 Debug
!IF "$(CFG)" == ""
CFG=prntmrui - Win32 Release
!MESSAGE No configuration specified. Defaulting to prntmrui - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "prntmrui - Win32 Release" && "$(CFG)" != "prntmrui - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prntmrui.mak" CFG="prntmrui - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prntmrui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "prntmrui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "prntmrui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\prntmrui.dll"


CLEAN :
	-@erase "$(INTDIR)\prntmrui.obj"
	-@erase "$(INTDIR)\prntmrui.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\prntmrui.dll"
	-@erase "$(OUTDIR)\prntmrui.exp"
	-@erase "$(OUTDIR)\prntmrui.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ= /nologo /Gz /MD /W3 /GX /Od  /D "WIN32"   /I $(NTDDK_INCLUDE) /I $(PLATFORM_SDK) /D "_WINDOWS" /Fp"$(INTDIR)\prntmrui.pch"  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\prntmrui.res"
	
LINK32=link.exe
LINK32_FLAGS= /LIBPATH:d:\ntddk\libfre vfw32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /dll /incremental:no /machine:I386 /def:".\prntmrui.DEF" /out:"$(OUTDIR)\prntmrui.dll" /implib:"$(OUTDIR)\prntmrui.lib" 
DEF_FILE= \
	".\prntmrui.DEF"
LINK32_OBJS= \
	"$(INTDIR)\dllentry.obj" \
	"$(INTDIR)\preview.obj" \
	"$(INTDIR)\prntmrui.obj" \
	"$(INTDIR)\convdevm.obj" \
	"$(INTDIR)\devcap.obj" \
	"$(INTDIR)\docevent.obj" \
	"$(INTDIR)\helper.obj" \
	"$(INTDIR)\ntspl.obj" \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\paper.obj" \
	"$(INTDIR)\prntmrui.res"
"$(OUTDIR)\prntmrui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prntmrui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\prntmrui.dll"


CLEAN :
	-@erase "$(INTDIR)\prntmrui.obj"
	-@erase "$(INTDIR)\prntmrui.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prntmrui.dll"
	-@erase "$(OUTDIR)\prntmrui.exp"
	-@erase "$(OUTDIR)\prntmrui.ilk"
	-@erase "$(OUTDIR)\prntmrui.lib"
	-@erase "$(OUTDIR)\prntmrui.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /I $(NTDDK_INCLUDE)  /I $(PLATFORM_SDK) /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\prntmrui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\prntmrui.res" /d "_DEBUG" 
	
LINK32=link.exe
LINK32_FLAGS=/LIBPATH:d:\ntddk\libfre comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib  /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\prntmrui.pdb" /debug /machine:I386 /def:".\prntmrui.DEF" /out:"$(OUTDIR)\prntmrui.dll" /implib:"$(OUTDIR)\prntmrui.lib" 
DEF_FILE= \
	".\prntmrui.DEF"
LINK32_OBJS= \
	"$(INTDIR)\dllentry.obj" \
	"$(INTDIR)\preview.obj" \
	"$(INTDIR)\prntmrui.obj" \
	"$(INTDIR)\convdevm.obj" \
	"$(INTDIR)\devcap.obj" \
	"$(INTDIR)\docevent.obj" \
	"$(INTDIR)\helper.obj" \
	"$(INTDIR)\ntspl.obj" \
	"$(INTDIR)\prntmrui.res"

"$(OUTDIR)\prntmrui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 



!IF "$(CFG)" == "prntmrui - Win32 Release" || "$(CFG)" == "prntmrui - Win32 Debug"
SOURCE=\
    preview.cpp \
    dllentry.cpp \
    prntmrui.cpp \
    convdevm.cpp \
    devcap.cpp \
    docevent.cpp \
    helper.cpp \
    ntspl.cpp 


"$(INTDIR)\*.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\prntmrui.rc

"$(INTDIR)\prntmrui.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)

!ENDIF 

