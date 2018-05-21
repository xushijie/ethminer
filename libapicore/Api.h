#pragma once

#include "ApiServer.h"
#include <libethcore/Farm.h>
#include <libethcore/Miner.h>
#include <jsonrpccpp/server/connectors/tcpsocketserver.h>
#include <json/json.h>
#include "ethminer/Config.h"
#include <unordered_map>
#include <string>
using namespace jsonrpc;
using namespace dev;
using namespace dev::eth;
using namespace std;

class Api
{
public:
	Api(const int &port, Farm &farm);
private:
	ApiServer *m_server;
	Farm &m_farm;
};

namespace HttpApi{
    bool postData(string json);
    string getLocalIp();
    void initialize(string remoteHost, string port);

    extern string remoteHost;
	extern string port;
	const string url = "/api/miner";

}

class JSonUtil{
public:
        static string json2string(Json::Value& root );

};

class JsonSetter {
private:
        Json::Value& _root;
public:
       JsonSetter(Json::Value& root):_root(root){
        }

        void operator()(std::pair<const string, string> ele) const {
                cout << "first : " << ele.first << "seconde : " << ele.second << endl;
                _root[ele.first] = ele.second;
        }
        /*
        void operator()(int ci) const {
                cout << ci << endl;
                }*/
};
