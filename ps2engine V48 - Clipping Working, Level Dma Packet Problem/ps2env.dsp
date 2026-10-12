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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
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
# Begin Group "Audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\audio.cpp
# End Source File
# End Group
# Begin Group "DMA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\addregisters.cpp
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
# End Group
# Begin Group "AI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\acquaintance.cpp
# End Source File
# Begin Source File

SOURCE=.\action.cpp
# End Source File
# Begin Source File

SOURCE=.\actionlist.cpp
# End Source File
# Begin Source File

SOURCE=.\ai.cpp
# End Source File
# Begin Source File

SOURCE=.\collision.cpp
# End Source File
# Begin Source File

SOURCE=.\event.cpp
# End Source File
# Begin Source File

SOURCE=.\gossip.cpp
# End Source File
# Begin Source File

SOURCE=.\gossipengine.cpp
# End Source File
# Begin Source File

SOURCE=.\timeline.cpp
# End Source File
# Begin Source File

SOURCE=.\timelineday.cpp
# End Source File
# Begin Source File

SOURCE=.\timelineevent.cpp
# End Source File
# End Group
# Begin Group "Graphics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\customimage.cpp
# End Source File
# Begin Source File

SOURCE=.\custommodel.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\keyframe.cpp
# End Source File
# Begin Source File

SOURCE=.\level.cpp
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

SOURCE=.\q3bspfile.cpp
# End Source File
# Begin Source File

SOURCE=.\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\vumgr.cpp
# End Source File
# End Group
# Begin Group "General"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\area.cpp
# End Source File
# Begin Source File

SOURCE=.\demon.cpp
# End Source File
# Begin Source File

SOURCE=.\gameclock.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.cpp
# End Source File
# Begin Source File

SOURCE=.\perftime.c
# End Source File
# Begin Source File

SOURCE=.\timestamp.cpp
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\joypadlib.c
# End Source File
# Begin Source File

SOURCE=.\PS2Pad.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.cpp
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

SOURCE=.\output.txt
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
# Begin Source File

SOURCE=.\vucode.txt
# End Source File
# End Group
# Begin Group "outside includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\outsideheader\sps2interface.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2lib.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2registers.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2regstructs.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2scratchpad.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2tags.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2types.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2util.h
# End Source File
# Begin Source File

SOURCE=.\outsideheader\sps2vumemory.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Graphics Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\customimage.h
# End Source File
# Begin Source File

SOURCE=.\customlvlfile.h
# End Source File
# Begin Source File

SOURCE=.\custommodel.h
# End Source File
# Begin Source File

SOURCE=.\effectgenerator.h
# End Source File
# Begin Source File

SOURCE=.\effectobject.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\gamehud.h
# End Source File
# Begin Source File

SOURCE=.\gamemenu.h
# End Source File
# Begin Source File

SOURCE=.\graphics.h
# End Source File
# Begin Source File

SOURCE=.\keyframe.h
# End Source File
# Begin Source File

SOURCE=.\level.h
# End Source File
# Begin Source File

SOURCE=.\lwofile.h
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

SOURCE=.\md3file.h
# End Source File
# Begin Source File

SOURCE=.\model.h
# End Source File
# Begin Source File

SOURCE=.\modelloader.h
# End Source File
# Begin Source File

SOURCE=.\ms3dfile.h
# End Source File
# Begin Source File

SOURCE=.\q3bspfile.h
# End Source File
# Begin Source File

SOURCE=.\skeletal.h
# End Source File
# Begin Source File

SOURCE=.\sps2inc.h
# End Source File
# Begin Source File

SOURCE=.\staticmodel.h
# End Source File
# Begin Source File

SOURCE=.\talkwindow.h
# End Source File
# Begin Source File

SOURCE=.\texture.h
# End Source File
# Begin Source File

SOURCE=.\vumgr.h
# End Source File
# End Group
# Begin Group "DMA Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\addregisters.h
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
# End Group
# Begin Group "Audio Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\audio.h
# End Source File
# Begin Source File

SOURCE=.\oggfile.h
# End Source File
# Begin Source File

SOURCE=.\soundeffect.h
# End Source File
# Begin Source File

SOURCE=.\soundmusic.h
# End Source File
# Begin Source File

SOURCE=.\wavloader.h
# End Source File
# End Group
# Begin Group "AI Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\acquaintance.h
# End Source File
# Begin Source File

SOURCE=.\action.h
# End Source File
# Begin Source File

SOURCE=.\actionlist.h
# End Source File
# Begin Source File

SOURCE=.\ai.h
# End Source File
# Begin Source File

SOURCE=.\collision.h
# End Source File
# Begin Source File

SOURCE=.\collisionengine.h
# End Source File
# Begin Source File

SOURCE=.\event.h
# End Source File
# Begin Source File

SOURCE=.\gossip.h
# End Source File
# Begin Source File

SOURCE=.\gossipengine.h
# End Source File
# Begin Source File

SOURCE=.\npcaction.h
# End Source File
# Begin Source File

SOURCE=.\path.h
# End Source File
# Begin Source File

SOURCE=.\pathgenerator.h
# End Source File
# Begin Source File

SOURCE=.\physics.h
# End Source File
# Begin Source File

SOURCE=.\playeraction.h
# End Source File
# Begin Source File

SOURCE=.\quest.h
# End Source File
# Begin Source File

SOURCE=.\sentence.h
# End Source File
# Begin Source File

SOURCE=.\sentencelist.h
# End Source File
# Begin Source File

SOURCE=.\timeline.h
# End Source File
# Begin Source File

SOURCE=.\timelineday.h
# End Source File
# Begin Source File

SOURCE=.\timelineevent.h
# End Source File
# End Group
# Begin Group "General Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\area.h
# End Source File
# Begin Source File

SOURCE=.\demon.h
# End Source File
# Begin Source File

SOURCE=.\entrance.h
# End Source File
# Begin Source File

SOURCE=.\gameclock.h
# End Source File
# Begin Source File

SOURCE=.\item.h
# End Source File
# Begin Source File

SOURCE=.\npc.h
# End Source File
# Begin Source File

SOURCE=.\perftime.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=.\PS2Defines.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\timestamp.h
# End Source File
# End Group
# Begin Group "Input Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\joypadlib.h
# End Source File
# Begin Source File

SOURCE=.\PS2Pad.h
# End Source File
# End Group
# End Group
# End Target
# End Project
