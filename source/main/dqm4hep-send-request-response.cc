  /// \file dqm4hep-send-request.cc
/*
 *
 * dqm4hep-send-request.cc source template automatically generated by a class generator
 * Creation date : sam. d�c. 3 2016
 *
 * This file is part of DQM4HEP libraries.
 * 
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "dqm4hep/Client.h"

#include "json/json.h"

using namespace dqm4hep::net;

int main(int argc, char **argv)
{
  if(argc < 3)
  {
    std::cout << "Usage : dqm4hep-send-request-response type name [content]" << std::endl;
    return 1;
  }

  std::string type(argv[1]);
  std::string name(argv[2]);
  Json::Value request, response;
  std::string jsonString;

  for(int i=3 ; i<argc ; i++)
    jsonString += argv[i];

  Json::Reader reader;
  bool success = reader.parse(jsonString, request);

  if(!success)
  {
    std::cout << "Invalid json string !" << std::endl;
    return 1;
  }

  Client client;
  client.sendRequest(type, name, request, response);

  Json::StyledWriter writer;
  std::cout << writer.write(response) << std::endl;

  return 0;
}

