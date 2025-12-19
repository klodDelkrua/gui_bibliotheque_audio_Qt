# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/bibliotheque_audio_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/bibliotheque_audio_autogen.dir/ParseCache.txt"
  "bibliotheque_audio_autogen"
  )
endif()
