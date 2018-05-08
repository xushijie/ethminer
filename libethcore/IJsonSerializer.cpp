#include "libethcore/IJsonSerializer.hpp"

bool CJsonSerializerAPI::serialize(JsonSerializable* pObj, std::string& output)
{
        if(pObj==NULL) return false;
        Json::Value root;
        pObj->serialize(root);
        Json::StyledWriter writer;
        output = writer.write(root );
        return true;
}

bool CJsonSerializerAPI::deserialize(JsonSerializable* pObj, std::string& input)
{
        return true;
}
