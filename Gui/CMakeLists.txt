#==========================================================================
# Program:   AssemblerPro
# Module:    CMakeLists.txt
# Language:  CMake
# Date:      $Date: 2021-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Nicola Vanella
#==========================================================================
# Copyright (c) BIC-IOR 2021 (http://www.ltmsoftware.org/alba.htm)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#==========================================================================

PROJECT(appGui)

# Set your list of sources here.
SET(PROJECT_SRCS
appGUI.cpp					appGUI.h
appSideBar.cpp				appSideBar.h
appGUIDialogProducer.cpp	appGUIDialogProducer.h
appGUIDialogProsthesis.cpp	appGUIDialogProsthesis.h
appGUIDialogComponent.cpp	appGUIDialogComponent.h
appGUIDialogMatrix.cpp		appGUIDialogMatrix.h
appGUISeparator.cpp			appGUISeparator.h
)

IF(APP_BUILD_DLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DVERTEBRAPP_GUI_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} albaDLL)
ELSE(APP_BUILD_DLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (APP_BUILD_DLL)