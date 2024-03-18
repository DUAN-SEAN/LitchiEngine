# set(PhysxRootDir "../../ThridParty/physx-4.1")
set(PhysxRootDir "../../ThirdParty/Physx")

#头文件目录
include_directories("${PhysxRootDir}/physx/include")
include_directories("${PhysxRootDir}/pxshared/include")

#链接目录
if(MSVC)
    # link_directories("${PhysxRootDir}/physx/bin/win.x86_64.vc142.mt/debug")
    link_directories(
        $<$<CONFIG:Debug>:${THIRD_PARTY_DIR}/PhysX/physx/bin/win.x86_64.vc142.mt/debug>
        $<$<CONFIG:Release>:${THIRD_PARTY_DIR}/PhysX/physx/bin/win.x86_64.vc142.mt/release>
    )
    #链接lib文件
    # link_libraries(physx_64 physxFoundation_64 physxExtensions_static_64 physxPvdSDK_static_64)
    set(LINK_LIBRARIES
        debug physx_64 physxFoundation_64 physxExtensions_static_64 physxPvdSDK_static_64
        optimized glut32 PhysXDevice64)
    link_libraries(${LINK_LIBRARIES})

    #拷贝dll到exe目录
    # file(COPY "physx-4.1/physx/bin/win.x86_64.vc142.mt/debug/physxFoundation_64.dll" DESTINATION "./")
    # file(COPY "physx/bin/win.x86_64.vc142.mt/debug/physxDevice64.dll" DESTINATION "./")
    # file(COPY "physx/bin/win.x86_64.vc142.mt/debug/physxCooking_64.dll" DESTINATION "./")
    # file(COPY "physx/bin/win.x86_64.vc142.mt/debug/physxCommon_64.dll" DESTINATION "./")
    # file(COPY "physx/bin/win.x86_64.vc142.mt/debug/physx_64.dll" DESTINATION "./")
else()
    link_directories("${PhysxRootDir}/physx/bin/mac.x86_64/debug")
    #链接lib文件
    link_libraries(physx_static_64 physxFoundation_static_64 physxExtensions_static_64 physxPvdSDK_static_64 physxCommon_static_64)
endif()