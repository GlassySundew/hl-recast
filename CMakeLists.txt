cmake_minimum_required(VERSION 3.12)
project(recast)

function(set_as_hdll target)
    set_target_properties(${target}.hdll
        PROPERTIES
        PREFIX ""
        OUTPUT_NAME ${target}
        SUFFIX .hdll
    )
endfunction()

# For your own glue (e.g. src/recast.cpp) to export HL entry points
add_definitions(-DRECAST_EXPORTS)
# Kept for parity with bullet; MSVC will just warn on this flag
add_definitions(-fpermissive)

include(GNUInstallDirs)
set(HDLL_DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Path to the recastnavigation sources vendor’d into this project
set(RECAST_DIR lib/recastnavigation)

# Path to your hashlink checkout
set(HASHLINK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../hashlink)

# Includes for recastnavigation modules + hashlink
include_directories(
    ${RECAST_DIR}/Detour/Include
    ${RECAST_DIR}/DetourCrowd/Include
    ${RECAST_DIR}/DetourTileCache/Include
    ${RECAST_DIR}/Recast/Include
    ${RECAST_DIR}/DebugUtils/Include
    ${HASHLINK_DIR}/src
)

if (NOT WIN32)
    link_directories(${HASHLINK_DIR})
    link_directories(/usr/local/lib)
endif()

# All core Recast/Detour sources (no RecastDemo)
set(RECAST_SOURCES
    # Detour
    ${RECAST_DIR}/Detour/Source/DetourAlloc.cpp
    ${RECAST_DIR}/Detour/Source/DetourAssert.cpp
    ${RECAST_DIR}/Detour/Source/DetourCommon.cpp
    ${RECAST_DIR}/Detour/Source/DetourNavMesh.cpp
    ${RECAST_DIR}/Detour/Source/DetourNavMeshBuilder.cpp
    ${RECAST_DIR}/Detour/Source/DetourNavMeshQuery.cpp
    ${RECAST_DIR}/Detour/Source/DetourNode.cpp

    # DetourCrowd
    ${RECAST_DIR}/DetourCrowd/Source/DetourCrowd.cpp
    ${RECAST_DIR}/DetourCrowd/Source/DetourLocalBoundary.cpp
    ${RECAST_DIR}/DetourCrowd/Source/DetourObstacleAvoidance.cpp
    ${RECAST_DIR}/DetourCrowd/Source/DetourPathCorridor.cpp
    ${RECAST_DIR}/DetourCrowd/Source/DetourPathQueue.cpp
    ${RECAST_DIR}/DetourCrowd/Source/DetourProximityGrid.cpp

    # DetourTileCache
    ${RECAST_DIR}/DetourTileCache/Source/DetourTileCache.cpp
    ${RECAST_DIR}/DetourTileCache/Source/DetourTileCacheBuilder.cpp

    # Recast (navmesh generation)
    ${RECAST_DIR}/Recast/Source/Recast.cpp
    ${RECAST_DIR}/Recast/Source/RecastAlloc.cpp
    ${RECAST_DIR}/Recast/Source/RecastArea.cpp
    ${RECAST_DIR}/Recast/Source/RecastAssert.cpp
    ${RECAST_DIR}/Recast/Source/RecastContour.cpp
    ${RECAST_DIR}/Recast/Source/RecastFilter.cpp
    ${RECAST_DIR}/Recast/Source/RecastLayers.cpp
    ${RECAST_DIR}/Recast/Source/RecastMesh.cpp
    ${RECAST_DIR}/Recast/Source/RecastMeshDetail.cpp
    ${RECAST_DIR}/Recast/Source/RecastRasterization.cpp
    ${RECAST_DIR}/Recast/Source/RecastRegion.cpp

    # DebugUtils (optional but handy for debug draw)
    ${RECAST_DIR}/DebugUtils/Source/DebugDraw.cpp
    ${RECAST_DIR}/DebugUtils/Source/DetourDebugDraw.cpp
    ${RECAST_DIR}/DebugUtils/Source/RecastDebugDraw.cpp
    ${RECAST_DIR}/DebugUtils/Source/RecastDump.cpp

    ${RECAST_DIR}/RecastDemo/Source/PerfTimer.cpp

)

# Just like bullet: dump the source list to a file (if you need it)
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/sourcelist.txt "")
foreach(source IN LISTS RECAST_SOURCES)
    file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/sourcelist.txt "${source}\n")
endforeach()
file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/sourcelist.txt "src/idl_hl.cpp\n")

# Main hdll target; src/recast.cpp is your HL glue / WebIDL-generated file
add_library(recast.hdll SHARED
    ${RECAST_SOURCES}
    src/idl_hl.cpp
)

if (WIN32)
    # Same trick as bullet: $(Platform) is the VS generator variable
    target_link_directories(recast.hdll PRIVATE "${HASHLINK_DIR}/$(Platform)")
    target_link_libraries(recast.hdll
        libhl
    )
else()
    target_link_libraries(recast.hdll
        hl
    )
endif()

target_include_directories(recast.hdll
    PRIVATE
    ${RECAST_DIR}/Detour/Include
    ${RECAST_DIR}/DetourCrowd/Include
    ${RECAST_DIR}/DetourTileCache/Include
    ${RECAST_DIR}/Recast/Include
    ${RECAST_DIR}/DebugUtils/Include
    ${RECAST_DIR}/RecastDemo/Include
)

set_as_hdll(recast)

# Drop recast.hdll directly into the hashlink root (same as bullet)
set_target_properties(recast.hdll PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${HASHLINK_DIR}
)

# Optional override for install location on Windows (mirror bullet)
if (WIN32)
    set(HDLL_DESTINATION "C:/GameDev/hl/")
endif()

install(
    TARGETS
        recast.hdll
    DESTINATION ${HDLL_DESTINATION}
)