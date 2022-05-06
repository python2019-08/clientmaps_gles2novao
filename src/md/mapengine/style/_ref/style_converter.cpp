
#include <md/util/logging.hpp>
#include <md/util/ignore.hpp>
#include <md/util/interpolate.hpp>
#include <md/mapengine/style/rapidjson_conversion.hpp> 
#include <md/mapengine/style/style_converter.hpp>

//------------------------------------------
// src\mbgl\style\conversion\constant.cpp
//------------------------------------------
//namespace mbgl { 
//namespace style { 
//namespace conversion {
#define USE_JsonConvert  0
int 
StyleConverter<bool>::operator()(const JSValue&  value, bool& aBool) const
{
    return JsonConvert::toBool(&value, aBool);  
}

int
StyleConverter<float>::operator()(const JSValue&  value, 
    float& aF) const
{
    return JsonConvert::toNumber(&value, aF);
}

int
StyleConverter<std::string>::operator()(const JSValue&  value, 
    std::string& aStr) const
{
    return JsonConvert::toString(&value,aStr);
}

template <class T>
int 
StyleConverter<T, 
              typename std::enable_if_t<std::is_enum<T>::value>
              >::operator()(const JSValue&  value, T& aEnum)const
{
    std::string str;
    int ret= JsonConvert::toString(&value, str);
    if (ret <0) 
    {        
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be a string"
        );
        return ret;
    }

    ret = Enum<T>::toEnum(str, aEnum);
    if (ret <0) {
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
#if USE_JsonConvert
    if (!JsonConvert::isArray(&value)) 
    {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array"
        );
        return -1;
    }

    std::vector<T> result;
    result.reserve(JsonConvert::arrayLength(&value) );

    for (std::size_t i = 0; i < JsonConvert::arrayLength(&value); ++i)
    {
        T  enumItem = {};
        int ret = StyleConverter<T>{}( *JsonConvert::arrayMember(&value, i), 
                                      enumItem);//GNOTE:!!!
        if (ret < 0) {
            return -2;
        }
        result.push_back(enumItem);
    }
#else
    if (!value.IsArray())
    {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array" );
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
#endif
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

int 
StyleConverter<md::Color>::operator()(const JSValue& value, 
    md::Color& aC) const
{
    std::string  str;
    int ret= JsonConvert::toString(&value, str);
    if (ret < 0) {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be a string" );
        return ret;
    }

    ret = md::Color::parse(str,aC);
    if (ret <0) {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be a valid color" );
        return -2;
    }

    return 0;
}

template <size_t N>
int
StyleConverter<std::array<float, N>>::operator()(const JSValue& value, 
    std::array<float, N>& result) const
{
#if USE_JsonConvert
    if (!JsonConvert::isArray(&value) ||
         JsonConvert::arrayLength(&value) != N)
    { 
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array of "
                + MdU::toString(N) + " numbers"
        );
        return -1;
    }

    //std::array<float, N> result;
    for (size_t i = 0; i < N; i++) 
    {
        float  n = 0;
        int ret = JsonConvert::toNumber(JsonConvert::arrayMember(&value, i), n );
        if (ret <0) 
        {
            MdU::Log::Error(MdU::Event::ParseStyle,
                "value must be an array of " + MdU::toString(N) + " numbers"
            );
            return -2;
        }
        result[i] = n;
    }
#else
    if (!value.IsArray() ||  value.Size() != N)
    {
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array of "
            + MdU::toString(N) + " numbers"
        );
        return -1;
    }

    //std::array<float, N> result;
    for (size_t i = 0; i < N; i++)
    {
        float  n = 0;
        int ret = JsonConvert::toNumber( &value[i], n);
        if (ret < 0)
        {
            MdU::Log::Error(MdU::Event::ParseStyle,
                "value must be an array of " + MdU::toString(N) + " numbers"
            );
            return -2;
        }
        result[i] = n;
    }
#endif
    return 0;
}

template int
    StyleConverter<std::array<float, 2>>::operator()(const JSValue&, 
        std::array<float, 2>&) const;
template int
    StyleConverter<std::array<float, 3>>::operator()(const JSValue&, 
        std::array<float, 3>&) const;
template int
    StyleConverter<std::array<float, 4>>::operator()(const JSValue&, 
        std::array<float, 4>&) const;

