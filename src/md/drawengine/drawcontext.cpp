#include <assert.h>

#include "platformbase/gl_interface.h" 
#include <md/drawengine/transform_state.hpp>

#include "md/drawengine/glshaders/glshaders.h"
#include "md/drawengine/rscmgr/gltexmgr.h"

#include "md/drawengine/drawcontext.h"


namespace clientmap {
//////////////////////////////////////////////////////////
CPaintContext::CPaintContext(const TransformState&  aState,
    TimePoint              aTmPoint 
):    _transState(aState) 
    ,_timePoint(aTmPoint)
{


}

CPaintContext::~CPaintContext() = default;  
//------------------------------------------------------
}// namespace clientmap 

 