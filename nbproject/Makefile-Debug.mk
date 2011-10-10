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
	${OBJECTDIR}/interval.o \
	${OBJECTDIR}/vtapi.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/cli_settings.o \
	${OBJECTDIR}/keyvalues.o \
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
LDLIBSOPTIONS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lpq -lpqtypes

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/vtapi

dist/Debug/GNU-Linux-x86/vtapi: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vtapi ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/sequence.o: sequence.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/sequence.o sequence.cpp

${OBJECTDIR}/commons.o: commons.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/commons.o commons.cpp

${OBJECTDIR}/interval.o: interval.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/interval.o interval.cpp

${OBJECTDIR}/vtapi.o: vtapi.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/vtapi.o vtapi.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/cli_settings.o: cli_settings.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/cli_settings.o cli_settings.cpp

${OBJECTDIR}/keyvalues.o: keyvalues.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/keyvalues.o keyvalues.cpp

${OBJECTDIR}/dataset.o: dataset.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/dataset.o dataset.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/vtapi

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
