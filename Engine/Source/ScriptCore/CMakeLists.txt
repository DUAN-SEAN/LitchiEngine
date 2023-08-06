
set(TARGET_NAME LitchiScriptCore)

# Setup project.
PROJECT(${TARGET_NAME} VERSION 1.0.0.0 LANGUAGES CSharp)

# Setup project to use C# utilities.
INCLUDE(CSharpUtilities)

file(GLOB_RECURSE CSharpSource "Source/*.cs")

# Create assembly info with current version.
#CONFIGURE_FILE("AssemblyInfo.cs.template" "${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cs")

# Add shared library project.
ADD_LIBRARY(${PROJECT_NAME} SHARED
    #"${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cs"
    ${CSharpSource}
)

# Set designer and XAML properties.
# CSHARP_SET_DESIGNER_CS_PROPERTIES("${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cs")

# 将Target工程添加到目录Engine目录下 
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# Set CLR assembly properties.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Core"
    VS_GLOBAL_ROOTNAMESPACE "LitchiEngine"
)
