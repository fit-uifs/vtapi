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
FC=

AS=as


# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/sequence.o \
	${OBJECTDIR}/commons.o \
	${OBJECTDIR}/vtapi_settings.o \
	${OBJECTDIR}/interval.o \
	${OBJECTDIR}/method.o \
	${OBJECTDIR}/video.o \
	${OBJECTDIR}/vtapi.o \
	${OBJECTDIR}/vtapi_libpq.o \
	${OBJECTDIR}/keyvalues.o \
	${OBJECTDIR}/process.o \
	${OBJECTDIR}/typemap.o \
	${OBJECTDIR}/query.o \
	${OBJECTDIR}/dataset.o


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
LDLIBSOPTIONS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lpq -lpqtypes -lgeos -lgeos_c ../dist/liblwgeom.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ../dist/libvtapi.so

../dist/libvtapi.so: ../dist/liblwgeom.so

../dist/libvtapi.so: ${OBJECTFILES}
	${MKDIR} -p ../dist
	${LINK.cc} -shared -o ../${CND_DISTDIR}/libvtapi.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/sequence.o: sequence.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/sequence.o sequence.cpp

${OBJECTDIR}/commons.o: commons.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/commons.o commons.cpp

${OBJECTDIR}/vtapi_settings.o: vtapi_settings.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/vtapi_settings.o vtapi_settings.cpp

${OBJECTDIR}/interval.o: interval.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/interval.o interval.cpp

${OBJECTDIR}/method.o: method.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/method.o method.cpp

${OBJECTDIR}/video.o: video.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/video.o video.cpp

${OBJECTDIR}/vtapi.o: vtapi.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/vtapi.o vtapi.cpp

${OBJECTDIR}/vtapi_libpq.o: vtapi_libpq.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/vtapi_libpq.o vtapi_libpq.c

${OBJECTDIR}/keyvalues.o: keyvalues.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/keyvalues.o keyvalues.cpp

${OBJECTDIR}/process.o: process.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/process.o process.cpp

${OBJECTDIR}/typemap.o: typemap.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/typemap.o typemap.cpp

${OBJECTDIR}/query.o: query.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/query.o query.cpp

${OBJECTDIR}/dataset.o: dataset.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -I../include -I../include/postgresql -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/dataset.o dataset.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../dist/libvtapi.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
