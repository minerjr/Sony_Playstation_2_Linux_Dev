# Microsoft Developer Studio Project File - Name="ps2env" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ps2env - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ps2env.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ps2env.mak" CFG="ps2env - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ps2env - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ps2env - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ps2env - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ps2env - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ps2env - Win32 Release"
# Name "ps2env - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc"
# Begin Source File

SOURCE=.\addregisters.cpp
# End Source File
# Begin Source File

SOURCE=.\ai.cpp
# End Source File
# Begin Source File

SOURCE=.\audio.cpp
# End Source File
# Begin Source File

SOURCE=.\custommodel.cpp
# End Source File
# Begin Source File

SOURCE=.\dma4kpage.cpp
# End Source File
# Begin Source File

SOURCE=.\dmamemory.cpp
# End Source File
# Begin Source File

SOURCE=.\dmapacket.cpp
# End Source File
# Begin Source File

SOURCE=.\dmascheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\joypadlib.c
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\matrixfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\md2file.cpp
# End Source File
# Begin Source File

SOURCE=.\modelloader.cpp
# End Source File
# Begin Source File

SOURCE=.\ms3dfile.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\vumgr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\addregisters.h
# End Source File
# Begin Source File

SOURCE=.\ai.h
# End Source File
# Begin Source File

SOURCE=.\audio.h
# End Source File
# Begin Source File

SOURCE=.\custommodel.h
# End Source File
# Begin Source File

SOURCE=.\dma4kpage.h
# End Source File
# Begin Source File

SOURCE=.\dmamemory.h
# End Source File
# Begin Source File

SOURCE=.\dmapacket.h
# End Source File
# Begin Source File

SOURCE=.\dmascheduler.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\graphics.h
# End Source File
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\joypadlib.h
# End Source File
# Begin Source File

SOURCE=.\matrix.h
# End Source File
# Begin Source File

SOURCE=.\matrixfunc.h
# End Source File
# Begin Source File

SOURCE=.\md2file.h
# End Source File
# Begin Source File

SOURCE=.\modelloader.h
# End Source File
# Begin Source File

SOURCE=.\ms3dfile.h
# End Source File
# Begin Source File

SOURCE=.\PS2Defines.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\sps2globals.h
# End Source File
# Begin Source File

SOURCE=.\sps2inc.h
# End Source File
# Begin Source File

SOURCE=.\sps2interface.h
# End Source File
# Begin Source File

SOURCE=.\sps2lib.h
# End Source File
# Begin Source File

SOURCE=.\sps2registers.h
# End Source File
# Begin Source File

SOURCE=.\sps2regstructs.h
# End Source File
# Begin Source File

SOURCE=.\sps2scratchpad.h
# End Source File
# Begin Source File

SOURCE=.\sps2tags.h
# End Source File
# Begin Source File

SOURCE=.\sps2types.h
# End Source File
# Begin Source File

SOURCE=.\sps2util.h
# End Source File
# Begin Source File

SOURCE=.\sps2vumemory.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\vumgr.h
# End Source File
# End Group
# Begin Group "Makefile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Makefile
# End Source File
# End Group
# Begin Group "VU Code"

# PROP Default_Filter "*.vsm"
# Begin Source File

SOURCE=.\trans.vsm
# End Source File
# End Group
# Begin Group "Debug Txt"

# PROP Default_Filter "*.txt"
# Begin Source File

SOURCE=.\Camera.txt
# End Source File
# Begin Source File

SOURCE=.\Color.txt
# End Source File
# Begin Source File

SOURCE=.\DmaSche.txt
# End Source File
# Begin Source File

SOURCE=.\Light.txt
# End Source File
# Begin Source File

SOURCE=.\Projection.txt
# End Source File
# Begin Source File

SOURCE=.\TestModel.txt
# End Source File
# Begin Source File

SOURCE=.\TestVuModel.txt
# End Source File
# Begin Source File

SOURCE=.\View.txt
# End Source File
# End Group
# End Target
# End Project
