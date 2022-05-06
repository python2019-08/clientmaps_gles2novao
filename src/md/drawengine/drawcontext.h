#pragma once
#ifndef _MAPSTU_CDrawContext_H__
#define _MAPSTU_CDrawContext_H__
#include <cstdint>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

#include "md/util/chrono.hpp" 
#include <md/drawengine/transform_state.hpp>


namespace clientmap {
//class TransformState;// 转换状态类
class GlShaders;
class GlTexMgr;
class GlPainters;

class CPaintContext
{
public:
    CPaintContext(const TransformState&  aState,
        TimePoint              aTmPoint
    );
    ~CPaintContext();

    const TransformState&  _transState;
    TimePoint              _timePoint;
};
   
}  // namespace clientmap

#endif // !_MAPSTU_CDrawContext_H__ 
