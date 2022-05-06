//#include "glcanvas/renderState.h"
#include <md/util/logtool.hpp>



#include "md/drawengine/glshaders/glshaders.h"
 
namespace clientmap {

GlShaders::GlShaders()
{
     
}

GlShaders::~GlShaders()
{ 
     
}

GlShdUColor&    GlShaders::getUColorShader() noexcept
{
    if (!mUColorShader) {
        mUColorShader = std::make_unique<GlShdUColor>(
            /*context, programParameters*/
            );
    }

    return static_cast<GlShdUColor&>(*mUColorShader);
}

GlShdTexUColor& GlShaders::getTexUCShader() noexcept
{
    if (!mTexUCShader) {
        mTexUCShader = std::make_unique<GlShdTexUColor>(
            /*context, programParameters*/
            );
    }

    return static_cast<GlShdTexUColor&>(*mTexUCShader);
}


GlShdAlphaTexUColor& GlShaders::getATexUCShader() noexcept
{
	if (!mATexUCShader) {
		mATexUCShader = std::make_unique<GlShdAlphaTexUColor>(
			/*context, programParameters*/
			);
	}

	return static_cast<GlShdAlphaTexUColor&>(*mATexUCShader);
}

GlShdLine&      GlShaders::getLineShader() noexcept
{
    if (!mLineShader) {
        mLineShader = std::make_unique<GlShdLine>(
            /*context, programParameters*/
            );
    }

    return static_cast<GlShdLine&>(*mLineShader);
}

GlShdThinLine& GlShaders::getThinLineShader() noexcept
{
    if (!mThinLineShader) {
        mThinLineShader = std::make_unique<GlShdThinLine>(
            /*context, programParameters*/
            );
    }

    return static_cast<GlShdThinLine&>(*mThinLineShader);
}

}// namespace clientmap {
