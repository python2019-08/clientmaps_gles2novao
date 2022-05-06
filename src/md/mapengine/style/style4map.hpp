#ifndef STYLE_4_MAP_HPP_INCLUDED
#define STYLE_4_MAP_HPP_INCLUDED

#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <forward_list>
#include <md/util/geo.hpp> 

#include <md/util/constants.hpp>
#include <md/util/rapidjson.hpp>

#include <md/mapengine/style/stylelayer.hpp>
#include <md/mapengine/style/stylelayer_background.hpp>
#include <md/mapengine/style/stylelayer_raster.hpp>
#include <md/mapengine/style/stylelayer_face.hpp>
#include <md/mapengine/style/stylelayer_building.hpp>
#include <md/mapengine/style/stylelayer_line.hpp>
#include <md/mapengine/style/stylelayer_symbol.hpp> 
 
namespace clientmap {
class StyleLayer;
class BackgroundStyle;
class RasterStyle;

class Style4Map;
using ShStyle4Map = std::shared_ptr<Style4Map >;

class Style4Map {
public:
    Style4Map() {
    }

    ~Style4Map();
	static ShStyle4Map loadStr(const std::string& aStyleRawData);

    static ShStyle4Map loadLocalStyle(const std::string& aStylePath );
	static ShStyle4Map loadURL(const std::string& aUrl);
	

    std::string _spriteURL;
    std::string _glyphURL;
     
    std::shared_ptr<BackgroundStyle>   _bgLayer;
    std::shared_ptr<RasterStyle>  _rasterLayer;
    std::vector<ShStyleLayer>          _vectLayers;
 
    std::string  _name;
    LonLat  _lngLat = { 116.333333, 39.93333333 };
    double  _zoom = 15.5;
    double  _bearing = 0;
    double  _pitch = 0;

    //std::set<md::FontStack> fontStacks() const;

private: 
	int parse(const std::string&);

    void parseLayers(const JSValue&);
    int parseLayer(const std::string& id, 
	                const JSValue&,
					std::unique_ptr<StyleLayer>&);

    std::unique_ptr< StyleLayer>
    createStyleLayer(const std::string& id,
        const std::string& aDsLayerId,
        const std::string& aType)noexcept;
private:
    typedef std::pair<const JSValue&, std::unique_ptr<StyleLayer>>
        JSVal_StyleLayer_Pair;
    std::unordered_map<std::string,JSVal_StyleLayer_Pair>  layersMap;

    // Store a stack of layer IDs we're parsing right now. 
    // This is to prevent reference cycles.
    std::forward_list<std::string> stack;
};
 
}// namespace clientmap 
#endif // !STYLE_4_MAP_HPP_INCLUDED