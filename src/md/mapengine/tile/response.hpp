#pragma once
 
#include <string>
#include <memory>
#include <md/util/chrono.hpp> 
#include "md/util/tile_id.hpp"

namespace clientmap {
 
enum class ETileType : uint8_t
{
    eVector = 0,
    eRaster,
};

using ShVoid = std::shared_ptr<void>;

class CTileResponse {
public:
    CTileResponse( TypedTileId aTTId, uint16_t  aErr, ShVoid aTileData);
    CTileResponse(const CTileResponse&);
    CTileResponse& operator=(const CTileResponse&);

public:
    enum class ErrorCode : uint16_t {
        Success = 200,//  OK
        NoContent =204,
        NotModified = 304,
        NotFound = 404,//404         
        Connection = 4,
        Server = 5,
        RateLimit = 6,
        Other = 7,
    };
    // 
    ErrorCode _error = ErrorCode::Other;     

    // This is set to true when the server requested 
    // that no expired resources be used by
    // specifying "Cache-Control: must-revalidate".
    bool _mustRevalidate = false;

    TypedTileId   _TTId{ 0,0,0,0 };

    // The actual data of the response. 
    // if _error is 204 or 404, nullptr.
    // Present only for non-error, non-notModified responses.
    ShVoid       _data = nullptr;

    int64_t  _modified =-1;// modified timePoint
    int64_t  _expires =-1;// expiration timePoint
    std::string  _etag;

    bool isFresh() const {
        return _expires > 0 
            ? (_expires > currentMillisec() ) 
            : (_error == ErrorCode::Success);
    }

    // Indicates whether we are allowed to use this response
    // according to HTTP caching rules.
    // It may or may not be stale.
    bool isUsable() const {
        return !_mustRevalidate || 
            (_expires > 0 && (_expires > currentMillisec()) );
    }
}; 

using ShTileResponse = std::shared_ptr<CTileResponse >;
//------------------------------------------------------
} // namespace clientmap
