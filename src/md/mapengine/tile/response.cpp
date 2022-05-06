
#include <md/mapengine/tile/response.hpp>

namespace clientmap {
CTileResponse::CTileResponse(
    TypedTileId   aTTId,
    uint16_t  aErrCode,
    ShVoid    aShTileData)
    :_TTId(aTTId), _error((ErrorCode)aErrCode),
    _data(aShTileData)
{

}
CTileResponse::CTileResponse(const CTileResponse& res) 
{
    *this = res;
}

CTileResponse& CTileResponse::operator=(const CTileResponse& res)
{
    _error = res._error ;
    _mustRevalidate = res._mustRevalidate;

    _data = res._data;
    _modified = res._modified;
    _expires = res._expires;
    _etag = res._etag;
    return *this;
}
 

} // namespace clientmap
