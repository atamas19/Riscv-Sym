# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appRiscvSim_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appRiscvSim_autogen.dir\\ParseCache.txt"
  "appRiscvSim_autogen"
  )
endif()
