#include <string>
#include <md/util/rapidjson.hpp>
 
namespace clientmap {

std::string formatJSONParseError(const JSDocument& doc) 
{
    return std::string{ rapidjson::GetParseError_En(doc.GetParseError()) } 
          + " at offset " 
          + std::to_string(doc.GetErrorOffset());
}

} // namespace mbgl


