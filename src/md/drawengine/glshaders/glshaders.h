#pragma once
#include <string>
#include <vector>
#include <memory>
 
#include "md/drawengine/glshaders/glshd_uc.h"
#include "md/drawengine/glshaders/glshd_tex_uc.h"
#include "md/drawengine/glshaders/glshd_alpha_tex_uc.h"
#include "md/drawengine/glshaders/glshd_line.h"
#include "md/drawengine/glshaders/glshd_thinline.h"
  
namespace clientmap {
  
class GlShaders final{
public:
    GlShaders(/*GLContext&*/);
    ~GlShaders();

    GlShdUColor&    getUColorShader() noexcept;
    GlShdTexUColor&      getTexUCShader() noexcept;
	GlShdAlphaTexUColor& getATexUCShader() noexcept;
    GlShdLine&      getLineShader() noexcept; 
    GlShdThinLine&  getThinLineShader() noexcept;

    //DebugProgram debug;
    //ClippingMaskProgram clippingMask;

private:
    std::unique_ptr<GlShaderBase> mUColorShader;
    std::unique_ptr<GlShaderBase> mTexUCShader;
	std::unique_ptr<GlShaderBase> mATexUCShader;
    std::unique_ptr<GlShaderBase> mLineShader; 
    std::unique_ptr<GlShaderBase> mThinLineShader;
    //GLContext& context;
    //ProgramParameters programParameters;
};

} // namespace clientmap

 