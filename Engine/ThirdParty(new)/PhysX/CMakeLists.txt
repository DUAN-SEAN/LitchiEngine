cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# -----------------------------------------------------------------------------------------------------------------------------
# PROJECT
# -----------------------------------------------------------------------------------------------------------------------------

project("PhysX")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -----------------------------------------------------------------------------------------------------------------------------
# SUB-DIRECTORIES
# -----------------------------------------------------------------------------------------------------------------------------                    

# PhysX Library
add_subdirectory("PhysX")
add_subdirectory("PhysXCharacterKinematic")
add_subdirectory("PhysXVehicle")
add_subdirectory("PhysXExtensions")
add_subdirectory("SceneQuery")
add_subdirectory("SimulationController")
add_subdirectory("PhysXCooking")
add_subdirectory("Common")
add_subdirectory("PxFoundation")
add_subdirectory("PxPvdSDK")
add_subdirectory("LowLevel")
add_subdirectory("LowLevelAABB")
add_subdirectory("LowLevelDynamics")
add_subdirectory("LowLevelCloth")
add_subdirectory("LowLevelParticles")
add_subdirectory("PxTask")
add_subdirectory("PsFastXml")

# # Snippets
add_subdirectory("Snippets")

# Samples
# add_subdirectory("Samples")

