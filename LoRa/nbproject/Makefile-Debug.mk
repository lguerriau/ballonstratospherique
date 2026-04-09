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
	${OBJECTDIR}/_ext/7a03ee3f/bme280.o \
	${OBJECTDIR}/_ext/7a03ee3f/i2c.o \
	${OBJECTDIR}/_ext/d297970e/LM75.o \
	${OBJECTDIR}/RadioLib/src/Hal.o \
	${OBJECTDIR}/RadioLib/src/Module.o \
	${OBJECTDIR}/RadioLib/src/modules/SX127x/SX1278.o \
	${OBJECTDIR}/RadioLib/src/modules/SX127x/SX127x.o \
	${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.o \
	${OBJECTDIR}/RadioLib/src/utils/Utils.o \
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
LDLIBSOPTIONS=-lpthread -lgpiod -llgpio

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lora

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lora: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lora ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/7a03ee3f/bme280.o: ../BME280/bme280.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7a03ee3f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a03ee3f/bme280.o ../BME280/bme280.cpp

${OBJECTDIR}/_ext/7a03ee3f/i2c.o: ../BME280/i2c.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7a03ee3f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a03ee3f/i2c.o ../BME280/i2c.cpp

${OBJECTDIR}/_ext/d297970e/LM75.o: ../LM75/LM75.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d297970e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d297970e/LM75.o ../LM75/LM75.cpp

${OBJECTDIR}/RadioLib/src/Hal.o: RadioLib/src/Hal.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/Hal.o RadioLib/src/Hal.cpp

${OBJECTDIR}/RadioLib/src/Module.o: RadioLib/src/Module.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/Module.o RadioLib/src/Module.cpp

${OBJECTDIR}/RadioLib/src/modules/SX127x/SX1278.o: RadioLib/src/modules/SX127x/SX1278.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/modules/SX127x
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/modules/SX127x/SX1278.o RadioLib/src/modules/SX127x/SX1278.cpp

${OBJECTDIR}/RadioLib/src/modules/SX127x/SX127x.o: RadioLib/src/modules/SX127x/SX127x.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/modules/SX127x
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/modules/SX127x/SX127x.o RadioLib/src/modules/SX127x/SX127x.cpp

${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.o: RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.o RadioLib/src/protocols/PhysicalLayer/PhysicalLayer.cpp

${OBJECTDIR}/RadioLib/src/utils/Utils.o: RadioLib/src/utils/Utils.cpp
	${MKDIR} -p ${OBJECTDIR}/RadioLib/src/utils
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioLib/src/utils/Utils.o RadioLib/src/utils/Utils.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DRADIOLIB_PLATFORM_PIHAL -IRadioLib/src -I. -I../BME280 -I../LM75 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

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
