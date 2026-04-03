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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/RadioLib/src/Hal.o \
	${OBJECTDIR}/RadioLib/src/Module.o \
	${OBJECTDIR}/RadioLib/src/modules/SX127x/SX1278.o \
	${OBJECTDIR}/RadioLib/src/modules/SX127x/SX127x.o \
	${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.o \
	${OBJECTDIR}/RadioLib/src/utils/Utils.o \
	${OBJECTDIR}/bme280.o \
	${OBJECTDIR}/i2c.o \
	${OBJECTDIR}/main.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bme280

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bme280: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bme280 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/RadioLib/src/Hal.o: RadioLib/src/Hal.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/Hal.o RadioLib/src/Hal.cpp

${OBJECTDIR}/RadioLib/src/Module.o: RadioLib/src/Module.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/Module.o RadioLib/src/Module.cpp

${OBJECTDIR}/RadioLib/src/modules/SX127x/SX1278.o: RadioLib/src/modules/SX127x/SX1278.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/modules/SX127x
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/modules/SX127x/SX1278.o RadioLib/src/modules/SX127x/SX1278.cpp

${OBJECTDIR}/RadioLib/src/modules/SX127x/SX127x.o: RadioLib/src/modules/SX127x/SX127x.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/modules/SX127x
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/modules/SX127x/SX127x.o RadioLib/src/modules/SX127x/SX127x.cpp

${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.o: RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.o RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.cpp

${OBJECTDIR}/RadioLib/src/utils/Utils.o: RadioLib/src/utils/Utils.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/utils/Utils.o RadioLib/src/utils/Utils.cpp

${OBJECTDIR}/bme280.o: bme280.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bme280.o bme280.cpp

${OBJECTDIR}/i2c.o: i2c.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/i2c.o i2c.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

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
