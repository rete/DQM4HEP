//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include <dqm4hep/Logging.h>
#include <dqm4hep/NetworkEventLoop.h>
#include <dqm4hep/NetworkManager.h>
#include <dqm4hep/NetworkCommon.h>

#include <signal.h>

using namespace dqm4hep::net;

NetworkManager networkMgr;

void sig_handler(int sig) {
  networkMgr.stop();
}

int main(int argc, char ** argv) {
  int port = (argc > 1) ? atoi(argv[1]) : Utils::networkManagerPort();
  signal(SIGINT,  sig_handler);
  networkMgr.start(port);
  return 0;
}
