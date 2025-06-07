cmake_minimum_required(VERSION 3.16)
project(Calculator VERSION 1.0.0 LANGUAGES CXX)

# Add Objective-C++ support only on macOS
if(APPLE)
    enable_language(OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find Qt6 components with fallback message
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
if(NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 is required but not found. Please install Qt6 or set Qt6_DIR.")
endif()

# Enable Qt's MOC (Meta-Object Compiler) and resource compilation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)  # Enable UIC for .ui files

# Collect source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    src/*.cpp
    src/*.cxx
    src/*.cc
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS 
    src/*.hpp 
    src/*.h
    src/*.hxx
)

file(GLOB_RECURSE UI_FILES CONFIGURE_DEPENDS src/*.ui)

# Platform-specific source files
if(APPLE)
    file(GLOB_RECURSE OBJCXX_SOURCES CONFIGURE_DEPENDS src/*.mm)
    if(OBJCXX_SOURCES)
        list(APPEND SOURCES ${OBJCXX_SOURCES})
        # Set Objective-C++ compilation flags
        set_source_files_properties(${OBJCXX_SOURCES} PROPERTIES
            COMPILE_FLAGS "-x objective-c++"
        )
    endif()
endif()

# Add Qt resource files if they exist
set(RESOURCES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc")
    list(APPEND RESOURCES src/resources.qrc)
endif()

# Platform-specific app icon setup
set(APP_ICON_RESOURCES)
if(WIN32)
    # Windows: Use .rc file for icon
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/app.rc")
        list(APPEND APP_ICON_RESOURCES resources/app.rc)
    endif()
elseif(APPLE)
    # macOS: Use .icns file - FIXED VERSION
    set(ICNS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/Calculator.icns")
    if(EXISTS "${ICNS_FILE}")
        # Set bundle icon file name
        set(MACOSX_BUNDLE_ICON_FILE Calculator.icns)
        
        # Mark the .icns file as a resource
        set_source_files_properties("${ICNS_FILE}" PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
        
        # Add to resources list
        list(APPEND APP_ICON_RESOURCES "${ICNS_FILE}")
        
        message(STATUS "Using icon: ${ICNS_FILE}")
    else()
        message(WARNING "Calculator.icns not found at ${ICNS_FILE}")
        
        # Fallback: try to use PNG and convert during build
        set(PNG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/Calculator.png")
        if(EXISTS "${PNG_FILE}")
            message(STATUS "Found PNG, will convert to ICNS during build")
            
            # Add custom command to convert PNG to ICNS
            add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Calculator.icns"
                COMMAND mkdir -p Calculator.iconset
                COMMAND sips -z 16 16 "${PNG_FILE}" --out Calculator.iconset/icon_16x16.png
                COMMAND sips -z 32 32 "${PNG_FILE}" --out Calculator.iconset/icon_16x16@2x.png
                COMMAND sips -z 32 32 "${PNG_FILE}" --out Calculator.iconset/icon_32x32.png
                COMMAND sips -z 64 64 "${PNG_FILE}" --out Calculator.iconset/icon_32x32@2x.png
                COMMAND sips -z 128 128 "${PNG_FILE}" --out Calculator.iconset/icon_128x128.png
                COMMAND sips -z 256 256 "${PNG_FILE}" --out Calculator.iconset/icon_128x128@2x.png
                COMMAND sips -z 256 256 "${PNG_FILE}" --out Calculator.iconset/icon_256x256.png
                COMMAND sips -z 512 512 "${PNG_FILE}" --out Calculator.iconset/icon_256x256@2x.png
                COMMAND sips -z 512 512 "${PNG_FILE}" --out Calculator.iconset/icon_512x512.png
                COMMAND sips -z 1024 1024 "${PNG_FILE}" --out Calculator.iconset/icon_512x512@2x.png
                COMMAND iconutil -c icns Calculator.iconset
                COMMAND rm -rf Calculator.iconset
                DEPENDS "${PNG_FILE}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                COMMENT "Converting PNG to ICNS for macOS bundle"
            )
            
            # Add the generated ICNS file
            set(MACOSX_BUNDLE_ICON_FILE Calculator.icns)
            set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/Calculator.icns" PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources"
            )
            list(APPEND APP_ICON_RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/Calculator.icns")
        else()
            message(WARNING "No icon file found (neither Calculator.icns nor Calculator.png)")
        endif()
    endif()
endif()

# Create executable with all collected files
add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS} 
    ${UI_FILES}
    ${RESOURCES}
    ${APP_ICON_RESOURCES}
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Gui
)

# Platform-specific settings and frameworks
if(WIN32)
    # Windows-specific settings
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        WIN32_EXECUTABLE TRUE
        OUTPUT_NAME "${PROJECT_NAME}"
    )
    
    # Link Windows-specific libraries
    target_link_libraries(${PROJECT_NAME} 
        dwmapi
        user32
        gdi32
        shell32
    )
    
    # Set Windows version requirements
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WINVER=0x0A00          # Windows 10
        _WIN32_WINNT=0x0A00    # Windows 10
        NOMINMAX               # Prevent Windows.h from defining min/max macros
        WIN32_LEAN_AND_MEAN    # Reduce Windows header bloat
    )
    
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.calculator"
        MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2025 Your Company. All rights reserved."
    )

    # Set app icon - ENSURE this matches what we set above
    if(MACOSX_BUNDLE_ICON_FILE)
            set_target_properties(${PROJECT_NAME} PROPERTIES
                MACOSX_BUNDLE_ICON_FILE "${MACOSX_BUNDLE_ICON_FILE}"
        )
        message(STATUS "Bundle icon set to: ${MACOSX_BUNDLE_ICON_FILE}")
    endif()

    # Link macOS-specific frameworks
    target_link_libraries(${PROJECT_NAME} 
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework AppKit"
    )

    # Set minimum macOS version 
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_DEPLOYMENT_TARGET "10.15"
    )

    # IMPORTANT: Add custom command to copy icon if using generated ICNS
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Calculator.icns")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_BINARY_DIR}/Calculator.icns"
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/Calculator.icns"
            COMMENT "Copying icon to app bundle"
        )
    endif()
    
elseif(UNIX AND NOT APPLE)
    # Linux/Unix-specific settings
    
    # Find required system libraries
    find_package(PkgConfig QUIET)
    
    # Try to find X11 libraries for enhanced window management (optional)
    find_package(X11 QUIET)
    
    # Link common Linux libraries
    target_link_libraries(${PROJECT_NAME} 
        ${CMAKE_DL_LIBS}    # Dynamic linking library
        pthread             # POSIX threads
    )
    
    # Link X11 libraries if found
    if(X11_FOUND)
        target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
        target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_X11)
    endif()
    
    # Set RPATH for finding Qt libraries
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endif()

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC-specific settings
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4                    # High warning level
        /permissive-          # Strict standards conformance
        /utf-8                # UTF-8 source and execution character sets
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang-specific settings
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic    # Enable warnings
        -Wno-unused-parameter       # Suppress unused parameter warnings (common in Qt)
    )
    
    # Debug-specific flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
        target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
    endif()
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated MOC files
)

# Source grouping for IDEs (Visual Studio, Xcode, etc.)
if(SOURCES)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
endif()
if(HEADERS)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${HEADERS})
endif()
if(UI_FILES)
    source_group("UI Files" FILES ${UI_FILES})
endif()
if(RESOURCES)
    source_group("Resource Files" FILES ${RESOURCES})
endif()

# Installation rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .        # macOS bundle
    RUNTIME DESTINATION bin     # Windows/Linux executable
)

# Linux desktop integration
if(UNIX AND NOT APPLE)
    # Install desktop file
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/Calculator.desktop")
        install(FILES "resources/Calculator.desktop"
            DESTINATION share/applications
        )
    endif()
    
    # Install application icon
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/Calculator.png")
        install(FILES "resources/Calculator.png"
            DESTINATION share/icons/hicolor/256x256/apps
        )
    endif()
    
    # Install smaller icon sizes if available
    foreach(SIZE 16 24 32 48 64 128 256)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/Calculator-${SIZE}.png")
            install(FILES "resources/Calculator-${SIZE}.png"
                DESTINATION share/icons/hicolor/${SIZE}x${SIZE}/apps
                RENAME Calculator.png
            )
        endif()
    endforeach()
endif()

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "Calculator")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A beautiful cross-platform calculator with modern UI")
set(CPACK_PACKAGE_VENDOR "Your Company")
set(CPACK_PACKAGE_CONTACT "your.email@example.com")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "Calculator")
    set(CPACK_NSIS_PACKAGE_NAME "Calculator")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/Calculator.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/Calculator.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\Calculator.exe")
    set(CPACK_NSIS_HELP_LINK "https://yourwebsite.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://yourwebsite.com")
    set(CPACK_NSIS_CONTACT "your.email@example.com")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Calculator.lnk' '$INSTDIR\\\\bin\\\\Calculator.exe'"
    )
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$SMPROGRAMS\\\\$START_MENU\\\\Calculator.lnk'"
    )
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_VOLUME_NAME "Calculator")
    # set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/resources/DMGSetup.scpt")
    # set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/resources/dmg-background.png")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    
    # Debian package settings
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqt6gui6")
    set(CPACK_DEBIAN_PACKAGE_SECTION "math")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    
    # RPM package settings
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.0")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# Optional: Print configuration summary
message(STATUS "")
message(STATUS "${PROJECT_NAME} Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Qt6 Version: ${Qt6_VERSION}")
message(STATUS "")