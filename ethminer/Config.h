#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <json/json.h>
#include <unordered_map>
#include <utility>
#include <iostream>

using namespace std;
class Config{
private:
        string name;
        string owner;
        string walletAddress;
        int gpu;

        //Following two for the log submission
        string server;
        string port;
private:
        Config(){
        }
        Config(const Config&);
        Config& operator=(const Config&);
public:
        static Config&  getInstance(){
                static Config instance;
                return instance;
        }

        void  loadConfig(string file){
                boost::property_tree::ptree pt;
                boost::property_tree::ini_parser::read_ini(file, pt);
                name= pt.get<std::string>("config.name");
                owner = pt.get<string>("config.owner");
                walletAddress= pt.get<std::string>("config.walletAddress");
                gpu = pt.get<int>("config.gpu");
                server = pt.get<int>("config.server");
        }


public:
        int getGPU(){
                return gpu;
        }
        string getName(){
                return name;
        }

        string getOwner(){
                return owner;
        }

        string getWalletAddres(){
                return walletAddress;
        }

        string getRemoteServer(){
                return server;
        }

        string getPort(){
                return port;
        }

        void initJson(Json::Value& root){
                root["name"] = name;
                root["owner"] = owner;

                root["walletAddress"] = walletAddress;
                root["gpu"] = gpu;
        }

};
