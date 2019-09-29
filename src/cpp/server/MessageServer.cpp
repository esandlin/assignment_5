/**
 * Copyright 2016 Tim Lindquist,
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p/>
 * Purpose: C++ class which serves as server for a collection of students. This
 * class is part of a student collection distributed application that uses JsonRPC.
 * Meant to run on OSX, Debian Linux, and Raspberry Pi Debian.
 * <p/>
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Tim Lindquist Tim.Lindquist@asu.edu
 *         Software Engineering, CIDSE, IAFSE, ASU Poly
 * @version July 2016
 */

#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
// the include below requires that you've built the jsonrpccpp package manually
// with the switchs as follows:
// cmake ../ -DTCP_SOCKET_SERVER=YES -DTCP_SOCKET_CLIENT=YES
// make
// make doc    // this will build html docs in the build/doc/html directory
// sudo make install
//#include <jsonrpccpp/server/connectors/tcpsocketserver.h>

#include <json/json.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <csignal>
#include "Message.cpp"
#include "MessageLibrary.cpp"
#include "studentserverstub.h"

using namespace jsonrpc;
using namespace std;

class MessageServer : public studentserverstub {
public:
   MessageServer(AbstractServerConnector &connector, int port);
   virtual std::string serviceInfo();
   virtual bool saveToJsonFile();
   virtual bool resetFromJsonFile();
   virtual bool add(const Json::Value& mediaName);
   virtual bool remove(const std::string& mediaName);
   virtual Json::Value get(const std::string& mediaName);
   virtual Json::Value getNames();
   virtual std::string getById(int studentId);
   
private:
   Message * library;
   int portNum;
};

MessageServer::MessageServer(AbstractServerConnector &connector, int port) :
                             studentserverstub(connector){
   library = new Message();
   library->resetFromJsonFile("students.json");
   portNum = port;
}

string MessageServer::serviceInfo(){
   std::string msg =
                "Student collection management service.";
   stringstream ss;
   ss << portNum;
   cout << "serviceInfo called. Returning: Student collection management service."
        << endl;
   return  msg.append(ss.str());
}

bool MessageServer::saveToJsonFile(){
   cout << "saving collection to students.json" << endl;
   bool ret = library->saveToJsonFile("students.json");
   return ret;
}

bool MessageServer::resetFromJsonFile(){
   cout << "restoring collection from students.json" << endl;
   bool ret = library->resetFromJsonFile("students.json");
   return ret;
}

bool MessageServer::add(const Json::Value& studentName) {
   cout << "Adding " << studentName << endl;
   bool ret = library->add(studentName);
   return ret;
}

bool MessageServer::remove(const string& studentName) {
   cout << "Removing " << studentName << endl;
   bool ret = library->remove(studentName);
   return ret;
}

Json::Value MessageServer::get(const string& studentName){
   cout << "Getting " << studentName << endl;
   return library->get(studentName);
}

Json::Value MessageServer::getNames(){
   Json::FastWriter fw;
   std::string names = fw.write(library->getNames());
   cout << "Get names returning: " << names  << endl;
   return library->getNames();
}

std::string MessageServer::getById(int studentId){
   cout << "getById returning: " << library->getById(studentId) << endl;
   return library->getById(studentId);
}

void exiting(){
   std::cout << "Server has been terminated. Exiting normally" << endl;
   //ss.StopListening();
}

int main(int argc, char * argv[]) {
   // invoke with ./bin/studentRPCServer 8080
   int port = 1099;
   if(argc > 1){
      port = atoi(argv[1]);
   }
   HttpServer httpserver(port);
   MessageServer ss(httpserver, port);
   // to use tcp sockets instead of http uncomment below (comment above), and the include
   // for tcpsocketserver.h above. If not running locally, you will need to input ip & port
   // from command line for both server and client programs.
   //TcpSocketServer tcpserver("localhost",port);
   //StudentServer ss(tcpserver, port);
   std::atexit(exiting);
   auto ex = [] (int i) {cout << "server terminating with signal " << i << endl;
                         // ss.StopListening();
                         exit(0);
                         //return 0;
                        };
   // ^C
   std::signal(SIGINT, ex);
   // abort()
   std::signal(SIGABRT, ex);
   // sent by kill command
   std::signal(SIGTERM, ex);
   // ^Z
   std::signal(SIGTSTP, ex);
   cout << "Student collection server listening on port " << port
      //<< " press return/enter to quit." << endl;
        << " use ps to get pid. To quit: kill -9 pid " << endl;
   ss.StartListening();
   Message cm("Eric Sandlin's Message Browser");
   return (Fl::run());
   while(true){

   }
   //int c = getchar();
   ss.StopListening();
   return 0;
}
