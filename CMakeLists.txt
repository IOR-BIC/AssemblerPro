#=========================================================================
# Program:   Assembler
# Module:    CMakeLists.txt
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

SET (CMAKE_BACKWARDS_COMPATIBILITY "2.4" CACHE INTERNAL "This project is for CMake 2.4")
CMAKE_MINIMUM_REQUIRED(VERSION 2.4) 

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
	cmake_policy(SET CMP0048 NEW)
endif(COMMAND cmake_policy)

PROJECT (Assembler)

#-----------------------------------------------------------------------------
# This enables support for Visual Studio: Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	#Add OpenMP Support
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

	#Add support for Edit & Continue
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	IF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	ENDIF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/appDefines.h.in 
							 ${PROJECT_BINARY_DIR}/appDefines.h 
							 @ONLY)

#-----------------------------------------------------------------------------
# This define the APP_SUBPROJECT macro
#-----------------------------------------------------------------------------
INCLUDE (${ASB_SOURCE_DIR}/CMakeModules/APPConfigMacro.cmake)

#-----------------------------------------------------------------------------
# Where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${ASB_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${ASB_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH)

FIND_PATH(MANUAL_HTML_DIR APP/APP_help.html)

#-----------------------------------------------------------------------------
# Options for APP building
#-----------------------------------------------------------------------------
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." 0)
MARK_AS_ADVANCED (BUILD_SHARED_LIBS)

OPTION(BUILD_TESTING "Build the testing tree." OFF)

#-----------------------------------------------------------------------------
# Find required libraries
#-----------------------------------------------------------------------------

INCLUDE("${PROJECT_SOURCE_DIR}/CMakeModules/FindALBA.cmake")
INCLUDE("${ALBA_BINARY_PATH}/ALBAConfig.cmake")

# Find ALBA library
IF(ALBA_FOUND)
	
  INCLUDE("${ALBA_USE_FILE}")
  
	# IF(APP_BUILD_DLL)
    # SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /FORCE:MULTIPLE")
	  # SET(APP_BUILD_DLL 1)
	# ELSE(APP_BUILD_DLL)
      SET(APP_BUILD_DLL 0)
	# ENDIF (RAPP_BUILD_DLL)
  
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Operations/appOperationsDefines.h.in ${PROJECT_BINARY_DIR}/appOperationsDefines.h @ONLY)
  #CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/VME/appVMEDefines.h.in ${PROJECT_BINARY_DIR}/appVMEDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Common/appCommonDefines.h.in ${PROJECT_BINARY_DIR}/appCommonDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Gui/appGuiDefines.h.in ${PROJECT_BINARY_DIR}/appGuiDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Views/appViewsDefines.h.in ${PROJECT_BINARY_DIR}/appViewsDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Interaction/appInteractionDefines.h.in ${PROJECT_BINARY_DIR}/appInteractionDefines.h @ONLY)
  
  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")
  INCLUDE_DIRECTORIES(${ASB_BINARY_DIR})
  INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/Application)
  INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/Common)
  INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/Operations)
  INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/Interaction)
  INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/Views)
  INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/Gui)
  INCLUDE_DIRECTORIES(${ASB_BINARY_DIR}/vtkAPP)
  #INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/VME)
  #INCLUDE_DIRECTORIES(${ASB_SOURCE_DIR}/VisualPipes)
  
  SUBDIRS (Application)
  SUBDIRS (Common)
  SUBDIRS (Operations)
  SUBDIRS (Interaction)  
  SUBDIRS (Views)
  SUBDIRS (Gui)
  SUBDIRS (vtkAPP)
  #SUBDIRS (VME)
  #SUBDIRS (VisualPipes)
  
  # Configuring Doxygen documentation  
  SUBDIRS (Documentation)
  
  # Export Build settings and library dependencies
  INCLUDE(CMakeExportBuildSettings)
  
  CMAKE_EXPORT_BUILD_SETTINGS(
    ${ASB_BINARY_DIR}/APPBuildSettings.cmake)
  
  export(TARGETS FILE ${ASB_BINARY_DIR}/APPLibraryDepends.cmake)
	
  CONFIGURE_FILE(
    ${ASB_SOURCE_DIR}/CMakeInput/APPConfig.cmake.in
    ${ASB_BINARY_DIR}/APPConfig.cmake
  @ONLY IMMEDIATE)
  
  CONFIGURE_FILE(
    ${ASB_SOURCE_DIR}/CMakeInput/APPUse.cmake.in
    ${ASB_BINARY_DIR}/APPUse.cmake
  @ONLY IMMEDIATE)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  IF(BUILD_TESTING)
    
    MESSAGE("variable ASB_SOURCE_DIR = " ${ASB_SOURCE_DIR})
    FIND_PATH(APP_DATA_ROOT "TestingDataRoot.txt" "${ASB_SOURCE_DIR}/Testing/UnitTestsData" "${ASB_SOURCE_DIR}/../TestingData" "${ASB_SOURCE_DIR}/../mmData/TestingData" "${ASB_SOURCE_DIR}/../../TestingData"  DOC "Test data repository")
    ENABLE_TESTING()
      
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
      
    SUBDIRS (Testing)
    
  ENDIF(BUILD_TESTING)
  
