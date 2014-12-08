#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../main.c ../spiEEProm.c ../spiFlashWinbond.c ../usb_descriptors.c ../../../microchip/USB/usb_device.c "../../../microchip/USB/HID Device Driver/usb_function_hid.c"

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/spiEEProm.o ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o ${OBJECTDIR}/_ext/1472/usb_descriptors.o ${OBJECTDIR}/_ext/249864330/usb_device.o ${OBJECTDIR}/_ext/615013156/usb_function_hid.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/spiEEProm.o.d ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o.d ${OBJECTDIR}/_ext/1472/usb_descriptors.o.d ${OBJECTDIR}/_ext/249864330/usb_device.o.d ${OBJECTDIR}/_ext/615013156/usb_function_hid.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/spiEEProm.o ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o ${OBJECTDIR}/_ext/1472/usb_descriptors.o ${OBJECTDIR}/_ext/249864330/usb_device.o ${OBJECTDIR}/_ext/615013156/usb_function_hid.o

# Source Files
SOURCEFILES=../main.c ../spiEEProm.c ../spiFlashWinbond.c ../usb_descriptors.c ../../../microchip/USB/usb_device.c ../../../microchip/USB/HID Device Driver/usb_function_hid.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ256GB206
MP_LINKER_FILE_OPTION=,--script="..\Bootloader Files\boot_hid_boot_p24FJ256GB206.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/spiEEProm.o: ../spiEEProm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/spiEEProm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/spiEEProm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../spiEEProm.c  -o ${OBJECTDIR}/_ext/1472/spiEEProm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/spiEEProm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/spiEEProm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/spiFlashWinbond.o: ../spiFlashWinbond.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../spiFlashWinbond.c  -o ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/spiFlashWinbond.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/spiFlashWinbond.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/usb_descriptors.o: ../usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../usb_descriptors.c  -o ${OBJECTDIR}/_ext/1472/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/usb_descriptors.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/249864330/usb_device.o: ../../../microchip/USB/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/249864330 
	@${RM} ${OBJECTDIR}/_ext/249864330/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/249864330/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../microchip/USB/usb_device.c  -o ${OBJECTDIR}/_ext/249864330/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/249864330/usb_device.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/249864330/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/615013156/usb_function_hid.o: ../../../microchip/USB/HID\ Device\ Driver/usb_function_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/615013156 
	@${RM} ${OBJECTDIR}/_ext/615013156/usb_function_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/615013156/usb_function_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../../microchip/USB/HID Device Driver/usb_function_hid.c"  -o ${OBJECTDIR}/_ext/615013156/usb_function_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/615013156/usb_function_hid.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/615013156/usb_function_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"      -g -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/spiEEProm.o: ../spiEEProm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/spiEEProm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/spiEEProm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../spiEEProm.c  -o ${OBJECTDIR}/_ext/1472/spiEEProm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/spiEEProm.o.d"      -g -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/spiEEProm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/spiFlashWinbond.o: ../spiFlashWinbond.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../spiFlashWinbond.c  -o ${OBJECTDIR}/_ext/1472/spiFlashWinbond.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/spiFlashWinbond.o.d"      -g -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/spiFlashWinbond.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/usb_descriptors.o: ../usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../usb_descriptors.c  -o ${OBJECTDIR}/_ext/1472/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/usb_descriptors.o.d"      -g -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/249864330/usb_device.o: ../../../microchip/USB/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/249864330 
	@${RM} ${OBJECTDIR}/_ext/249864330/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/249864330/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../microchip/USB/usb_device.c  -o ${OBJECTDIR}/_ext/249864330/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/249864330/usb_device.o.d"      -g -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/249864330/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/615013156/usb_function_hid.o: ../../../microchip/USB/HID\ Device\ Driver/usb_function_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/615013156 
	@${RM} ${OBJECTDIR}/_ext/615013156/usb_function_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/615013156/usb_function_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../../microchip/USB/HID Device Driver/usb_function_hid.c"  -o ${OBJECTDIR}/_ext/615013156/usb_function_hid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/615013156/usb_function_hid.o.d"      -g -omf=elf -Os -I".." -I"../../../netcruzer/lib" -I"../../../microchip/Include" -I"../../../microchip/Include/USB" -I"../Configs" -I"../../../microchip/USB" -DBRD_SBC66EC_R1 -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/615013156/usb_function_hid.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../Bootloader\ Files/boot_hid_boot_p24FJ256GB206.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="/C/Program Files/Microchip/MPLAB C30/lib",--library-path="../Bootloader Files",--no-force-link,--smart-io,-Map="${DISTDIR}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../Bootloader\ Files/boot_hid_boot_p24FJ256GB206.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="/C/Program Files/Microchip/MPLAB C30/lib",--library-path="../Bootloader Files",--no-force-link,--smart-io,-Map="${DISTDIR}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
	@echo Normalizing hex file
	@"C:/Program Files (x86)/Microchip/MPLABX/mplab_ide/mplab_ide/modules/../../bin/hexmate" --edf="C:/Program Files (x86)/Microchip/MPLABX/mplab_ide/mplab_ide/modules/../../dat/en_msgs.txt" dist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.hex -odist/${CND_CONF}/${IMAGE_TYPE}/SBC66EC-R1_USBHIDBoot.X.${IMAGE_TYPE}.hex

endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
