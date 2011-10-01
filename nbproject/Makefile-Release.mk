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
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1185971108/vtapi.o \
	${OBJECTDIR}/_ext/1185971108/commons.o \
	${OBJECTDIR}/_ext/1185971108/dataset.o \
	${OBJECTDIR}/_ext/1185971108/sequence.o \
	${OBJECTDIR}/_ext/1185971108/keyvalues.o \
	${OBJECTDIR}/test.o \
	${OBJECTDIR}/_ext/1185971108/interval.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/vtapi

dist/Release/GNU-Linux-x86/vtapi: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vtapi ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1185971108/vtapi.o: /home/chmelarp/Projects/VidTeAPI/vtapi/vtapi.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1185971108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1185971108/vtapi.o /home/chmelarp/Projects/VidTeAPI/vtapi/vtapi.cpp

${OBJECTDIR}/_ext/1185971108/commons.o: /home/chmelarp/Projects/VidTeAPI/vtapi/commons.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1185971108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1185971108/commons.o /home/chmelarp/Projects/VidTeAPI/vtapi/commons.cpp

${OBJECTDIR}/_ext/1185971108/dataset.o: /home/chmelarp/Projects/VidTeAPI/vtapi/dataset.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1185971108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1185971108/dataset.o /home/chmelarp/Projects/VidTeAPI/vtapi/dataset.cpp

${OBJECTDIR}/_ext/1185971108/sequence.o: /home/chmelarp/Projects/VidTeAPI/vtapi/sequence.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1185971108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1185971108/sequence.o /home/chmelarp/Projects/VidTeAPI/vtapi/sequence.cpp

${OBJECTDIR}/_ext/1185971108/keyvalues.o: /home/chmelarp/Projects/VidTeAPI/vtapi/keyvalues.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1185971108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1185971108/keyvalues.o /home/chmelarp/Projects/VidTeAPI/vtapi/keyvalues.cpp

${OBJECTDIR}/test.o: test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/test.o test.cpp

${OBJECTDIR}/_ext/1185971108/interval.o: /home/chmelarp/Projects/VidTeAPI/vtapi/interval.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1185971108
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1185971108/interval.o /home/chmelarp/Projects/VidTeAPI/vtapi/interval.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/vtapi

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