ELSE (ALBA_FOUND)
  MESSAGE (FATAL_ERROR "ALBA library not found, cannot build application!!")
ENDIF (ALBA_FOUND)

#-----------------------------------------------------------------------------
# APP deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for NSIS Nullsoft Installer that runs currently under Windows only and Parabuild.
# An APP_VERSION variable is created to gather versioning information
IF (WIN32)
    
  # Installer useful values
  SET(APP_EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/Release/" CACHE PATH "Source directory for APP executable. This is used by the installer to find the executable.")
  
  STRING(REPLACE "/" "\\" APP_EXECUTABLE_OUTPUT_PATH_WIN ${APP_EXECUTABLE_OUTPUT_PATH})
  SET(APP_EXECUTABLE_OUTPUT_PATH_WIN ${APP_EXECUTABLE_OUTPUT_PATH_WIN} CACHE INTERNAL "get the executable from this directory")
  
  # MESSAGE("variable value = " ${APP_EXECUTABLE_OUTPUT_PATH})
  # MESSAGE("variable value = " ${APP_EXECUTABLE_OUTPUT_PATH_WIN})
  
  # Test for environment variable reading
  # MESSAG# "$ENV{PATH}")

  # Automatic values from parabuild created temporary environment variable
  SET(BUILD_NUMBER $ENV{BUILD_NUMBER} CACHE INTERNAL "build number")
  
  # Create the main version variable:
  SET(APP_VERSION "Build_${BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
  
  # MESSAGE("variable value = " ${APP_VERSION})
    
  # Config installer file
  # Set the installer directory variable
  SET(APP_INSTALLER_DIR ${PROJECT_SOURCE_DIR}/Installer)
  # Convert it to Windows format
  STRING (REPLACE "/" "\\" APP_INSTALLER_DIR_WIN ${APP_INSTALLER_DIR} )
  # MESSAGE("variable APP_INSTALLER_DIR_WIN = " ${APP_INSTALLER_DIR_WIN})
  
  # Installer executable will be written to this directory
  SET(APP_INSTALLER_EXECUTABLE_OUTPUT_DIR ${APP_EXECUTABLE_OUTPUT_PATH} CACHE PATH "the installer executable will be written to this directory ")
  # Convert it to Windows format  
  STRING(REPLACE "/" "\\" APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${APP_INSTALLER_EXECUTABLE_OUTPUT_DIR})
  SET(APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN} CACHE INTERNAL "get the executable  from this directory")
  # MESSAGE("variable APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN = " ${APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN})
  
  # Installer Configuration
  CONFIGURE_FILE(${APP_INSTALLER_DIR}/APPInstallerScript.nsi.in  ${APP_EXECUTABLE_OUTPUT_PATH}/APPInstallerScript.nsi @ONLY)
  CONFIGURE_FILE(${ALBA_BINARY_PATH}/INSTALLER/EnumUsersReg.nsh  ${APP_EXECUTABLE_OUTPUT_PATH}/EnumUsersReg.nsh @ONLY)
  CONFIGURE_FILE(${ALBA_BINARY_PATH}/INSTALLER/InstallerCommon.nsh  ${APP_EXECUTABLE_OUTPUT_PATH}/InstallerCommon.nsh @ONLY)
  
  IF(EXISTS "${PROJECT_SOURCE_DIR}/ParabuildScripts/")
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/ParabuildScripts/tagCVS.bat.in ${PROJECT_SOURCE_DIR}/ParabuildScripts/tagCVS.bat @ONLY)
  ENDIF (EXISTS "${PROJECT_SOURCE_DIR}/ParabuildScripts/")

  # Version File Info
  DATE2(date) #(dd-mm-yyyy)
  STRING (REPLACE "-" "/" APP_DATE ${date} )
  message(${APP_VERSION} " " ${APP_DATE})
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Version.txt.in ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.txt @ONLY)

ENDIF (WIN32)