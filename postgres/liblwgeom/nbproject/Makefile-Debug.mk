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
	${OBJECTDIR}/lwsegmentize.o \
	${OBJECTDIR}/lwline.o \
	${OBJECTDIR}/g_box.o \
	${OBJECTDIR}/g_coord.o \
	${OBJECTDIR}/lwcollection.o \
	${OBJECTDIR}/box2d.o \
	${OBJECTDIR}/g_serialized.o \
	${OBJECTDIR}/measures.o \
	${OBJECTDIR}/lwmsurface.o \
	${OBJECTDIR}/vsprintf.o \
	${OBJECTDIR}/ptarray.o \
	${OBJECTDIR}/g_util.o \
	${OBJECTDIR}/lwalgorithm.o \
	${OBJECTDIR}/lwtree.o \
	${OBJECTDIR}/lwcircstring.o \
	${OBJECTDIR}/lwspheroid.o \
	${OBJECTDIR}/g_geometry.o \
	${OBJECTDIR}/lwpoint.o \
	${OBJECTDIR}/g_ptarray.o \
	${OBJECTDIR}/lwutil.o \
	${OBJECTDIR}/lwpoly.o \
	${OBJECTDIR}/lwmcurve.o \
	${OBJECTDIR}/lex.yy.o \
	${OBJECTDIR}/lwgunparse.o \
	${OBJECTDIR}/lwmpoint.o \
	${OBJECTDIR}/lwcurvepoly.o \
	${OBJECTDIR}/lwgeodetic.o \
	${OBJECTDIR}/lwmline.o \
	${OBJECTDIR}/lwgparse.o \
	${OBJECTDIR}/lwgeom.o \
	${OBJECTDIR}/wktparse.tab.o \
	${OBJECTDIR}/lwmpoly.o \
	${OBJECTDIR}/lwcompound.o \
	${OBJECTDIR}/lwgeom_api.o


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
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ../../dist/libliblwgeom.so

../../dist/libliblwgeom.so: ${OBJECTFILES}
	${MKDIR} -p ../../dist
	${LINK.c} -shared -o ../../dist/libliblwgeom.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/lwsegmentize.o: lwsegmentize.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwsegmentize.o lwsegmentize.c

${OBJECTDIR}/lwline.o: lwline.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwline.o lwline.c

${OBJECTDIR}/g_box.o: g_box.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/g_box.o g_box.c

${OBJECTDIR}/g_coord.o: g_coord.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/g_coord.o g_coord.c

${OBJECTDIR}/lwcollection.o: lwcollection.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwcollection.o lwcollection.c

${OBJECTDIR}/box2d.o: box2d.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/box2d.o box2d.c

${OBJECTDIR}/g_serialized.o: g_serialized.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/g_serialized.o g_serialized.c

${OBJECTDIR}/measures.o: measures.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/measures.o measures.c

${OBJECTDIR}/lwmsurface.o: lwmsurface.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwmsurface.o lwmsurface.c

${OBJECTDIR}/vsprintf.o: vsprintf.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/vsprintf.o vsprintf.c

${OBJECTDIR}/ptarray.o: ptarray.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/ptarray.o ptarray.c

${OBJECTDIR}/g_util.o: g_util.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/g_util.o g_util.c

${OBJECTDIR}/lwalgorithm.o: lwalgorithm.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwalgorithm.o lwalgorithm.c

${OBJECTDIR}/lwtree.o: lwtree.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwtree.o lwtree.c

${OBJECTDIR}/lwcircstring.o: lwcircstring.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwcircstring.o lwcircstring.c

${OBJECTDIR}/lwspheroid.o: lwspheroid.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwspheroid.o lwspheroid.c

${OBJECTDIR}/g_geometry.o: g_geometry.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/g_geometry.o g_geometry.c

${OBJECTDIR}/lwpoint.o: lwpoint.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwpoint.o lwpoint.c

${OBJECTDIR}/g_ptarray.o: g_ptarray.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/g_ptarray.o g_ptarray.c

${OBJECTDIR}/lwutil.o: lwutil.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwutil.o lwutil.c

${OBJECTDIR}/lwpoly.o: lwpoly.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwpoly.o lwpoly.c

${OBJECTDIR}/lwmcurve.o: lwmcurve.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwmcurve.o lwmcurve.c

${OBJECTDIR}/lex.yy.o: lex.yy.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lex.yy.o lex.yy.c

${OBJECTDIR}/lwgunparse.o: lwgunparse.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwgunparse.o lwgunparse.c

${OBJECTDIR}/lwmpoint.o: lwmpoint.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwmpoint.o lwmpoint.c

${OBJECTDIR}/lwcurvepoly.o: lwcurvepoly.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwcurvepoly.o lwcurvepoly.c

${OBJECTDIR}/lwgeodetic.o: lwgeodetic.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwgeodetic.o lwgeodetic.c

${OBJECTDIR}/lwmline.o: lwmline.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwmline.o lwmline.c

${OBJECTDIR}/lwgparse.o: lwgparse.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwgparse.o lwgparse.c

${OBJECTDIR}/lwgeom.o: lwgeom.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwgeom.o lwgeom.c

${OBJECTDIR}/wktparse.tab.o: wktparse.tab.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/wktparse.tab.o wktparse.tab.c

${OBJECTDIR}/lwmpoly.o: lwmpoly.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwmpoly.o lwmpoly.c

${OBJECTDIR}/lwcompound.o: lwcompound.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwcompound.o lwcompound.c

${OBJECTDIR}/lwgeom_api.o: lwgeom_api.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/lwgeom_api.o lwgeom_api.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../../dist/libliblwgeom.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
