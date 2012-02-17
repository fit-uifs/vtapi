#
#    Makefile for x86-64 @ FIT
#

# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=x86_64-linux-gcc
CCC=x86_64-linux-g++
CXX=x86_64-linux-g++
FC=x86_64-linux-gfortran

# Include project Makefile
# include Makefile64.mak

# Object Directory
OBJECTDIR=build/

# Object Files
OBJECTFILES=${OBJECTDIR}/pgCubeBox3d.o

# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build: ${BUILD_SUBPROJECTS} libpgCubeBox3d.so

libpgCubeBox3d.so: ${OBJECTFILES}
#	${MKDIR} -p plugins
	x86_64-linux-gcc -shared -o libpgCubeBox3d.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/pgCubeBox3d.o: pgCubeBox3d.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -c -fPIC -O3 -I`pg_config --includedir-server` -I`pg_config --includedir` -Iinclude -o ${OBJECTDIR}/pgCubeBox3d.o pgCubeBox3d.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean:
	${RM} -r build/
	${RM} libpgCubeBox3d.so

# Subprojects
.clean-subprojects:


