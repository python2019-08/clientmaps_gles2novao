#include <algorithm>
#include <set>
#include <sstream>
#include <fstream>
 
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <md/util/math_tools.hpp>
#include <md/util/logtool.hpp>
#include <md/util/file_io.hpp>

#include <md/mapengine/style/style_value.hpp>



#include "md/mapengine/style/style4map.hpp"  
namespace clientmap {
Style4Map::~Style4Map() = default;


ShStyle4Map Style4Map::loadStr(const std::string& aStyleRawData)
{
	ShStyle4Map  pMStyle;
	do
	{ 
		if (aStyleRawData.size() < 10)
		{
			break;
		}

		ShStyle4Map pMStyleTmp = std::make_shared<Style4Map>();
		if (!pMStyleTmp)
		{
			break;
		}

		int iRet = pMStyleTmp->parse(aStyleRawData);
		if (iRet < 0)
		{
			break;
		}
		pMStyle = pMStyleTmp;
	} while (false);

	return pMStyle;
}

ShStyle4Map Style4Map::loadLocalStyle(const std::string& aStylePath)
{
	ShStyle4Map  pMStyle;
	do
	{
		std::string  styleRawData;
		int iRet = readFile(aStylePath, styleRawData);
		if (iRet <0 || styleRawData.size() <10)
		{

			break;
		}

		ShStyle4Map pMStyleTmp = std::make_shared<Style4Map>();
		if (!pMStyleTmp)
		{
			break;
		}

		iRet = pMStyleTmp->parse(styleRawData);
		if (iRet < 0)
		{
			break;
		}
		pMStyle = pMStyleTmp;
	} while (false);

    return pMStyle;
}

ShStyle4Map Style4Map::loadURL(const std::string& aUrl)
{
	ShStyle4Map  pMStyle;
#if 0
    if (!fileSource) {
        observer->onStyleError(
            std::make_exception_ptr(
                util::StyleLoadException("Unable to find resource provider for style url.")
            )
        );
        return;
    }

    lastError = nullptr;
    observer->onStyleLoading();

    loaded = false;
    url = url_;

    styleRequest = fileSource->request(Resource::style(aUrl),
        [this](const Response& res)
    {
        // Don't allow a loaded, mutated style to be overwritten with a new version.
        if (mutated && loaded) {
            return;
        }

        if (res.error)
        {
            const std::string message = "loading style failed: " + res.error->message;
            Log::Error(Event::Setup, message.c_str());
            observer->onStyleError(std::make_exception_ptr(util::StyleLoadException(message)));
            observer->onResourceError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        }
        else if (res.notModified || res.noContent) {
            return;
        }
        else {
            parse(*res.data);
        }
    });
#endif
	return pMStyle;
}

int Style4Map::parse(const std::string& json)
{
    rapidjson::GenericDocument<rapidjson::UTF8<>,
        rapidjson::CrtAllocator> document;
    document.Parse<0>(json.c_str());

    if (document.HasParseError())
    {
        std::string errStr = formatJSONParseError(document);
        Log::Error(errStr.c_str());
        assert(0);
        return -1;
    }

    if (!document.IsObject())
    {
        Log::Error("style must be an object" );
        return  -2;
    }

    if (document.HasMember("version"))
    {
        const JSValue& versionValue = document["version"];
        const int version = versionValue.IsNumber() ? versionValue.GetInt() : 0;
        if (version != 8)
        {
            Log::Warning(
                "current renderer implementation only supports style spec version 8; "
                "using an outdated style will cause rendering errors"
            );
        }
    }

    if (document.HasMember("name"))
    {
        const  JSValue& value = document["name"];
        if (value.IsString())
        {
            _name = { value.GetString(), value.GetStringLength() };
        }
    }

    if (document.HasMember("center"))
    {
        const  JSValue& value = document["center"];
        LonLat convertedLatLng = {};
        int ret = parseStyleConst(value, convertedLatLng);
        if (ret >= 0)
        {
            _lngLat = convertedLatLng;
        }
        else {
             Log::Warning( 
                "center coordinate must be a longitude, latitude pair");
        }
    }

    if (document.HasMember("zoom"))
    {
        const  JSValue& value = document["zoom"];
        if (value.IsNumber()) {
            _zoom = value.GetDouble();
        }
    }

    if (document.HasMember("bearing"))
    {
        const  JSValue& value = document["bearing"];
        if (value.IsNumber()) {
            _bearing = value.GetDouble();
        }
    }

    if (document.HasMember("pitch"))
    {
        const  JSValue& value = document["pitch"];
        if (value.IsNumber()) {
            _pitch = value.GetDouble();
        }
    }

    if (document.HasMember("layers"))
    {
        parseLayers(document["layers"]);
    }

    if (document.HasMember("sprite"))
    {
        const  JSValue& sprite = document["sprite"];
        if (sprite.IsString()) {
            _spriteURL = { sprite.GetString(), sprite.GetStringLength() };
        }
    }

    if (document.HasMember("glyphs"))
    {
        const  JSValue& glyphs = document["glyphs"];
        if (glyphs.IsString()) {
            _glyphURL = { glyphs.GetString(), glyphs.GetStringLength() };
        }
    }

    // Call for side effect of logging warnings for invalid values.
    //fontStacks();

    return 0;
}

void
Style4Map::parseLayers(const JSValue& value)
{
    std::vector<std::string> ids;

    if (!value.IsArray()) {
        Log::Warning( "Style layers must be an array");
        return;
    }

    for (auto& layerValue : value.GetArray())
    {
        if (!layerValue.IsObject()) {
            Log::Warning( "Style layer must be an object");
            continue;
        }

        if (!layerValue.HasMember("id")) {
            Log::Warning( "Style layer must have an id");
            continue;
        }

        const JSValue& id = layerValue["id"];
        if (!id.IsString()) {
            Log::Warning( "Style layer id must be a string");
            continue;
        }

        const std::string layerID = { id.GetString(),
                                      id.GetStringLength() };
        if (layersMap.find(layerID) != layersMap.end()) {
            Log::Warning( "duplicate Style layer id %s", layerID.c_str());
            continue;
        }

        layersMap.emplace(layerID, 
            JSVal_StyleLayer_Pair{ layerValue, nullptr });
        ids.push_back(layerID);
    }

    for (const auto& id : ids) 
    {
        auto it = layersMap.find(id);

        parseLayer(it->first, 
            it->second.first, it->second.second);
    }

    for (const auto& id : ids)
    {
        auto it = layersMap.find(id);

        if (it->second.second) 
        {
            auto& layer = *(it->second.second);
            // 
            if (StyleLayerType::background == layer.getType())
            {
                ShStyleLayer SL = std::move(it->second.second);
                ShBgStyleLayer bgLayer= std::dynamic_pointer_cast<BackgroundStyle>(SL);
                _bgLayer = std::move(bgLayer);
            }
            else if (StyleLayerType::raster == layer.getType())
            {
                ShStyleLayer SL = std::move(it->second.second);
                ShRasterStyleLayer rasLayer =
                    std::dynamic_pointer_cast<RasterStyle>(SL);
                _rasterLayer = std::move(rasLayer);
            }
            else {
                _vectLayers.emplace_back(std::move(it->second.second));
            }            
        }
    }

    if (!_bgLayer) {
        _bgLayer = std::make_shared<BackgroundStyle>("bg123", "background");
    }
}

int
Style4Map::parseLayer(const std::string& id,
    const JSValue& value,
    std::unique_ptr<StyleLayer>& aLayer)
{
    if (aLayer) {
        // Skip parsing this again. We already have a valid layer definition.
        return -1;
    }

    // Make sure we have not previously attempted to parse this layer.
    if (std::find(stack.begin(), stack.end(), id) != stack.end())
    {
        Log::Warning( "layer reference of '%s' is circular", id.c_str());
        return -2;
    }
    //------------------------------------
    // ½âÎöstyleLayer
    int ret = 0;
    if (!value.IsObject() ) {
        std::string err = "layer must be an object";
        return -3;
    }
    //----id
    if (!value.HasMember("id")) {
        std::string err = "layer must have an id";
        return -4;
    }
    const JSValue& idValue = value["id"]; 

    if (!idValue.IsString()) {
        std::string err = "layer id must be a string";
        return -1;//error
    }
    std::string Layerid= { idValue.GetString(), 
                           idValue.GetStringLength() };
    //----type 
    auto typeValue = JsonTool::objectMember(&value, "type");
    if (!typeValue) {
        std::string err = "layer must have a type";
        return -4;
    }

    std::string type;
    ret = parseStyleConst(*typeValue, type);
    if (ret < 0) {
        std::string err = "layer type must be a string";
        return -5;
    }
    //----ds-layerid

    auto dsLIdValue = JsonTool::objectMember(&value, "ds-layerid");
    if (!dsLIdValue) {
        std::string err = "layer must have a ds-layerid";
        return -4;
    }

    std::string dsLId;
    ret = parseStyleConst(*dsLIdValue, dsLId);
    if (ret < 0) {
        std::string err = "ds-layerid must be a string";
        return -5;
    }
    //----
    std::unique_ptr<StyleLayer> layer = createStyleLayer(id, dsLId, type);
    if (!layer)
        return -6;
    //----
    auto visibleValue = JsonTool::objectMember(&value, "is-visible");
    if (visibleValue) {
        bool isVisible = true;
        ret = parseStyleConst(*visibleValue, isVisible);
        if (ret >= 0)
        { 
            layer->setVisibility( isVisible );
        }
    }
    //----
    auto minZValue = JsonTool::objectMember(&value, "minzoom");
    if (minZValue) {
        float minZ = 0;
        ret = parseStyleConst(*minZValue, minZ);
        if (ret >= 0)
        {
            minZ = cm_clamp<float>(minZ, 0, MAX_ZOOM);
            layer->setMinZoom(minZ);
        }
    } 
    //----
    auto maxZValue = JsonTool::objectMember(&value, "maxzoom");
    if (maxZValue) {
        float maxZ = 0;
        ret = parseStyleConst(*maxZValue, maxZ);
        if (ret >= 0)
        {
            maxZ = cm_clamp<float>(maxZ,  0, MAX_ZOOM);
            layer->setMaxZoom(maxZ);
        }
    }

    auto paintValue = JsonTool::objectMember(&value, "paint");
    if (paintValue)
    {
        if (!JsonTool::isObject(paintValue)) {
            std::string err = "layout must be an object";
            return -10;
        }

        ret = layer->parse( *paintValue );
        if (ret < 0) {
            return ret;
        }
    } 
    aLayer = std::move(layer);
    return 0;
}

std::unique_ptr< StyleLayer>
Style4Map::createStyleLayer(const std::string& id,
    const std::string& aDsLayerId,
    const std::string& aType) noexcept
{
    std::unique_ptr<StyleLayer> layer = nullptr;
    if (id == "building") {
        Log::Debug("id == \"building\"\n");
    }

    if (aType == StyleLayer::typeName(StyleLayerType::background)) {
        layer = std::make_unique<BackgroundStyle>(id, aDsLayerId);
    } 
    else if (aType == StyleLayer::typeName(StyleLayerType::symbol)) {
        layer = std::make_unique<SymbolStyle>(id, aDsLayerId);
    } 
    else if (aType == StyleLayer::typeName(StyleLayerType::face) ) { 
        layer = std::make_unique<FaceStyle>(id, aDsLayerId);
    } 
    else if (aType == StyleLayer::typeName(StyleLayerType::building)) {       
        layer = std::make_unique<BuildingStyle>(id, aDsLayerId);
    }
    else if (aType == StyleLayer::typeName(StyleLayerType::line)) {     
        layer = std::make_unique<LineStyle>(id, aDsLayerId);
    }
    else if (aType == StyleLayer::typeName(StyleLayerType::raster)) {       
        layer = std::make_unique<RasterStyle>(id, aDsLayerId);
    }
    else {
        std::string errMsg = "invalid layer type";
    }

    return layer;
}
//
//std::set<md::FontStack>
//Style4Map::fontStacks() const
//{
//    std::vector< md::Immutable<md::StyleLayer::Impl> > impls;
//    impls.reserve(_layers.size());
//    for (const auto& layer : _layers)
//    {
//        impls.emplace_back(layer->baseImpl);
//    }
//    return md::fontStacks(impls);
//}

}// namespace clientmap {