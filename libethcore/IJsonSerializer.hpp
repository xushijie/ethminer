#pragma once

#include <string>
#include <json/json.h>

class JsonSerializable{
public:
  virtual ~JsonSerializable( void ) {};
  virtual void serialize( Json::Value& root ) =0;
  virtual void deserialize( Json::Value& root) =0;
};

class CJsonSerializerAPI
{
public:
  static bool serialize( JsonSerializable* pObj, std::string& output );
  static bool deserialize( JsonSerializable* pObj, std::string& input );

private:
  CJsonSerializerAPI( void ) {};
};
