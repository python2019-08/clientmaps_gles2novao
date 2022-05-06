#include <QtGui/QGuiApplication>

#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QImage>
#include <QtCore/QString>
#include <QtCore/QStringList>
//#include <QtCore/QMargins>
//#include <QtCore/QThreadStorage>
//#include <QtCore/QVariant>
//#include <QtCore/QVariantList> 
//#include <QtCore/QVariantMap>


#include <functional> 
#include <memory>
#include <sstream>

#include "md/md_map.h" // CMDMap

#include "QMapGL4MdMap.hpp" 


////////////////////////////////////////////////////////////////
// #pragma comment(lib,"opengl32.lib")....start
////////////////////////////////////////////////////////////////
#ifdef MDENGINE_LIB
#define MD_API
#else
#ifdef MDENGINE_DLL
#define MD_API _declspec(dllexport)
#else
#define MD_API _declspec(dllimport)
#endif
#endif

#pragma warning(disable: 4244)

#define APP_LIB 1

#define CONFIG_DEBUG    1
#define PLatform_64BIT    1

#if  APP_LIB == 1
#if defined(OsID_WINDOWS)// -------windows
#if      !defined(SWDevFW_QT)
//---------OsID_WINDOWS ,start--------
#   ifdef  CONFIG_DEBUG
#       define  LIB_PATH "Debug_MD"
#   else
#       define  LIB_PATH "Release_MD"
#   endif

#ifdef PLatform_64BIT
#    define PLATFORM_PATH "x64"
#else
#    define PLATFORM_PATH "win32"
#endif


#pragma comment(lib,"opengl32.lib")
#pragma comment( lib, "../../vendor/lib/" PLATFORM_PATH "/glew32.lib" )
#endif //#if      !defined(SWDevFW_QT)

//#pragma comment( lib, "ws2_32.lib" )
//#pragma message("******Automatically load****** mod:socket") 


//#pragma comment( lib, "../../../lib/" PLATFORM_PATH"/" LIB_PATH "/mwbdisplay.lib" )
//#pragma message("******Automatically load****** mod:mwbdisplay")


//#pragma comment( lib, "../../../vendor/lib/" PLATFORM_PATH "/" LIB_PATH "/zlib.lib" )
//#pragma message("******Automatically load****** zlib.lib")
//
//#pragma comment( lib, "../../../vendor/lib/" PLATFORM_PATH "/" LIB_PATH "/libpng.lib" )
//#pragma message("******Automatically load****** libpng.lib")
//
//#pragma comment( lib, "../../../vendor/lib/" PLATFORM_PATH "/" LIB_PATH "/freetype250.lib" )	
//#pragma message("******Automatically load****** freetype250.lib")
//
//#pragma comment( lib, "../../../vendor/lib/" PLATFORM_PATH "/" LIB_PATH "/sqlite3.lib" )	
//#pragma message("******Automatically load****** sqlite3.lib")

//---------OsID_WINDOWS ,start--------
#elif OsID_IOS     //---------ios
#elif OsID_MAC     //---------mac
#elif OsID_ANDROID //---------android
#else                  //---------error: unknown platform
#error "unknown platform!"
#endif
#endif 
////////////////////////////////////////////////////////////////
// #pragma comment(lib,"opengl32.lib")....end
////////////////////////////////////////////////////////////////
//static_assert(md::underlying_type(UniqueGLContext) == md::underlying_type(ContextMode::Unique), "error");
 
QMapGL4MdMap::QMapGL4MdMap(QObject* _parent )
    : QObject(_parent) 
   , m_mdMap(std::make_unique<clientmap::CMDMap>() )
{
    
}
 
QMapGL4MdMap::~QMapGL4MdMap()
{
    done();
}

#define  RES_ROOT_DIR   "../../resroot" 

int QMapGL4MdMap::init(double      _pixelRatio )
{
    // Setup the Map object.  
    if (m_mdMap)
    { 
		const std::string baseUrl = "https://127.0.0.1/";
        const std::string accessToken="NoToken";        
        m_mdMap->init(shared_from_this(), _pixelRatio,
            RES_ROOT_DIR,  baseUrl, accessToken );

        //// Set default location to beijing.
        //m_mdMap->setCoordinateZoom(QMapbox::Coordinate(60.170448, 24.942046), 14);

        //QString styleUrl = "MAPBOX_STYLE_URL";
        //m_mdMap->setStyleUrl(styleUrl);
        //setWindowTitle(QString("Mapbox GL: ") + styleUrl);

        return 0;
    }

    return -1;
}

void QMapGL4MdMap::done()
{
    if (m_mdMap)
    {
        m_mdMap->done();
    }
}

void QMapGL4MdMap::setStyle(const std::string& aStyleName)
{
    if (m_mdMap) {
        m_mdMap->setStyle(aStyleName);
    }
}

void QMapGL4MdMap::setDefaultFboId(uint32_t fbo)
{
    if (m_mdMap) {
        m_mdMap->setDefaultFboId(fbo);
    }
}
void QMapGL4MdMap::resize(int32_t _w, int32_t _h)
{
    if (m_mdMap) {
        m_mdMap->resize(_w, _h);
    }
}

void QMapGL4MdMap::jumpTo(double aLon, double aLat, 
    double aZoom, 
    double aRollAngle, 
    double aPitchAngle)
{
    if (m_mdMap) {
        
        clientmap::CameraOptions  camOption;
        camOption.withCenter({ aLon ,aLat });
        camOption.withZoom(aZoom);
        if (aRollAngle >= 0)
        {
            camOption.withBearing(aRollAngle);
        }

        if (aPitchAngle >= 0)
        {
            camOption.withPitch(aPitchAngle);
        }        
        
        m_mdMap->jumpTo(camOption);
    }
}
 
void QMapGL4MdMap::returnToDefaultPos()
{
    if (m_mdMap) {  
        m_mdMap->returnToDefaultPos();
    }
}

void QMapGL4MdMap::moveBy(const QPointF & aOffset)
{
    if (m_mdMap) {
        clientmap::ScreenPt  offset1{ aOffset.x(),aOffset.y() };
        m_mdMap->moveBy(offset1);
    }
}

void QMapGL4MdMap::scaleBy(double scale,const QPointF& aAnchor)
{
    if (m_mdMap) {
        clientmap::ScreenPt  anchor{ aAnchor.x(),aAnchor.y() };
        m_mdMap->scaleBy(scale, anchor);
    }
}
void QMapGL4MdMap::zoomBy(double aDeltaZ )
{
    if (m_mdMap) { 
        m_mdMap->zoomBy(aDeltaZ);
    }
}

void QMapGL4MdMap::rotateBy(const QPointF & aP1, 
    const QPointF &aP2)
{
    if (m_mdMap) {
        clientmap::ScreenPt p1{ aP1.x(),aP1.y() };
        clientmap::ScreenPt p2{ aP1.x(),aP2.y() };
        m_mdMap->rotateBy(p1, p2);
    }
}

void QMapGL4MdMap::rotateBy_deg(double aDegDiff)
{
    if (m_mdMap) { 
        m_mdMap->rotateBy_deg(aDegDiff, {});
    }
}

void QMapGL4MdMap::pitchBy(double pitch)
{
    if (m_mdMap) {
        m_mdMap->pitchBy(pitch);
    }
}

//void QMapGL4MdMap::update(std::shared_ptr<mbgl::UpdateParameters> parameters)
//{
//    std::lock_guard<std::recursive_mutex> lock(m_mapRendererMutex);
//
//    m_updateParameters = std::move(parameters);
//
//    if (!m_mapRenderer) {
//        return;
//    }
//
//    m_mapRenderer->updateParameters(std::move(m_updateParameters));
//
//    requestRendering();
//}
 

void QMapGL4MdMap::triggerDraw()
{
    //if (!m_renderQueued.test_and_set()) {
    emit needsRendering();
    //}
}

void QMapGL4MdMap::drawFrame()
{
    //m_renderQueued.clear();
    m_mdMap->drawFrame();
}


 
 