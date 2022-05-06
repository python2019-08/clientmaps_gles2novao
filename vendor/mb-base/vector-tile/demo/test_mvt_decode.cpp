#include <mapbox/vector_tile.hpp>
#include <fstream>
#include <stdexcept>
#include <iostream>

std::string open_tile(std::string const& path) 
{
    std::ifstream stream(path.c_str(),std::ios_base::in|std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + path + "'");
    }
    std::string message(  std::istreambuf_iterator<char>(stream.rdbuf()),
                         (std::istreambuf_iterator<char>()));
    stream.close();
    return message;
}

class print_value {

public:
    std::string operator()(std::vector<mapbox::feature::value> val) {
        return "vector";
    }

    std::string operator()(std::unordered_map<std::string, mapbox::feature::value> val) {
        return "unordered_map";
    }

    std::string operator()(mapbox::feature::null_value_t val) {
        return "null";
    }

    std::string operator()(std::nullptr_t val) {
        return "nullptr";
    }

    std::string operator()(uint64_t val) {
        return std::to_string(val);
    }
    std::string operator()(int64_t val) {
        return std::to_string(val);
    }
    std::string operator()(double val) {
        return std::to_string(val);
    }
    std::string operator()(std::string const& val) {
        return val;
    }

    std::string operator()(bool val) {
        return val ? "true" : "false";
    }
};



void parseMvt(const char*  aTilePath,
    int aTileIdx,
    int aTileIdy,
    int aTileIdz)
{
    std::string tile_path( aTilePath );
    std::string buffer = open_tile(tile_path);
    mapbox::vector_tile::buffer tile(buffer);
    std::cout << "Decoding tile: " << tile_path << "\n";
     
    //---------gmq,start
    const double M_PI = 3.14159265;
    const double util_EXTENT = 4096;
    const double worldSize = util_EXTENT * std::pow(2, aTileIdz);
    const double x0 = util_EXTENT * static_cast<double>(aTileIdx);
    const double y0 = util_EXTENT * static_cast<double>(aTileIdy);

    auto tileCoordinatesToLatLng = [&](const mapbox::geometry::point<int16_t>& p)
    {
        double x2 = (p.x + x0) * 360 / worldSize - 180;
        double y2 = 180 - (p.y + y0) * 360 / worldSize;
        return mapbox::geometry::point<double>(
            x2,
            360.0 / M_PI * std::atan(std::exp(y2 * M_PI / 180)) - 90.0
            );
    };
    //===========gmq,end

    for (auto const& name : tile.layerNames())
    {
        const mapbox::vector_tile::layer layer = tile.getLayer(name);
        std::size_t feature_count = layer.featureCount();
        if (feature_count == 0) {
            std::cout << "Layer '" << name << "' (empty)\n";
            continue;
        }

        std::cout << "Layer '" << name << "'\n";
        std::cout << "  Features:\n";

        for (std::size_t i = 0; i < feature_count; ++i)
        {
            auto const feature = mapbox::vector_tile::feature(layer.getFeature(i), layer);

            auto const& feature_id = feature.getID();
            if (feature_id.is<uint64_t>())
            {
                std::cout << "    id: " << feature_id.get<uint64_t>() << "\n";
            }
            else {
                std::cout << "    id: (no id set)\n";
            }

            std::cout << "    type: " << int(feature.getType()) << "\n";


            auto props = feature.getProperties();
            std::cout << "    Properties:\n";
            for (auto const& prop : props)
            {
                print_value printvisitor;
                std::string value = mapbox::util::apply_visitor(printvisitor, prop.second);
                std::cout << "      " << prop.first << ": " << value << "\n";
            }


            std::cout << "    Vertices:\n";
            const float scale_extent = float(util_EXTENT) / feature.getExtent();//mycode
            mapbox::vector_tile::points_arrays_type geom =
                feature.getGeometries<mapbox::vector_tile::points_arrays_type>(scale_extent);//mycode
                //feature.getGeometries<mapbox::vector_tile::points_arrays_type>(1.0);//raw code
            for (auto const& point_array : geom)
            {
                for (auto const& point : point_array)
                {
                    //std::clog << "(" << point.x << "," << point.y << ")";
                    mapbox::geometry::point<double > lonlat= tileCoordinatesToLatLng(point);
                    std::clog << "(" << lonlat.x << "," << lonlat.y << ")";
                }
            }
            std::clog << "\n";
        }
    }
}
 
int main(int argc, char** argv) 
{
    try {
        //if (argc < 2) {
        //    std::clog << "please provide path to uncompressed Mapbox Vector tile (.mvt)\n";
        //    return -1;
        //}
        const char* path = R"(C:\_abel\_stu\map\mapbox-data-example\mapbox-geojson-pbf\2-0-3.vector.pbf)";

        parseMvt( path,0,3, 2);
 
    } catch (std::exception const& ex) {
        std::clog << "ERROR: " << ex.what() << "\n";
        return -1;
    }
}
 