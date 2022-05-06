
#include <md/util/string.hpp>
#include <md/util/logtool.hpp>
#include <md/util/cpp_misc.hpp>
#include <md/util/interpolate.hpp>
//#include <md/mapengine/style/rapidjson_conversion.hpp> 
#include <md/mapengine/style/style_value.hpp>

 
namespace clientmap {
 
int parseStyleConst(const JSValue&  v, bool& aBool)
{
    if (!v.IsBool()) {
        return  -1;//error
    }
    aBool = v.GetBool();
    return 0;// ok 
}

int parseStyleConst(const JSValue&  value, float& aF)
{
    if (!value.IsNumber()) {
        return -1;//error
    }
    double d = value.GetDouble();
    aF = (float)d;
    return 0;// ok
}

int parseStyleConst(const JSValue&  value, double& aD)
{
    if (!value.IsNumber()) {
        return -1;//error
    }
    aD = value.GetDouble();
    return 0;// ok
}

int
parseStyleConst(const JSValue&  value, std::string& aStr)
{
    if (!value.IsString()) 
    {
        return -1;//error
    }//return {{ value->GetString(), value->GetStringLength() }};
    aStr = { value.GetString(), value.GetStringLength() };
    return 0;//ok
}


int
parseStyleConst(const JSValue& value, LonLat& aResult)
{
    if (!JsonTool::isArray(&value) || JsonTool::arrayLength(&value) < 2)
    {
        Log::Error(
            "coordinate array must contain numeric longitude and latitude values"
        );
        return -1;
    }
    //Style spec uses GeoJSON convention for specifying coordinates
    double  longitude = 0;
    double  latitude = 0;
    int retLng = parseStyleConst(value[0], longitude);
    int retLat = parseStyleConst(value[1], latitude);

    if ((retLat < 0) || (retLng < 0)) {
        Log::Error(
            "coordinate array must contain numeric longitude and latitude values"
        );
        return -2;
    }

    if (latitude < -90 || latitude > 90) {
        Log::Error(
            "coordinate latitude must be between -90 and 90"
        );
        return -3;
    }

    aResult = { longitude, latitude };
    return 0;
}

int parseStyleConst(const JSValue& value, ColorI& aC)
{
    std::string  str;
    int ret = parseStyleConst(value, str);
    if (ret < 0) {
        Log::Error("style json value must be a string");
        return ret;
    }

    ret = ColorI::parse255RGB(str, aC);
    if (ret < 0)
    {
        Log::Error("style json value must be a valid color");
        return -2;
    }

    return 0;
}

#if 0
template <class T>
int
StyleConverter<T,
    typename std::enable_if_t<std::is_enum<T>::value>
>::operator()(const JSValue&  value, T& aEnum)const
{
    std::string str;
    int ret = JsonConvert::toString(&value, str);
    if (ret < 0)
    {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be a string"
        );
        return ret;
    }

    ret = Enum<T>::toEnum(str, aEnum);
    if (ret < 0) {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be a valid enumeration value"
        );
        return ret;
    }

    return 0;
}


template <class T>
auto StyleConverter< std::vector<T>,
    typename std::enable_if_t<std::is_enum<T>::value
    >
>::operator()(const JSValue&  value, std::vector<T>& aEnumVec) const -> int
{
 
    if (!JsonTool::isArray(&value))
    {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array");
        return -1;
    }

    std::vector<T> result;
    result.reserve(value.Size());
    for (std::size_t i = 0; i < value.Size(); ++i)
    {
        T  enumItem = {};
        int ret = StyleConverter<T>{}(value[i], enumItem);
        if (ret < 0) {
            return -2;
        }
        result.push_back(enumItem);
    }
 
    return 0;
}

template int
StyleConverter<style::AlignmentType>::operator()(const JSValue&,
    style::AlignmentType&) const;
template int
StyleConverter<style::CirclePitchScaleType>::operator()(const JSValue&,
    style::CirclePitchScaleType&) const;
template int
StyleConverter<style::HillshadeIlluminationAnchorType>::operator()(const JSValue&,
    style::HillshadeIlluminationAnchorType&) const;
template int
StyleConverter<style::IconTextFitType>::operator()(const JSValue&,
    style::IconTextFitType&) const;
template int
StyleConverter<style::LightAnchorType>::operator()(const JSValue&,
    style::LightAnchorType&) const;
template int
StyleConverter<style::LineCapType>::operator()(const JSValue&,
    style::LineCapType&) const;
template int
StyleConverter<style::LineJoinType>::operator()(const JSValue&,
    style::LineJoinType&) const;
template int
StyleConverter<style::RasterResamplingType>::operator()(const JSValue&,
    style::RasterResamplingType&) const;
template int
StyleConverter<style::SymbolAnchorType>::operator()(const JSValue&,
    style::SymbolAnchorType&) const;