template <size_t N>
int 
StyleConverter<std::array<double, N>>::operator()(const JSValue& value,
    std::array<double, N>& result) const
{
    if (!JsonConvert::isArray(&value) || 
        JsonConvert::arrayLength(&value) != N) 
    {
        //error.message = "value must be an array of " 
        //     + util::toString(N) + " numbers";
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array of "
            + MdU::toString(N) + " numbers"
        );
        return -1;
    }

    //std::array<double, N> result;
    for (size_t i = 0; i < N; i++) 
    {
        double n = 0;
        int ret = JsonConvert::toDouble( JsonConvert::arrayMember(&value, i), n);
        if (ret <0) 
        {
            //error.message = "value must be an array of " 
            // + util::toString(N) + " numbers";
            MdU::Log::Error(MdU::Event::ParseStyle,
                "value must be an array of " + MdU::toString(N) + " numbers"
            );
            return -2;
        }
        result[i] = n;
    }
    return 0;
}

template int
StyleConverter<std::array<double, 3>>::operator()(const JSValue&, 
    std::array<double, 3>&) const;

int 
StyleConverter<std::vector<float> >::operator()(const JSValue& value, 
    std::vector<float>& result
    ) const 
{
    if (!JsonConvert::isArray(&value)) 
    {
        //error.message = "value must be an array";
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array"
        );
        return -1;
    }

    //std::vector<float> result;
    result.reserve( JsonConvert::arrayLength(&value) );

    for (std::size_t i = 0; i < JsonConvert::arrayLength(&value); ++i)
    {
        float  number = 0;
        int ret = JsonConvert::toNumber( JsonConvert::arrayMember(&value, i), 
            number);
        if (ret <0) 
        {
            //error.message = "value must be an array of numbers";
            MdU::Log::Error(MdU::Event::ParseStyle,
                "value must be an array of numbers"
            );
            return -2;
        }
        result.push_back( number);
    }

    return 0;
}

int
StyleConverter<std::vector<std::string>>::operator()(const JSValue& value, 
    std::vector<std::string>& error) const
{
    if (!JsonConvert::isArray(&value)) {
        //error.message = "value must be an array";
        MdU::Log::Error(MdU::Event::ParseStyle,
            "value must be an array"
        );
        return -1;
    }

    std::vector<std::string> result;
    result.reserve(JsonConvert::arrayLength(&value));

    for (std::size_t i = 0; i < JsonConvert::arrayLength(&value); ++i)
    {
        std::string str;
        int ret= JsonConvert::toString(JsonConvert::arrayMember(&value, i),str);
        if (ret < 0 ) 
        {
            //error.message = "value must be an array of strings";
            MdU::Log::Error(MdU::Event::ParseStyle,
                "value must be an array of strings"
            );
            return -2;
        }
        result.push_back(str);
    }

    return 0;
}
//----------------------------------------------
// src\mbgl\style\conversion\coordinate.cpp
//------------------------------------------
int
StyleConverter<LatLng>::operator() (const JSValue& value, 
    LatLng& aResult) const
{
    if (!JsonConvert::isArray(&value) || 
         JsonConvert::arrayLength(&value) < 2) 
    {
        //error.message = "coordinate array must contain numeric longitude and latitude values";
        MdU::Log::Error(MdU::Event::ParseStyle,
            "coordinate array must contain numeric longitude and latitude values"
        );
        return -1;
    }
    //Style spec uses GeoJSON convention for specifying coordinates
    double  latitude = 0; 
    double  longitude = 0;
    int retLat = JsonConvert::toDouble(JsonConvert::arrayMember(&value, 1), 
        latitude);
    int retLng = JsonConvert::toDouble(JsonConvert::arrayMember(&value, 0), 
        longitude);

    if ( (retLat <0) || (retLng <0)) { 
        MdU::Log::Error(MdU::Event::ParseStyle,
            "coordinate array must contain numeric longitude and latitude values"
        );
        return -2;
    }

    if (latitude < -90 || latitude > 90) { 
        MdU::Log::Error(MdU::Event::ParseStyle,
            "coordinate latitude must be between -90 and 90"
        );
        return -3;
    }

    aResult = { latitude, longitude };
    return 0;
}


int
StyleConverter<md::StyleImage>::operator()(const JSValue& value,
	md::StyleImage& aResult) const
{
	std::string   result;
	if (JsonConvert::isArray(&value))
	{
		JSValue& imageParameters =(JSValue&)*JsonConvert::arrayMember(&value, 0);
		std::size_t imageParametersLength = JsonConvert::arrayLength(&imageParameters);
		if (imageParametersLength < 1) 
		{
			//error.message = "Image has to contain an ID.";
			MdU::Log::Error(MdU::Event::ParseStyle,
				"Image has to contain an ID."
			);
			return -1;
		}

		std::string imageID;
		int ret=JsonConvert::toString(JsonConvert::arrayMember(&imageParameters, 0),
			imageID);
		if (ret <0) {
			//error.message = "Image has to contain an ID.";
			MdU::Log::Error(MdU::Event::ParseStyle,
				"Image has to contain an ID."
			);
			return -2;
		}

		aResult = md::StyleImage(imageID, false);
		return 0;
	}
	else if ( JsonConvert::toString(&value, result) >=0) {
		aResult = md::StyleImage(result, false);
		return 0;
	}
	else {
		//error.message = "Image must be plain string or array type.";
		MdU::Log::Error(MdU::Event::ParseStyle,
			"Image must be plain string or array type."
		);
		return -3;
	}
}
 
