
#include <md/mapengine/style/rapidjson_conversion.hpp>
 
#include <md/mapengine/style/layers/stylelayer_background.hpp>
#include <md/mapengine/style/layers/stylelayer_circle.hpp>
#include <md/mapengine/style/layers/stylelayer_fill.hpp>
#include <md/mapengine/style/layers/stylelayer_fill_extrusion.hpp>
#include <md/mapengine/style/layers/stylelayer_heatmap.hpp>
#include <md/mapengine/style/layers/stylelayer_hillshade.hpp>
#include <md/mapengine/style/layers/stylelayer_line.hpp>
#include <md/mapengine/style/layers/stylelayer_raster.hpp>
#include <md/mapengine/style/layers/stylelayer_symbol.hpp>
#include <md/mapengine/style/layers/stylelayer_location_indicator.hpp>

#include <md/mapengine/style/style_converter.hpp>

 
namespace { //  ÄäÃû¿Õ¼ä 
int 
setObjectMember(std::unique_ptr<md::StyleLayer>& layer,
    const JSValue& value,
    const char* member )
{
    int ret = 0;     
    auto memberValue = JsonConvert::objectMember(&value, member);
    if (memberValue)
    {
        ret = layer->setProperty(member, *memberValue);
    }
    return ret; 
}
} // namespace
 
//------------------------------
//template <class LayerType>
//std::unique_ptr<StyleLayer> convertVectorLayer(const std::string& id,
//        const JSValue& value)
//{   
//    // (void)value;
//    std::unique_ptr<LayerType> layer = std::make_unique<LayerType>(id );   
//    return  std::move(layer);
//    //return  std::make_unique<LayerType>(id);
//}
//
std::unique_ptr<md::StyleLayer>
createStyleLayer(const std::string& id, 
                 const std::string& aType) noexcept
{
    
    std::unique_ptr<md::StyleLayer> layer;
    if (aType == "fill") {// VectorLayer
        //layer = std::unique_ptr<md::StyleLayer>(new md::FillLayer(id));
        layer = std::make_unique<md::FillLayer>(id);
    }
    else if (aType == "fill-extrusion") {// VectorLayer         
        layer = std::make_unique<md::FillExtrusionLayer>(id);
    }
    else if (aType == "line") {// VectorLayer        
        layer = std::make_unique<md::LineLayer>(id);
    }
    else if (aType == "circle") {// VectorLayer
        layer = std::make_unique<md::CircleLayer>(id);
    }
    else if (aType == "symbol") {// VectorLayer
        layer = std::make_unique<md::SymbolLayer>(id);
    }
    else if (aType == "raster") {//RasterLayer
        layer = std::make_unique<md::RasterLayer>(id);
    }
    else if (aType == "heatmap") {// VectorLayer
        layer = std::make_unique<md::HeatmapLayer>(id);
    }
    else if (aType == "hillshade") {// HillshadeLayer
        layer = std::make_unique<md::HillshadeLayer>(id);
    }
    else if (aType == "location-indicator") {
        layer = std::make_unique<md::LocationIndicatorLayer>(id);
    }
    else if (aType == "background") {//BackgroundLayer
        layer = std::make_unique<md::BackgroundLayer>(id);
    }
    else {
        std::string errMsg = "invalid layer type"; 
    }  
 
    return layer;
}
//------------------------------ 
int
StyleConverter<std::unique_ptr<md::StyleLayer>>::operator()(const JSValue& value,
    std::unique_ptr<md::StyleLayer>& aLayer
    ) const
{
    int ret = 0;
    if (!JsonConvert::isObject(&value)) {
        std::string err = "layer must be an object";
        return -1;
    }
    //----id
    auto idValue = JsonConvert::objectMember(&value, "id");
    if (!idValue) {
        std::string err = "layer must have an id";
        return -2;
    }

    std::string id;
    ret = JsonConvert::toString(idValue,id);
    if (ret <0) {
        std::string err = "layer id must be a string";
        return -3;
    }
    //----type
    auto typeValue = JsonConvert::objectMember(&value, "type");
    if (!typeValue) {
        std::string err = "layer must have a type";
        return -4;
    }

    std::string type;
    ret = JsonConvert::toString(typeValue, type);
    if (ret <0) {
        std::string err = "layer type must be a string";
        return -5;
    }
    //----
    std::unique_ptr<md::StyleLayer> layer = createStyleLayer(id, type);
    if (!layer)
        return -6;

    //----
    if (setObjectMember(layer, value, "minzoom" )<0)
        return -7;
    if (setObjectMember(layer, value, "maxzoom") <0)
        return -8;

    auto layoutValue = JsonConvert::objectMember(&value, "layout");
    if (layoutValue)
    {
        if (!JsonConvert::isObject(layoutValue)) {
            std::string err = "layout must be an object";
            return -10;
        }

        ret = JsonConvert::eachMember( layoutValue,
            [&](const std::string& k, const JSValue& v)
            { return layer->setProperty(k, v); }
        );
        if (ret <0) {             
            return ret;
        }
    }

    ret = setPaintProperties(*layer, value);
    if (ret <0) { 
        return ret;
    }

    aLayer = std::move(layer);
    return 0;
}
 