template int
StyleConverter<style::SymbolPlacementType>::operator()(const JSValue&,
    style::SymbolPlacementType&) const;
template int
StyleConverter<style::SymbolZOrderType>::operator()(const JSValue&,
    style::SymbolZOrderType&) const;
template int
StyleConverter<style::TextJustifyType>::operator()(const JSValue&,
    style::TextJustifyType&) const;
template int
StyleConverter<style::TextTransformType>::operator()(const JSValue&,
    style::TextTransformType&) const;
template int
StyleConverter<style::TranslateAnchorType>::operator()(const JSValue&,
    style::TranslateAnchorType&) const;
template int
StyleConverter<style::VisibilityType>::operator()(const JSValue&,
    style::VisibilityType&) const;
template int
StyleConverter<std::vector<style::TextVariableAnchorType>>::operator()(const JSValue&,
    std::vector<style::TextVariableAnchorType>&) const;
template int
StyleConverter<std::vector<style::TextWritingModeType>>::operator()(const JSValue&,
    std::vector<style::TextWritingModeType>&) const;
#endif



template <size_t N>
int parseStyleConst(const JSValue& value, std::array<float, N>& aOut)
{
    if (!JsonTool::isArray(&value) || JsonTool::arrayLength(&value) != N)
    {
        Log::Error("value must be an array of %d numbers", N);
        return -1;
    }

    //std::array<float, N> result;
    for (size_t i = 0; i < N; i++)
    {
        float  n = 0;
        int ret = parseStyleConst( value[i], n);
        if (ret < 0)
        {
            Log::Error("value must be an array of %d numbers" ,N);
            return -2;
        }
        aOut[i] = n;
    }
 
    return 0;
}

// explicit instantiate
template int
parseStyleConst<2>(const JSValue&, std::array<float, 2>&);
template int
parseStyleConst<3>(const JSValue&, std::array<float, 3>&);
template int
parseStyleConst<4>(const JSValue&, std::array<float, 4>&);

 
template <size_t N>
int
parseStyleConst(const JSValue& value, std::array<double, N>& aOut)
{
    if (!JsonTool::isArray(&value) ||
        JsonTool::arrayLength(&value) != N)
    {
        //error.message = "value must be an array of " 
        //     + util::toString(N) + " numbers";
        Log::Error( "value must be an array of %d numbers",N );
        return -1;
    }

    //std::array<double, N> result;
    for (size_t i = 0; i < N; i++)
    {
        double n = 0;
        const JSValue* arrMember = JsonTool::arrayMember(&value, i);
        int ret = parseStyleConst(*arrMember, n);
        if (ret < 0)
        {
            //error.message = "value must be an array of " 
            // + util::toString(N) + " numbers";
            Log::Error("value must be an array of %d numbers",N);
            return -2;
        }
        aOut[i] = n;
    }
    return 0;
}

template int
parseStyleConst<3>(const JSValue&, std::array<double, 3>&);

int
parseStyleConst(const JSValue& value, std::vector<float>& aOut)
{
    if (!JsonTool::isArray(&value))
    {
        //error.message = "value must be an array";
        Log::Error("value must be an array");
        return -1;
    }

    //std::vector<float> result;
    aOut.reserve(JsonTool::arrayLength(&value));

    for (std::size_t i = 0; i < JsonTool::arrayLength(&value); ++i)
    {
        float  number = 0;
        const JSValue* arrMember=JsonTool::arrayMember(&value, i);
        int ret = parseStyleConst(*arrMember,number);
        if (ret < 0)
        {
            //error.message = "value must be an array of numbers";
            Log::Error("value must be an array of numbers");
            return -2;
        }
        aOut.push_back(number);
    }

    return 0;
}

