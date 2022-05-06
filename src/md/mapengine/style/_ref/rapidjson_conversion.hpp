#pragma once
 
#include <md/util/rapidjson.hpp>


using namespace MdU;
 
class JsonConvert
{
public:
    static bool isUndefined(const JSValue* value) {
        return value->IsNull();
    }

    static bool isArray(const JSValue* value) {
        return value->IsArray();
    }

    static std::size_t 
    arrayLength(const JSValue* value) 
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

    static const JSValue*  //----
    objectMember(const JSValue* value, const char * name) 
    {
        if (!value->HasMember(name)) {
            return {};
        }
        const JSValue* const& member = &(*value)[name];
        return {member};
    }

    template <class Fn>
    static int 
    eachMember(const JSValue* value, Fn&& fn) //----
    {
        assert( value->IsObject() );
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

    static int//optional<bool>
        toBool(const JSValue* value, bool& ret)
    {
        if (!value->IsBool()) {
            return  -1;//error
        }
        ret = value->GetBool();
        return 0;// ok
    }

    static int//optional<float> 
        toNumber(const JSValue* value, float& aResult)
    {
        if (!value->IsNumber()) {
            return -1;//error
        }
        aResult = (float)value->GetDouble();
        return 0;// ok
    }

    static int//optional<double> 
        toDouble(const JSValue* value, double& aResult)
    {
        if (!value->IsNumber()) {
            return -1;//error
        }
        aResult = value->GetDouble();
        return 0;// ok
    }

    static int//optional<std::string> 
        toString(const JSValue* value, std::string& aResult)
    {
        if (!value->IsString()) {
            return -1;//error
        }//return {{ value->GetString(), value->GetStringLength() }};
        aResult = { value->GetString(), value->GetStringLength() } ;
        return 0;//ok
    }

#if 0
    static optional<Value> toValue(const JSValue* value) 
    {
        switch (value->GetType()) {
            case rapidjson::kNullType:
            case rapidjson::kFalseType:
                return { false };

            case rapidjson::kTrueType:
                return { true };

            case rapidjson::kStringType:
                return { std::string { value->GetString(), 
                                       value->GetStringLength() 
                                       } 
                        };

            case rapidjson::kNumberType:
                if (value->IsUint64()) return { value->GetUint64() };
                if (value->IsInt64()) return { value->GetInt64() };
                return { value->GetDouble() };

            default:
                return {};
        }
    }

    static optional<GeoJSON> toGeoJSON(const JSValue* value, Error& error) 
    {
        try {
            return mapbox::geojson::convert(*value);
        } catch (const std::exception& ex) {
            error = { ex.what() };
            return {};
        }
    }
#endif
};
 

