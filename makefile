
##################################################################################################

!ifndef version
version = Release
!endif

##################################################################################################

curdir = $+ $(%cdrive):$(%cwd) $-
cppdir = $(curdir)\cpp
!ifneq version Win32_Debug
objdir = $(curdir)\build\$(targetcpu)_$(version)
!else
objdir = $(curdir)\build\$(version)
!endif

##################################################################################################

.BEFORE
	@if NOT EXIST $(objdir) @md $(objdir) >nul

##################################################################################################

make : .SYMBOLIC
	@call wmake -h -f mkmodules cppdir=$(cppdir)\ objdir=$(objdir)\
!ifneq version Win32_Debug
	@rem call wmake -d -h -f makedep targetcpu=$(targetcpu) version=$(version) cppdir=$(cppdir) objdir=$(objdir)
!endif
	@call wmake $(__MAKEOPTS__) -f makeobj targetcpu=$(targetcpu) version=$(version) cppdir=$(cppdir) objdir=$(objdir)


##################################################################################################

