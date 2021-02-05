#=========================================================================
# Program:   Assembler
# Module:    APPConfigMacro.cmake
# Language:  CMake
# Date:      $Date: 2021-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Nicola Vanella
#==========================================================================
# Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#=========================================================================*/
#
# Macro to synchronously configure a sub project
# This simply includes a subproject Configure.cmake script
# that should do all necessary configuration. A couple of useful
# variables are also defined. 
# Notice SUBPROJECT_NAME SUBPROJECT_DIR SUBPROJECT_SOURCE_DIR SUBPROJECT_BINARY_DIR are not 
# rentrant variables
#
MACRO(APP_SUBPROJECT SUBPROJECT_NAME SUBPROJECT_DIR)
  
  # if not previous configure errors
  IF (NOT CONFIGURE_ERROR)  
    IF (EXISTS ${PROJECT_SOURCE_DIR}/${SUBPROJECT_DIR}/Configure.cmake)
      MESSAGE (STATUS "Configuring subproject ${SUBPROJECT_NAME}")
      # Set some useful variables
      SET(SUBPROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}/${SUBPROJECT_DIR})
      SET (${SUBPROJECT_NAME}_BINARY_DIR ${PROJECT_BINARY_DIR}/${SUBPROJECT_DIR})
      SET(SUBPROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${SUBPROJECT_DIR})
      SET (${SUBPROJECT_NAME}_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${SUBPROJECT_DIR})
        
      # set flag to advise the subfolder was found
      SET (${SUBPROJECT_NAME}_CONFIG_FOUND 1)

      # execute subproject configuration
      INCLUDE (${SUBPROJECT_SOURCE_DIR}/Configure.cmake)
    ENDIF (EXISTS ${PROJECT_SOURCE_DIR}/${SUBPROJECT_DIR}/Configure.cmake)
  ELSE (NOT CONFIGURE_ERROR)
    MESSAGE ("Not configuring ${SUBPROJECT_NAME} due to previuos errors")
  ENDIF (NOT CONFIGURE_ERROR)

ENDMACRO(APP_SUBPROJECT)

# Return the date (yyyy-mm-dd)
macro(DATE RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect date")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()

# Return the date (dd-mm-yyyy)
macro(DATE2 RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..)/(..)/(....).*" "\\1-\\2-\\3" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%d-%m-%Y" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect date")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()

# Return the time (hh:mm:ss)
macro(TIME RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C echo %TIME%" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..:..:..),(..)" "\\1" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%H:%M:%S" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect time")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()