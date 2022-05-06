
#ifndef _MAPSTU_RENDER_ENGINE_H__
#define _MAPSTU_RENDER_ENGINE_H__
#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

#include <md/drawengine/transform.hpp>

#include "md/drawengine/drawdevice.h" //IDrawDevice
#include <md/drawengine/transform_state.hpp>

namespace clientmap {
 
//===============================================
/** 渲染场景基础类 */
class CPaintContext;
//class TransformState;// 转换状态类
class GlPainters;
 
 
class DrawSceneBase {
public:
	DrawSceneBase( )
		:  m_bVisible(true) 
	{ }

	virtual ~DrawSceneBase()
	{  
        m_bVisible = false;
    }

	/**
	 * 渲染器监听函数
	 *		本组函数均在显示线程（display）中调用
	 */
public:
	// 重置资源 第一次渲染前必然会调用一次
	virtual void onReloadResource( ) {}


    /* Perform any setup needed before drawing each frame
     * _textUnit is the next available texture unit
     */
    virtual bool onBeginDrawFrame(const GlPainters& aPainter,
                                  const CPaintContext& aDC)
    { 
        return false;
    }
    virtual bool onBeginDrawSelectionFrame(const GlPainters& aPainter, 
                                           const CPaintContext& aDC)
    { 
        return false;
    }
    virtual bool onDrawFrame(const GlPainters& aPainter, 
                             const CPaintContext& aDC)
    { 
        return false;
    }

    /* Perform any unsetup needed after drawing each frame */
    virtual bool onEndDrawFrame(const GlPainters& aPainter,
                                const CPaintContext& aDC)
    {
        return false;
    }
 
public:
	// 判断显示状态
	bool isVisible() const {
		return m_bVisible;
	}

	// 设置显示状态
    void setVisible(bool _b)
    {
        m_bVisible = _b;
    }
  
 
private:
	bool	m_bVisible =false; // 显示开关 
public:
	friend class CDrawEngine;
};
typedef std::shared_ptr<DrawSceneBase>  DrawSceneBaseSP;
typedef std::weak_ptr<DrawSceneBase>    DrawSceneBaseWP;
//typedef std::unique_ptr<DrawSceneBase>  DrawSceneBaseUP;
//===============================================
// 渲染引擎RenderEngine
//===============================================
class CDrawEngineData;// 渲染引擎实际数据 


class CDrawEngine final 
{
    CDrawEngine(const CDrawEngine&);
    const CDrawEngine& operator=(const CDrawEngine&);

public:
    CDrawEngine();

    ~CDrawEngine();
	/** 
	 *	初始化
     *  @param  _drawDevice   主要用于在实际渲染前后处理OpenGL渲染环境的处理
     *  @param  _pixelRatio 
	 *	@param	_assetPath	渲染资源路径
	 *					存放显示引擎需要的图片、字体等基础资源的文件目录
	 *	@param	_localFontFamily	显示层 主要用于在实际渲染前后处理OpenGL渲染
	 */
	bool init(std::shared_ptr<IDrawDevice> _drawDevice,
        double             _pixelRatio 
        );

	// 显示系统结束
	// 释放IRenderListener前*必须*调用此函数
	void done();

	// 重置：销毁资源（字体、纹理、vbo等），重新初始化
	bool restore();
	 

	// 重置资源
	void resetResource(); 

    // 设置OpenGL默认Framebuffer
    void setDefaultFboId(uint32_t);

    void resize(int32_t _w, int32_t _h);
    //------------------
    // 设置首次渲染 首次渲染只渲染背景色 解决黑屏问题
    void setFirstRender();
 
    void requestDrawing();

	/** 
     * 渲染入口函数，该函数运行于显示线程中。
	 * @return 执行显示是否成功。
	 */
	bool drawFrame( );

    // 获取空闲状态
    bool isIdle() const;

public:  
	// 添加场景
	void addScene(DrawSceneBaseSP aScenePtr );
	// 移除场景
	void rmScene(DrawSceneBaseSP aScenePtr);
private:
    void _beginDrawFrame(const CPaintContext& aPC);
    void _drawFrame(const CPaintContext& aPC);
    void _endDrawFrame(const CPaintContext& aPC);
 
public: // 屏幕基本信息设置
	// 屏幕视口尺寸
	void setViewSize(int32_t _w, int32_t _h);
	void getViewSize(int32_t &_w, int32_t &_h);

	// The device pixel ratio for device.  
	// Common values are 1 for normal-dpi displays and 2 for high-dpi
    // "retina" displays.
	// 该属性用于保证在任何屏幕上显示效果尽可能一致
	void setScreenPixelRatio(double _pxRatio); 
public: 
    //----------------
    // 获取相机参数.本组函数仅在非渲染场合使用。
    //----------------
	// 中心点坐标 单位：经纬度坐标
	void getMapCenter(double& _lon, double& _lat) const;
 
	// 缩放级别(Zoom Level) 无级缩放时取小于当前缩放比例的最大缩放级别.
	int32_t getZoom() const;

	// 缩放比例
	double getScale() const;

	// 旋转角度(RotateZ): 与正北夹角 角度单位 [0, 360)
	double getBearing() const;

	// 倾斜角度(rotate X) 视线垂直地图为0度 角度单位
	double getPitch() const;
 
public:
    const TransformState& getTransState4Draw()const
    {
        return mTransState4Draw;
    }
	 
public://------gesture operation------
    // Transition
    void cancelTransitions();
    void setGestureInProgress(bool);
    bool isGestureInProgress() const;
    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;

    // Camera
    CameraOptions getCameraOptions( ) const;

    void jumpTo(const CameraOptions&);
    void easeTo(const CameraOptions&, const AnimationOptions&);
    void flyTo(const CameraOptions&,
        const AnimationOptions&);


    void moveBy(const  ScreenPt&, const AnimationOptions& = {});
    void scaleBy(double scale, 
            const  ScreenPt& aAnchor,
            const AnimationOptions& aAnim = {});
    void zoomBy(double aDiffZ, const AnimationOptions& aAnim= {});
    void pitchBy(double pitch, const AnimationOptions& aAnim = {});
    void rotateBy(const  ScreenPt& first,
            const  ScreenPt& second,
            const AnimationOptions& = {}
    );
    void rotateBy_deg(const  double aDegDiff, const AnimationOptions& = {});

private:	
    
    // 初始数据
    std::weak_ptr<IDrawDevice> m_drawDevice;
	 
    // mTransState4Draw is asigned only when renderring thread begin. 
    // m_transCS is used to protect mTransState, not
    // to protect mTransState4Draw.
    // mTransState4Draw be equal to an temprary local variable,
    // not needs to be protected by CS. 
    bool            mCameraMutated = false;
    std::mutex      m_transCS;
    Transform       mTrans;
    TransformState  mTransState; // in not-renderring thread
    TransformState  mTransState4Draw;// in renderring thread    
    std::unique_ptr<GlPainters>  mPainters;// in renderring thread
    
    // discard redundant draw calls via the `renderQueued` flag.
    std::atomic_flag m_renderQueued = ATOMIC_FLAG_INIT;
    //
    std::mutex                    m_sceneArrCS;
    std::vector<DrawSceneBaseSP > m_sceneArr;// weak_ptr array
};


}  // namespace clientmap

#endif // #ifndef _MAPSTU_RENDER_ENGINE_H__