int
parseStyleConst(const JSValue& value, std::vector<std::string>& error) 
{
    if (!JsonTool::isArray(&value)) {
        //error.message = "value must be an array";
        Log::Error( "value must be an array");
        return -1;
    }

    std::vector<std::string> result;
    result.reserve(JsonTool::arrayLength(&value));

    for (std::size_t i = 0; i < JsonTool::arrayLength(&value); ++i)
    {
        std::string str;
        const JSValue* arrMember =JsonTool::arrayMember(&value, i);
        int ret = parseStyleConst(*arrMember, str);
        if (ret < 0)
        {
            //error.message = "value must be an array of strings";
            Log::Error("value must be an array of strings");
            return -2;
        }
        result.push_back(str);
    }

    return 0;
} 
//-----------------------------------------------------------------
//-----------------------------------------------------------------
template<class D, class R>
int convertStops(const JSValue& value, std::map<D, R> & aStops)
{
    if (!JsonTool::isObject(&value))
    {
        std::string err = { "function must be an object" };
        return -1;
    }

    auto stopsValue = JsonTool::objectMember(&value, "stops");
    if (!stopsValue)
    {
        std::string err = { "function value must specify stops" };
        return -2;
    }

    if (!JsonTool::isArray(stopsValue))
    {
        std::string err = { "function stops must be an array" };
        return -3;
    }

    if (JsonTool::arrayLength(stopsValue) == 0)
    {
        std::string err = { "function must have at least one stop" };
        return -4;
    }

    for (std::size_t i = 0; i < JsonTool::arrayLength(stopsValue); ++i)
    {
        const auto &stopV= JsonTool::arrayMember(stopsValue, i);

        if (!JsonTool::isArray(stopV))
        {
            std::string err = { "function stop must be an array" };
            return -5;
        }

        if (JsonTool::arrayLength(stopV) != 2)
        {
            std::string err = { "function stop must have two elements" };
            return -6;
        }

        D d{};
        int ret = parseStyleConst( *JsonTool::arrayMember(stopV, 0), d);
        if (ret < 0)
        {
            continue;
        }

        R r{};
        ret = parseStyleConst( *JsonTool::arrayMember(stopV, 1), r);
        if (ret < 0)
        {
            continue;
        }

        aStops.emplace(d, r);
    }

    return 0;
} 

static float convertBase(const JSValue& aValue)
{
    auto baseValue = JsonTool::objectMember(&aValue, "base");
    if (!baseValue) {
        return 1.0;
    }

    float base = 1.0;
    int ret = parseStyleConst(*baseValue, base);
    if (ret < 0) {
        std::string errMsg = "function base must be a number";
        return 1.0;
    }

    return base;
} 

template<class T>
int StyleConverter<  ExponentialStops<T>
    //, typename std::enable_if_t<
    //std::is_same<T, ColorI>::value ||
    //std::is_same<T, float>::value ||
    //std::is_same<T, double>::value >
>::operator()(const JSValue &aValue, ExponentialStops<T> & aResult) const
{
    static_assert( Interpolatable<T>::value);

    if (JsonTool::isUndefined(&aValue))
    {
        aResult = {};
        return -1;
    }
    else if (!JsonTool::isObject(&aValue))
    {
        T  constant = {};
        int ret = parseStyleConst(aValue, constant);
        if (ret < 0) {
            aResult = {};
        }
        else {
            aResult = { constant };
        }
        return 0;
    }
    else {
        std::map<float, T> stops{};
        int ret = convertStops<float, T>(aValue, stops);
        if (ret < 0)
        {
            assert(0);
            aResult = {};
            return -2;
        }

        float base = convertBase(aValue);
        aResult = { stops, base };
        return 0;
    }
}

// 声明和实现分离的偏特化模板类,需要显式实例化  
// 否则 ，在exe中别的cpp中用该模板类，编译会有下面的错误
//error LNK2019 : 无法解析的外部符号 "StyleConverter<class clientmap::ExponentialStops<float>,void>::operator()(...)，该符号在函数 "parse( )" 中被引用
//error LNK2019 : 无法解析的外部符号 "StyleConverter<class clientmap::ExponentialStops<class std::array<float,2> >,void>::operator()(...)const " ，该符号在函数 "parse( ) 中被引用
template struct StyleConverter< ExponentialStops<ColorI> >;
template struct StyleConverter< ExponentialStops<float> >;
template struct StyleConverter< ExponentialStops< std::array<float, 2> > >;

template<class T>
int StyleConverter< IntervalStops<T>
    , typename std::enable_if_t<
    std::is_same<T, std::string>::value ||
    std::is_same<T, std::vector<float> >::value ||
    std::is_same<T, bool>::value ||
    std::is_enum<T>::value  >
>::operator()(const JSValue& aValue, IntervalStops<T> & aResult) const
{
    //static_assert(!Interpolatable<T>::value);

    if (JsonTool::isUndefined(&aValue))
    {
        aResult = {};
        return -1;
    }
    else if (!JsonTool::isObject(&aValue))
    {
        T  constant = {};
        int ret = parseStyleConst(aValue, constant);
        if (ret < 0) {
            aResult = {};
        }
        else {
            aResult = { constant };
        }
    }
    else {
        std::map<float, T> stops{};
        int ret = convertStops<float, T>(aValue, stops);
        if (ret < 0)
        {
            assert(0);
            aResult = {};
            return -2;
        }

        aResult = { stops };
        return 0;
    }
}

// 声明和实现分离的偏特化模板类,需要显式实例化  
// 否则 ，在exe中别的cpp中用该模板类，编译会有下面的错误
// error LNK2019 : 无法解析的外部符号
template struct StyleConverter< IntervalStops<std::string> >;
template struct StyleConverter< IntervalStops<bool> >;
template struct StyleConverter< IntervalStops<std::vector<float>> >;

}// namespace clientmap {