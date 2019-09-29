#include <iostream>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <json/json.h>
#include <vector>
#include "Message.cpp"
#include "MessageServer.cpp"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <map>

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
 * Purpose: C++ class to represent a collection of students. This class is
 * part of a student collection distributed application that uses JsonRPC.
 * Meant to run on OSX, Debian Linux, and Raspberry Pi Debian.
 * <p/>
 * Ser321 Principles of Distributed Software Systems
 * see http://pooh.poly.asu.edu/Ser321
 * @author Tim Lindquist Tim.Lindquist@asu.edu
 *         Software Engineering, CIDSE, IAFSE, ASU Poly
 * @version July 2016
 */


using namespace std;



class MessageLibrary {
protected:

	std::map<std::string, MessageLibrary> library;

public:

	string name;
	int studentid;
	vector<string> takes;

	MessageLibrary();
	MessageLibrary(const Json::Value &jsonObj);
	MessageLibrary(string jsonString);
	MessageLibrary(string aName, int studentid, vector<string> takes);
	virtual ~MessageLibrary();

	virtual bool resetFromJsonFile(string jsonFileName);
	virtual bool saveToJsonFile(string jsonFileName);
	string serviceInfo();
	virtual bool add(const Json::Value &aStudentJson);
	virtual bool remove(string studentName);
	virtual Json::Value get(string studentName); //returns a student's json
	virtual Json::Value getNames(); //returns a json array of strings
	virtual string getById(int studentid); //returns the students name whose id is studentid
	string toJsonString();
	Json::Value toJson();
	void fromJson(Json::Value json);
	void setValues(string aName, int aStudentId, vector<string> aTakes);
	void print();
};

MessageLibrary::MessageLibrary() {
	name = "";
	studentid = 0;
	takes = vector<string>();
}

MessageLibrary::MessageLibrary(string aName, int aStudentId,
		vector<string> aTakes) {
	name = aName;
	studentid = aStudentId;
	takes = aTakes;
}

MessageLibrary::MessageLibrary(const Json::Value &jsonObj) {
	string nameStr = "name";
	string studentidStr = "studentid";
	string takesStr = "takes";
	Json::Value::Members mbr = jsonObj.getMemberNames();
	for (vector<string>::const_iterator i = mbr.begin(); i != mbr.end(); i++) {
		Json::Value jsonM = jsonObj[*i];
		if (nameStr.compare(*i) == 0) {
			name = jsonM.asString();
		} else if (studentidStr.compare(*i) == 0) {
			studentid = jsonM.asInt();
		} else if (takesStr.compare(*i) == 0) {
			takes = vector<string>();
			for (int i = 0; i < jsonM.size(); i++) {
				takes.push_back(jsonM[i].asString());
			}
		}
	}
}

MessageLibrary::MessageLibrary(string jsonString) {
	string nameStr = "name";
	string studentidStr = "studentid";
	string takesStr = "takes";
	Json::Reader reader;
	Json::Value root;
	bool parseSuccess = reader.parse(jsonString, root, false);
	if (parseSuccess) {
		Json::Value::Members mbr = root.getMemberNames();
		for (vector<string>::const_iterator i = mbr.begin(); i != mbr.end();
				i++) {
			Json::Value jsonM = root[*i];
			if (nameStr.compare(*i) == 0) {
				name = jsonM.asString();
			} else if (studentidStr.compare(*i) == 0) {
				studentid = jsonM.asInt();
			} else if (takesStr.compare(*i) == 0) {
				takes = vector<string>();
				for (int i = 0; i < jsonM.size(); i++) {
					takes.push_back(jsonM[i].asString());
				}
			}
		}
	} else {
		cout << "Student constructor parse error with input: " << jsonString
				<< endl;
	}
}

MessageLibrary::~MessageLibrary() {
	name = "";
	studentid = -1;
}

string MessageLibrary::toJsonString() {
	string ret = "{}";
	Json::Value jsonLib;
	jsonLib["name"] = name;
	jsonLib["studentid"] = studentid;
	Json::Value tmp(Json::arrayValue);
	for (int i = 0; i < takes.size(); i++) {
		tmp[i] = takes[i];
	}
	jsonLib["takes"] = tmp;
	ret = jsonLib.toStyledString();
	return ret;
}

