/**
 * Copyright (c) 2016 Tim Lindquist,
 * Software Engineering,
 * Arizona State University at the Polytechnic campus
 * <p/>
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation version 2
 * of the License.
 * <p/>
 * This program is distributed in the hope that it will be useful,
 * but without any warranty or fitness for a particular purpose.
 * <p/>
 * Please review the GNU General Public License at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 * see also: https://www.gnu.org/licenses/gpl-faq.html
 * so you are aware of the terms and your rights with regard to this software.
 * Or, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,USA
 * <p/>
 * Purpose: C++ command line student collection client using json-rpc-cpp.
 * Meant to run on OSX, Debian Linux, and Raspberry Pi Debian.
 * <p/>
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Tim Lindquist (Tim.Lindquist@asu.edu),ASU-IAFSE,Software Engineering
 * @file    StudentTerminalClient.cpp
 * @date    July, 2015
 * @license See above
 **/

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <jsonrpccpp/client/connectors/httpclient.h>
// the include below requires that you've built the jsonrpccpp package manually
// with the switchs as follows:
// cmake ../ -DTCP_SOCKET_SERVER=YES -DTCP_SOCKET_CLIENT=YES
// make
// make doc    // this will build html docs in the build/doc/html directory
// sudo make install
//#include <jsonrpccpp/client/connectors/tcpsocketclient.h>
#include <json/json.h>
#include "studentcollectionstub.h"
#include "../server/Student.hpp"

using namespace jsonrpc;
using namespace std;

void split(vector<string> & aStringVector,  /* return value */
           const string & aString,
           const string & aDelimiter) {
   size_t  start = 0, end = 0;
   while ( end != string::npos) {
      end = aString.find(aDelimiter, start);
      // If at end, use length=maxLength.  Else use length=end-start.
      aStringVector.push_back(aString.substr(start,
                          (end == string::npos) ? string::npos : end - start));
      // If at end, use start=maxSize.  Else use start=end+delimiter.
      start = ((end > (string::npos - aDelimiter.size()) )
               ? string::npos : end + aDelimiter.size());
   }
}

int main(int argc, char*argv[]) {
   string host = "http://127.0.0.1:1099";
   if(argc>1){
      host = string(argv[1]);
   }
   // the server is communicating via http requests, so create a stub object.
   HttpClient httpclient(host);
   studentcollectionstub sc(httpclient);
   // to use tcp sockets instead of http uncomment below (comment above), and the include
   // for tcpsocketclient.h above. If not running locally, you will need to input ip & port
   // from command line for both server and client programs.
   //TcpSocketClient tcpclient("localhost",8080);
   //studentcollectionstub sc(tcpclient);
   cout << "Connecting to host " << host << endl;
   try {
      string inLine;
      while(true){
         std::cout << "Enter end, getNames, get aName, remove aName, getById 123 >";
         std::getline (std::cin, inLine);
         vector<string> tokens;
         split(tokens, inLine, " ");
         string arg = "";
         for(int i=1; i<tokens.size(); i++){
            arg = arg + tokens[i];
            if (tokens.size()>i+1){
               arg = arg + " ";
            }
         }
         /*
         if(tokens.size()>1){
            istringstream argStream(tokens[1]);
            argStream >> arg;
         }
         */
         if(tokens[0]=="getNames"){
            Json::Value result = sc.getNames();
            cout << "server has students named: ";
            for(int i=0; i<result.size(); i++){
               cout << " " << result[i].asString();
            }
            cout << endl;
            string resStr = result.toStyledString();
            cout << "result of getNames as Json string: " << resStr;
         } else if(tokens[0]=="get"){
            Json::Value aMDinJson = sc.get(arg);
            Student md(aMDinJson);
            md.print();
         } else if(tokens[0]=="remove"){
            bool res = sc.remove(arg);
         } else if(tokens[0]=="getById"){
            std::stringstream ss(arg);
            int id = 0;
            ss >> id;
            string res = sc.getById(id);
            cout << "result of getById " << arg << " is " << res << endl;
         } else if(tokens[0]=="end"){
            break;
         }
      }
   } catch (JsonRpcException e) {
      cerr << e.what() << endl;
   }
}

