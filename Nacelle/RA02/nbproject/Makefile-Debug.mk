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
	${OBJECTDIR}/AprsBuilder.o \
	${OBJECTDIR}/RadioManager.o \
	${OBJECTDIR}/StationApp.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/sx1278-LoRa-RaspberryPi-master/LoRa.o


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
LDLIBSOPTIONS=-lpigpio -lpthread -lwiringPi

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ra02

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ra02: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ra02 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/7a03ee3f/bme280.o: ../BME280/bme280.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7a03ee3f
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a03ee3f/bme280.o ../BME280/bme280.cpp

${OBJECTDIR}/_ext/7a03ee3f/i2c.o: ../BME280/i2c.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7a03ee3f
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a03ee3f/i2c.o ../BME280/i2c.cpp

${OBJECTDIR}/_ext/d297970e/LM75.o: ../LM75/LM75.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d297970e
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d297970e/LM75.o ../LM75/LM75.cpp

${OBJECTDIR}/AprsBuilder.o: AprsBuilder.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AprsBuilder.o AprsBuilder.cpp

${OBJECTDIR}/RadioManager.o: RadioManager.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RadioManager.o RadioManager.cpp

${OBJECTDIR}/StationApp.o: StationApp.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/StationApp.o StationApp.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/sx1278-LoRa-RaspberryPi-master/LoRa.o: sx1278-LoRa-RaspberryPi-master/LoRa.c
	${MKDIR} -p ${OBJECTDIR}/sx1278-LoRa-RaspberryPi-master
	${RM} "$@.d"
	$(COMPILE.c) -g -Isx1278-LoRa-RaspberryPi-master -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sx1278-LoRa-RaspberryPi-master/LoRa.o sx1278-LoRa-RaspberryPi-master/LoRa.c

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
