############################################################## -*- Makefile -*-
#
# Makefile (Visual C++ 6.0)
#
#	make release version: nmake nodebug=1
#	make debug version: nmake
#
###############################################################################


# VC++ rules	###############################################################

APPVER		= 4.0
TARGETOS	= WINNT
!include <win32.mak>

!ifdef nodebug
DEBUG_FLAG	= -DNDEBUG
!else
DEBUG_FLAG	=
!endif

.cpp.obj:
	$(cc) -GX $(cdebug) $(cflags) $(cvars) $(DEFINES) $(INCLUDES) \
		$(DEBUG_FLAG) -Fo$@ $*.cpp
.rc.res:
	$(rc) $(rcflags) $(rcvars) $*.rc


# tools		###############################################################

RM		= del
COPY		= copy
ECHO		= echo


# pngav.exe	###############################################################

TARGET_1	= pngav.exe
OBJS_1		= spi.obj registry.obj main.obj 
RES_1		= pngav.res
LIBS_1		= $(guilibs) shell32.lib libcp.lib shlwapi.lib \
		../libpng/msvc/win32/libpng/lib/libpng.lib \
		../libpng/msvc/win32/zlib/lib/zlib.lib

LDFLAGS_1	= $(guilflags)

DEFINES		= -DSTRICT \
		-DWINVER=0x0500 -D_WIN32_WINNT=0x0500 -D_WIN32_IE=0x0400
INCLUDES	= -I../libpng -I../zlib


$(TARGET_1):	$(OBJS_1) $(RES_1)
	$(link) -out:$@ $(ldebug) $(LDFLAGS_1) $(OBJS_1) $(LIBS_1) $(RES_1)


# other		###############################################################

clean:
	-$(RM) *.obj
	-$(RM) $(TARGET_1)
	-$(RM) *.res *.aps *.opt *.pdb
	-$(RM) *~ $(CLEAN)

distrib:
	unlha a source.lzh	\
		*.h		\
		*.cpp		\
		*.rc		\
		*.ico		\
		Makefile
	unlha a pngav1xx.lzh	\
		pngav.exe	\
		README.txt	\
		test.png	\
		source.lzh
	-$(RM) source.lzh
