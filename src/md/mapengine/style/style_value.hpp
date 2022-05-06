#pragma once
#include <array>
#include <string>
#include <vector>
#include <map>

#include <md/util/color.hpp> 
#include <md/util/rapidjson.hpp> 
#include <md/util/geo.hpp>
#include <md/util/stops_def.hpp>
  
#include <md/mapengine/style/styletypes.hpp>

 
namespace clientmap {

class JsonTool
{
public:
    static bool isUndefined(const JSValue* value) {
        return value->IsNull();
    }

    static bool isArray(const JSValue* value) {
        return value->IsArray();
    }

    static std::size_t arrayLength(const JSValue* value)
    {
        return value->Size();
    }

    static const JSValue*
        arrayMember(const JSValue* value, std::size_t i)
    {
        return &(*value)[rapidjson::SizeType(i)];
    }

    static bool isObject(const JSValue* value) {
        return value->IsObject();
    }

    static const JSValue*  
        objectMember(const JSValue* value, const char * name)
    {
        if (!value->HasMember(name)) {
            return {};
        }
        const JSValue* const& member = &(*value)[name];
        return { member };
    }

    template <class Fn>
    static int
        eachMember(const JSValue* value, Fn&& fn) //----
    {
        assert(value->IsObject());
        for (const auto& property : value->GetObject())
        {
            int result = fn(
                { property.name.GetString(), property.name.GetStringLength() },
                property.value
            );
            if (result < 0) {
                return result;
            }
        }
        return 0;//return {};
    }
};

int parseStyleConst(const JSValue&  value, bool& aBool);
int parseStyleConst(const JSValue&  value, float& aF);
int parseStyleConst(const JSValue&  value, double& aF);
int parseStyleConst(const JSValue&  value, std::string& aStr);
int parseStyleConst(const JSValue&  value, LonLat& pt);
 
//------------------------------------------
template <class T, typename Enable = void>
struct StyleConverter;

template<class T>
struct StyleConverter< ExponentialStops<T>
    //,typename std::enable_if_t<
    //std::is_same<T, ColorI>::value || 
    //std::is_same<T, float>::value ||
    //std::is_same<T, double>::value  > 
    >
{
    static constexpr const char *type = "exponential";

    int operator()(const JSValue &value,
        ExponentialStops<T> & aResult) const;
};


template<class T>
struct StyleConverter< IntervalStops<T> 
    , typename  std::enable_if_t<
    std::is_same<T, std::string>::value ||
    std::is_same<T, std::vector<float> >::value ||
    std::is_same<T, bool>::value ||
    std::is_enum<T>::value  > 
    >
{
    static constexpr const char *type = "interval";

    int operator()(const JSValue &value,
        IntervalStops<T> &error) const;
};
 
}//namespace clientmap 