Json::Value MessageLibrary::toJson() {
	Json::Value jsonLib;
	jsonLib["name"] = name;
	jsonLib["studentid"] = studentid;
	Json::Value tmp(Json::arrayValue);
	for (int i = 0; i < takes.size(); i++) {
		tmp[i] = takes[i];
	}
	jsonLib["takes"] = tmp;
	return jsonLib;
}

void MessageLibrary::setValues(string aName, int aStudentId,
		vector<string> aTakes) {
	name = aName;
	studentid = aStudentId;
	takes = aTakes;
}

void MessageLibrary::print() {
	cout << "Student " << name << " with ID " << studentid << " takes ";
	for (int i = 0; i < takes.size(); i++) {
		cout << takes[i] << " ";
	}
	cout << endl;
}

MessageLibrary::MessageLibrary() {
	resetFromJsonFile("students.json");
}

MessageLibrary::~MessageLibrary() {
	//cout << "student collection destructor.\n";
	library.clear();
}

bool MessageLibrary::resetFromJsonFile(string jsonFileName) {
	bool ret = false;
	Json::Reader reader;
	Json::Value root;
	std::ifstream json(jsonFileName.c_str(), std::ifstream::binary);
	bool parseSuccess = reader.parse(json, root, false);
	if (parseSuccess) {
		Json::Value::Members mbr = root.getMemberNames();
		for (vector<string>::const_iterator i = mbr.begin(); i != mbr.end();
				i++) {
			//cout << *i << " " << endl;
			Json::Value jsonMedia = root[*i];
			MessageLibrary *aStudent = new MessageLibrary(jsonMedia);
			library[*i] = *aStudent;
			//cout << "adding ";
			//aStudent->print();
		}
		ret = true;
	}
	return ret;
}

bool MessageLibrary::saveToJsonFile(string jsonFileName) {
	bool ret = false;
	Json::Value jsonLib;
	for (std::map<string, MessageLibrary>::iterator i = library.begin();
			i != library.end(); i++) {
		string key = i->first;
		//cout << key << " " << endl;
		MessageLibrary aStudent = library[key];
		Json::Value jsonStudent = aStudent.toJson();
		jsonLib[key] = jsonStudent;
	}
	ret = true;
	Json::StyledStreamWriter ssw("  ");
	std::ofstream jsonOutFile(jsonFileName.c_str(), std::ofstream::binary);
	ssw.write(jsonOutFile, jsonLib);
	return ret;
}

bool MessageLibrary::add(const Json::Value &aStudentJson) {
	bool ret = false;
	MessageLibrary aStudent(aStudentJson);
	// The map container does not allow duplicates so adding will replace existing
	// value with new Student instance.
	//aStudent.print();
	library[aStudent.name] = aStudent;
	return true;
}

bool MessageLibrary::remove(string aName) {
	bool ret = false;
	// does the library contain this student? if so remove, if not return false
	if (library.count(aName) > 0) {
		library.erase(aName);
		ret = true;
	}
	return true;
}

Json::Value MessageLibrary::get(string aName) {
	MessageLibrary aStudent(
			"{\"name\":\"" + aName
					+ " Unknown\",\"studentid\":-1,\"takes\":[\"None\" ]}");
	if (library.count(aName) > 0) {
		aStudent = library[aName];
	}
	return aStudent.toJson();
}

Json::Value MessageLibrary::getNames() {
	Json::Value ret(Json::arrayValue);
	vector < string > myVec;
	for (map<string, MessageLibrary>::iterator it = library.begin();
			it != library.end(); ++it) {
		myVec.push_back(it->first);
	}
	for (std::vector<string>::iterator it = myVec.begin(); it != myVec.end();
			++it) {
		ret.append(Json::Value(*it));
	}
	return ret;
}

std::string MessageLibrary::getById(int studentid) {
	std::string ret = "";
	for (map<string, MessageLibrary>::iterator it = library.begin();
			it != library.end(); ++it) {
		MessageLibrary stud = library[it->first];
		if (stud.studentid == studentid) {
			ret = stud.name;
		}
	}
	return ret;
}
