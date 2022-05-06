#pragma once
#include <array>
#include <string>
#include <vector>
#include <map>
 
 
namespace clientmap {
class Marker
{
public:
    // Create an empty marker with the given ID. An ID of 0 indicates an invalid marker.
    Marker(uint32_t id);

    ~Marker();
private:
    uint32_t m_id;
};
}//namespace clientmap 