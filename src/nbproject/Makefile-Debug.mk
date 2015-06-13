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
	${OBJECTDIR}/backend_factory.o \
	${OBJECTDIR}/backend_pg/pgbackendbase.o \
	${OBJECTDIR}/backend_pg/pgconnection.o \
	${OBJECTDIR}/backend_pg/pgquerybuilder.o \
	${OBJECTDIR}/backend_pg/pgresultset.o \
	${OBJECTDIR}/backend_sl/slbackendbase.o \
	${OBJECTDIR}/backend_sl/slconnection.o \
	${OBJECTDIR}/backend_sl/slquerybuilder.o \
	${OBJECTDIR}/backend_sl/slresultset.o \
	${OBJECTDIR}/commons.o \
	${OBJECTDIR}/compat.o \
	${OBJECTDIR}/dataset.o \
	${OBJECTDIR}/interval.o \
	${OBJECTDIR}/intervalevent.o \
	${OBJECTDIR}/keyvalues.o \
	${OBJECTDIR}/logger.o \
	${OBJECTDIR}/method.o \
	${OBJECTDIR}/process.o \
	${OBJECTDIR}/processcontrol.o \
	${OBJECTDIR}/processparams.o \
	${OBJECTDIR}/processstate.o \
	${OBJECTDIR}/query.o \
	${OBJECTDIR}/sequence.o \
	${OBJECTDIR}/timexer.o \
	${OBJECTDIR}/tkey.o \
	${OBJECTDIR}/vtapi.o \
	${OBJECTDIR}/vtapi_misc.o \
	${OBJECTDIR}/vtapi_settings.o


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
LDLIBSOPTIONS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lgeos_c -lprocps -llwgeom -lltdl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../${CND_DISTDIR}/libvtapi.so

../${CND_DISTDIR}/libvtapi.so: ${OBJECTFILES}
	${MKDIR} -p ../${CND_DISTDIR}
	${LINK.cc} -o ../${CND_DISTDIR}/libvtapi.so ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/backend_factory.o: backend_factory.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_factory.o backend_factory.cpp

${OBJECTDIR}/backend_pg/pgbackendbase.o: backend_pg/pgbackendbase.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_pg
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_pg/pgbackendbase.o backend_pg/pgbackendbase.cpp

${OBJECTDIR}/backend_pg/pgconnection.o: backend_pg/pgconnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_pg
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_pg/pgconnection.o backend_pg/pgconnection.cpp

${OBJECTDIR}/backend_pg/pgquerybuilder.o: backend_pg/pgquerybuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_pg
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_pg/pgquerybuilder.o backend_pg/pgquerybuilder.cpp

${OBJECTDIR}/backend_pg/pgresultset.o: backend_pg/pgresultset.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_pg
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_pg/pgresultset.o backend_pg/pgresultset.cpp

${OBJECTDIR}/backend_sl/slbackendbase.o: backend_sl/slbackendbase.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_sl
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_sl/slbackendbase.o backend_sl/slbackendbase.cpp

${OBJECTDIR}/backend_sl/slconnection.o: backend_sl/slconnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_sl
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_sl/slconnection.o backend_sl/slconnection.cpp

${OBJECTDIR}/backend_sl/slquerybuilder.o: backend_sl/slquerybuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_sl
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_sl/slquerybuilder.o backend_sl/slquerybuilder.cpp

${OBJECTDIR}/backend_sl/slresultset.o: backend_sl/slresultset.cpp 
	${MKDIR} -p ${OBJECTDIR}/backend_sl
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/backend_sl/slresultset.o backend_sl/slresultset.cpp

${OBJECTDIR}/commons.o: commons.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/commons.o commons.cpp

${OBJECTDIR}/compat.o: compat.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/compat.o compat.cpp

${OBJECTDIR}/dataset.o: dataset.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dataset.o dataset.cpp

${OBJECTDIR}/interval.o: interval.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/interval.o interval.cpp

${OBJECTDIR}/intervalevent.o: intervalevent.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/intervalevent.o intervalevent.cpp

${OBJECTDIR}/keyvalues.o: keyvalues.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/keyvalues.o keyvalues.cpp

${OBJECTDIR}/logger.o: logger.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/logger.o logger.cpp

${OBJECTDIR}/method.o: method.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/method.o method.cpp

${OBJECTDIR}/process.o: process.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/process.o process.cpp

${OBJECTDIR}/processcontrol.o: processcontrol.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/processcontrol.o processcontrol.cpp

${OBJECTDIR}/processparams.o: processparams.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/processparams.o processparams.cpp

${OBJECTDIR}/processstate.o: processstate.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/processstate.o processstate.cpp

${OBJECTDIR}/query.o: query.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/query.o query.cpp

${OBJECTDIR}/sequence.o: sequence.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sequence.o sequence.cpp

${OBJECTDIR}/timexer.o: timexer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/timexer.o timexer.cpp

${OBJECTDIR}/tkey.o: tkey.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tkey.o tkey.cpp

${OBJECTDIR}/vtapi.o: vtapi.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/vtapi.o vtapi.cpp

${OBJECTDIR}/vtapi_misc.o: vtapi_misc.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/vtapi_misc.o vtapi_misc.cpp

${OBJECTDIR}/vtapi_settings.o: vtapi_settings.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -I.. -I../include -I../include/postgresql -I/usr/include/postgresql -I/usr/local/include/opencv2 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/vtapi_settings.o vtapi_settings.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../${CND_DISTDIR}/libvtapi.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
