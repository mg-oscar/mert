#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/mert.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs gtk+-3.0` `pkg-config --libs glib-2.0` -pthread -export-dynamic `pkg-config --libs libxml++-2.6` `pkg-config --libs sqlite3` `pkg-config --libs libusb-1.0` `pkg-config --libs libpcsclite`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk /home/user/mert/bin/mert

/home/user/mert/bin/mert: ${OBJECTFILES}
	${MKDIR} -p /home/user/mert/bin
	g++ -o /home/user/mert/bin/mert ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/mert.o: mert.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags gtk+-3.0` `pkg-config --cflags glib-2.0` `pkg-config --cflags libxml++-2.6` `pkg-config --cflags sqlite3` `pkg-config --cflags libusb-1.0` `pkg-config --cflags libpcsclite` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mert.o mert.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
