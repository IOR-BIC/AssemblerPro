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

PROJECT(appInteraction)

# Set your list of sources here.
SET(PROJECT_SRCS
appInteractor2DTemplate.cpp		appInteractor2DTemplate.h
)

SET(PROJECT_LIBS ${PROJECT_LIBS})

IF(APP_BUILD_DLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DAPP_INTERACTIONS_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} albaDLL appViews)
ELSE(APP_BUILD_DLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (APP_BUILD_DLL)