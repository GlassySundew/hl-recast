CD /D %~dp0
dir
cmake .. -A x64 -G "Visual Studio 17 2022" -DRECAST_SRC_DIR="lib/recastnavigation" -DHL_INCLUDE_DIR="../src" -DHL_LIB_DIR="../" -DHDLL_DESTINATION="../"