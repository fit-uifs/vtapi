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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=

# Macros
PLATFORM=GNU-Linux-x86

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Release/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/pgCubeBox3d.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/${PLATFORM}/libpgCubeBox3d.so

dist/Release/${PLATFORM}/libpgCubeBox3d.so: ${OBJECTFILES}
	${MKDIR} -p dist/Release/${PLATFORM}
	${LINK.c} -shared -o dist/Release/${PLATFORM}/libpgCubeBox3d.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/pgCubeBox3d.o: pgCubeBox3d.c
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I`pg_config --includedir-server` -I`pg_config --includedir` -Iinclude -fPIC  -o ${OBJECTDIR}/pgCubeBox3d.o pgCubeBox3d.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/${PLATFORM}/libpgCubeBox3d.so

# Subprojects
.clean-subprojects:
