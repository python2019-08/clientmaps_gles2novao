#pragma once

#include <cstdint>

#include <mutex>
#include <memory>

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QPointF>
  
#include "md/drawengine/drawdevice.h"
namespace clientmap {
class CMDMap;
}

class QMapGL4MdMap 
    : public QObject 
    , public clientmap::IDrawDevice
    , public std::enable_shared_from_this<QMapGL4MdMap>
{
    Q_OBJECT

public:
    explicit QMapGL4MdMap(QObject* parent = 0);
    virtual ~QMapGL4MdMap();

    int init( double      _pixelRatio );
    void done();

    void setStyle(const std::string& aStyleName);

    void setDefaultFboId(uint32_t _fboId);
    void resize(int32_t _w, int32_t _h);

    void jumpTo(double aLon, double aLat, 
        double aZoom =13, 
        double aRollAngle=0, 
        double aPitchAngle=0);
    void returnToDefaultPos( );
  
    void moveBy(const QPointF &offset);
    void scaleBy(double scale, const QPointF& aAnchor);
    void zoomBy(double aDeltaZ );
    void rotateBy(const QPointF &first, const QPointF &second);
    void rotateBy_deg(double aDegDiff);
    void pitchBy(double pitch);
    

    // 实现 接口md::IRenderDevice的虚函数
    bool activate() override {
        return true;
    }
    void deactivate()override {}
    void triggerDraw()override;
  
    //----
    void drawFrame();
signals:
    void needsRendering();

private:
    //Q_DISABLE_COPY(QMapGL4MdMap)  ;
    
    // 
    std::unique_ptr<clientmap::CMDMap> m_mdMap;

    //std::atomic_flag m_renderQueued = ATOMIC_FLAG_INIT;
};
