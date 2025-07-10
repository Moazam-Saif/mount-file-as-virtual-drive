cmake_minimum_required(VERSION 3.16)
project(VirtualDriveMount)

set(CMAKE_CXX_STANDARD 17)

# Find WinFsp installation
set(WINFSP_ROOT "C:/Program Files (x86)/WinFsp")
set(WINFSP_INCLUDE_DIR "${WINFSP_ROOT}/inc")
set(WINFSP_LIB_DIR "${WINFSP_ROOT}/lib")

# Check if WinFsp is installed
if(NOT EXISTS ${WINFSP_INCLUDE_DIR})
    message(FATAL_ERROR "WinFsp not found. Please install WinFsp from https://github.com/winfsp/winfsp/releases")
endif()

# Include directories
include_directories(
    "${WINFSP_INCLUDE_DIR}"
    "${WINFSP_INCLUDE_DIR}/fuse"   # <-- Add this line
)

# Link directories
link_directories(${WINFSP_LIB_DIR})

# Add executable
add_executable(VirtualDriveMount 
    main.cpp
    VirtualDrive.cpp
)

# Link libraries
target_link_libraries(VirtualDriveMount
    winfsp-x64
    Virtdisk
)

# Copy WinFsp DLL to output directory
add_custom_command(TARGET VirtualDriveMount POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${WINFSP_ROOT}/bin/winfsp-x64.dll"
    $<TARGET_FILE_DIR:VirtualDriveMount>
)
