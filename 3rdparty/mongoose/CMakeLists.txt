
dqm4hep_package( mongoose
  MAJOR 6 MINOR 12 PATCH 0
  INCLUDE_DIRS include
  INSTALL_INCLUDES include/
)

# -------------------------------------------------
# build the mongoose library
dqm4hep_add_package_library( mongoose
  SOURCES src/*.c
  LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT}
  DEFINITIONS 
    -DMG_ENABLE_THREADS 
    -DMG_ENABLE_HTTP_WEBSOCKET 
    -DMG_ENABLE_HTTP 
    -DMG_ENABLE_BROADCAST
)

