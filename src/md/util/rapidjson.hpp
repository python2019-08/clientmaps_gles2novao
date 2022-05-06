#pragma once

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace clientmap {

using JSDocument = rapidjson::GenericDocument< rapidjson::UTF8<>, 
                                               rapidjson::CrtAllocator>;
using JSValue = rapidjson::GenericValue< rapidjson::UTF8<>, 
                                         rapidjson::CrtAllocator>;

std::string formatJSONParseError(const JSDocument&);

} //  namespace clientmap
