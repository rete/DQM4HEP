#################################################
# cmake file for building DQMNet component
# @author Ete Remi, DESY
#################################################

dqm4hep_package( DQMNet
  USES DQMCore dim asio websocketpp mongoose
  INCLUDE_DIRS include
  INSTALL_INCLUDES include/dqm4hep
)

# -------------------------------------------------
# build the DQMNet library
dqm4hep_add_package_library( DQMNet
  SOURCES src/*.cc
  DEFINITIONS     
    -DMG_ENABLE_THREADS 
    -DMG_ENABLE_HTTP_WEBSOCKET 
    -DMG_ENABLE_HTTP 
    -DMG_ENABLE_BROADCAST
)

# -------------------------------------------------
# build the DQMNet binaries
# dqm4hep_add_executable( dqm4hep-send-command            SOURCES main/dqm4hep-send-command.cc )
# dqm4hep_add_executable( dqm4hep-send-request-response   SOURCES main/dqm4hep-send-request-response.cc )
# dqm4hep_add_executable( dqm4hep-send-request            SOURCES main/dqm4hep-send-request.cc )
# dqm4hep_add_executable( dqm4hep-server-info             SOURCES main/dqm4hep-server-info.cc )
# dqm4hep_add_executable( dqm4hep-server-list             SOURCES main/dqm4hep-server-list.cc )
# dqm4hep_add_executable( dqm4hep-server-running          SOURCES main/dqm4hep-server-running.cc )
# dqm4hep_add_executable( dqm4hep-subscribe-service       SOURCES main/dqm4hep-subscribe-service.cc )
# dqm4hep_add_executable( dqm4hep-test-ws-server          SOURCES main/test-ws-server.cc )
dqm4hep_add_executable( dqm4hep-network-manager         SOURCES main/dqm4hep-network-manager.cc )
dqm4hep_add_executable( dqm4hep-test-server             SOURCES main/test-server.cc )
dqm4hep_add_executable( dqm4hep-test-client             SOURCES main/test-client.cc )

# -------------------------------------------------
# build the DQMNet doxygen documentation
dqm4hep_package_doxygen_sources( 
  SOURCES include/dqm4hep/*.h src/*.cc
)