//----------------------------------------------
// src\mbgl\style\conversion\function.cpp
//------------------------------------------
// toDo: 把该函数代码移到style_stops.hpp里，改函数名为parseStops
template<class D, class R>
int convertStops_358(const JSValue& value,
                     std::map<D, R> & aStops
                    )
{
    if (!JsonConvert::isObject(&value))
    {
        std::string err = { "function must be an object" };
        return -1;
    }

    auto stopsValue = JsonConvert::objectMember(&value, "stops");
    if (!stopsValue)
    {
        std::string err = { "function value must specify stops" };
        return -2;
    }

    if (!JsonConvert::isArray(stopsValue))
    {
        std::string err = { "function stops must be an array" };
        return -3;
    }

    if (JsonConvert::arrayLength(stopsValue) == 0)
    {
        std::string err = { "function must have at least one stop" };
        return -4;
    }

    for (std::size_t i = 0; i < JsonConvert::arrayLength(stopsValue); ++i)
    {
        const auto &stopValue = JsonConvert::arrayMember(stopsValue, i);

        if (!JsonConvert::isArray(stopValue))
        {
            std::string err = { "function stop must be an array" };
            return -5;
        }

        if (JsonConvert::arrayLength(stopValue) != 2)
        {
            std::string err = { "function stop must have two elements" };
            return -6;
        }

        D d{};
        int ret = StyleConverter<D>{}(*arrayMember(stopValue, 0), d);
        if (ret <0)
        {
            continue;
        }

        R r{};
        ret = StyleConverter<R>{}(*arrayMember(stopValue, 1), r);
        if (ret < 0)
        {
            continue;
        }

        aStops.emplace(d, r);
    }

    return 0;
}

static double convertBase(const JSValue& aValue ) 
{
    auto baseValue = JsonConvert::objectMember(&aValue, "base");
    if (!baseValue) {
        return 1.0;
    }

    double base = 1.0;
    int ret = JsonConvert::toDouble(baseValue, base);
    if (ret < 0) {
        std::string errMsg = "function base must be a number";
        return 1.0;
    }

    return base;
}


template<class T>
int StyleConverter<
        md::ExponentialStops<T> 
        //,std::enable_if<
        //std::is_same<T, md::Color>::value ||
        //std::is_same<T, float>::value ||
        //std::is_same<T, double>::value>

    >::operator()( const JSValue &aValue,
                  md::ExponentialStops<T> & aResult) const
{ 
        static_assert( MdU::Interpolatable<T>::aValue);

        if (JsonConvert::isUndefined(&aValue))
        {
            aResult = {};
            return 0;
        }
        else if (!JsonConvert::isObject(&aValue))
        {
            T  constant = {};
            int ret = StyleConverter<T>(aValue, constant);
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
            int ret = convertStops_358<float, T>(aValue, stops);
            if (ret < 0)
            {
                assert(0);
                aResult = {};
                return -1;
            }

            float base = convertBase(aValue);
            aResult = { stops, base };
            return 0;
        } 
}
 

template<class T> 
int StyleConverter<md::IntervalStops<T>
    //,std::enable_if<
    //std::is_same<T, std::string>::value ||
    //std::is_same<T, std::vector<float> >::value ||
    //std::is_same<T, bool>::value ||
    //std::is_enum<T>::value  >
>::operator()(const JSValue& aValue,
        md::IntervalStops<T> & aResult) const
{ 
    static_assert( !MdU::Interpolatable<T>::value );
    
    if (JsonConvert::isUndefined(&aValue))
    {
        aResult = {};
        return 0;
    }
    else if (JsonConvert::isObject(&aValue))
    {
        T  constant = {};
        int ret = StyleConverter<T>(aValue, constant);
        if (ret < 0) {
            aResult = {};
        }
        else {
            aResult = { constant };
        }
    }
    else {
        std::map<float, T> stops{};
        int ret = convertStops_358<float, T>(aValue, stops);
        if (ret < 0)
        {
            assert(0);
            aResult = {};
            return -1;
        }

        aResult = { stops };
        return 0;
    } 
}
// 
//template<class T>
//template< typename TLimits >
//int  StyleConverter<TestStyleV<T>>::operator()(const JSValue &value,
//                md::ExponentialStops<T>& aResult
//    ) const 
//{
//}
// 
  