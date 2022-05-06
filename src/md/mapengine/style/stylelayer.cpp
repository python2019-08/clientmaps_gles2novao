#include <md/util/logtool.hpp>
#include <md/util/cpp_misc.hpp>

#include <md/mapengine/style/stylelayer.hpp>
 
namespace clientmap { 
   
StyleLayer::StyleLayer( const std::string& aLayerId, 
                        const std::string& aDsLayerId)
    :mId(aLayerId), mDsLayerId(aDsLayerId)
{
    
}

StyleLayer::~StyleLayer() = default;

const std::string& StyleLayer::getID() const
{
    return mId;
}

const StyleLayerType StyleLayer::getType() const
{
    return mType;
}

const std::string& StyleLayer::getDsLayerID() const
{
    return mDsLayerId;
}

bool StyleLayer::isVisible() const
{
    return mIsVisible;
}
void StyleLayer::setVisibility(bool _b)
{
    mIsVisible = _b;
}

float StyleLayer::getMinZoom() const 
{
    return  mMinZoom;
}

float StyleLayer::getMaxZoom() const 
{
    return mMaxZoom;
}

void StyleLayer::setMinZoom(float minZoom) 
{
    if (mMinZoom == minZoom)
        return; 
    mMinZoom = minZoom;
}

void StyleLayer::setMaxZoom(float maxZoom) 
{
    if (mMaxZoom == maxZoom) return;
 
    mMaxZoom = maxZoom; 
}


const std::string& StyleLayer::typeName(StyleLayerType aType)
{
    static std::unique_ptr < std::vector<std::string> > s_typeNames;
    if (!s_typeNames)
    {
        s_typeNames = std::make_unique< std::vector<std::string>>();
        s_typeNames->resize((std::size_t)(StyleLayerType::LayerTypeCnt));

        std::vector<std::string>& arr = *s_typeNames;
        arr[(int)(StyleLayerType::background)] = "background";
        arr[(int)(StyleLayerType::raster)] = "raster";
        arr[(int)(StyleLayerType::face)] = "fill";
        arr[(int)(StyleLayerType::building)] = "building";
        arr[(int)(StyleLayerType::line)] = "line";
        arr[(int)(StyleLayerType::symbol)] = "symbol";
    }
    assert(aType > StyleLayerType::unknown &&
        aType < StyleLayerType::LayerTypeCnt
    );

    return s_typeNames->at((std::size_t)aType);
}
 
} // namespace clientmap
