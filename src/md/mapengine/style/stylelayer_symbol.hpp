 
#pragma once

#include <md/mapengine/style/stylelayer.hpp>
 

#include <vector>

namespace clientmap {
  
class SymbolStyle final : public StyleLayer {
public:
    SymbolStyle(const std::string& aLayerId,
        const std::string& aDsLayerId);
    ~SymbolStyle() override;
 
    int parse(const JSValue& value)override; 

protected:
 
};
 
} // namespace md

// clang-format on
