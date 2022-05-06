//
//  《2020-02-17 Qt下使用Opengl的方法》
//作者：MathPhilosophy 链接：https://www.jianshu.com/p/e1ca28078778
//// Qt下可以会通过QOpenGLWindow接入opengl, 关键代码如下, 对于其中需要
//// 注意的地方加了注释说明，并标了序号：
//void MyOpenGLWindow::initializeGL()
//{
//    //1. 这个context必须要使用窗口自带的context,如果自己创建一个
//    //   context,那么它不会随着窗口的缩放相应改变大小
//    QOpenGLContext* pContext = this->context();
//
//    //2.通过QSurfaceFormat可以指定使用的opengl版本号;
//    QSurfaceFormat sur;
//    sur.setVersion(3, 3);
//    //3.这句话一定要调用，否则设置的版本号不生效
//    sur.setProfile(QSurfaceFormat::CoreProfile);
//    pContext->setFormat(sur);
//
//    //4.这两句一定要调用，否则后面的初始化openglfunctions会失败
//    pContext->create();
//    pContext->makeCurrent(this);
//
//    //5.表示使用opengl3.3的接口，初始化函数调用之后，才能使用opengl业务代码
//    QOpenGLFunctions_3_3_Core* pCore = new QOpenGLFunctions_3_3_Core();
//    pCore->initializeOpenGLFunctions();
//
//    //opengl业务初始化代码
//    //...  业务代码似乎可以使用pCore的成员函数，也可以使用原生opengl函数效果一样
//}
//
////paintGl中的逻辑也可以放到其他函数中，并通过计时器或其他触发都能正常绘制
//void MyOpenGLWindow::paintGL()
//{
//    QOpenGLContext* pContext = this->context();
//
//    //6.这句话必须在每次绘制前调用，否则绘制不生效或有问题
//    pContext->makeCurrent(this);
//
//    //7.这个分辨率一定要设置，如果scale是2，则glViewport中的参数不乘以scale的话
//    //绘制区域只占1/4
//    const qreal scale = devicePixelRatio();
//    //8 ,注意glViewport正确调用位置就在这里，而非resizeGL()函数中；
//    glViewport(0, 0, width()*scale, height()* scale);
//    //opengl绘制业务逻辑
//    //...
//    glFlush();
//    //9.关键的一句话，否则绘制不生效，要实现动态绘制效果，必须调用
//    //  这句话，否则只能是初始化绘制一次，后面就无法修改了；
//    pContext->swapBuffer(this);
//}
//
#include<utility>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtGui/QColor>
#include <QtGui/QImage>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QMargins>
#include <QtCore/QObject>

#include "QMapGL4MdMap.hpp" 

#include "QtMapGLWidget.h"



QtMapGLWidget::QtMapGLWidget(QWidget *parent)
{
    setWindowIcon(QIcon(":icon.png"));

}

QtMapGLWidget::~QtMapGLWidget()
{
	// Make sure the context is current and then explicitly
    // destroy all underlying OpenGL resources.
    makeCurrent();
    
	//delete m_texture;
	//delete m_shader;
	//delete m_program;
	//m_vbo.destroy();
	//m_vao.destroy();
	//doneCurrent();
}



void QtMapGLWidget::changeStyle()
{
    //static uint8_t currentStyleIndex;

    //auto& styles = QMapbox::defaultStyles();

    //m_map->setStyleUrl(styles[currentStyleIndex].first);
    //setWindowTitle(QString("Mapbox GL: ") + styles[currentStyleIndex].second);

    //if (++currentStyleIndex == styles.size()) {
    //    currentStyleIndex = 0;
    //}

    m_sourceAdded = false;
}

void QtMapGLWidget::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key()) 
    {
    case Qt::Key_C:
        changeStyle();
        break;
    case Qt::Key_L: 
    {
        if (m_sourceAdded) {
            return;
        }

        m_sourceAdded = true;

        // Not in all styles, but will work on streets
        QString before = "waterway-label";

        QFile geojson(":source1.geojson");
        geojson.open(QIODevice::ReadOnly);           
    }break;
    case Qt::Key_A :
    {
        QPointF delta(-1,0);
        m_QMdMap->moveBy(delta);
    }
    break; 
    case Qt::Key_D: 
    {
        QPointF delta( 1,  0);
        m_QMdMap->moveBy(delta);
    }
    break;  
    case Qt::Key_W:
    {
        QPointF delta(0, -1);
        m_QMdMap->moveBy(delta);
    }
    break;
    case Qt::Key_S:
    {
        QPointF delta(0, 1);
        m_QMdMap->moveBy(delta);
    }
    break;
    case Qt::Key_Q:
    {
        m_QMdMap->rotateBy_deg(-1);
    }
    break;
    case Qt::Key_R:
    {
        m_QMdMap->rotateBy_deg(1);
    }
    break;
    case Qt::Key_PageUp:
    {
        m_QMdMap->pitchBy(-1);
    }
    break;
    case Qt::Key_PageDown:
    {
        m_QMdMap->pitchBy(1);
    }
    break;
    case Qt::Key_Alt:
    {
        m_QMdMap->returnToDefaultPos();
    }
    break;    
    default:
        break;
    }

    //if (ev->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier) && 
    //    ev->key() == Qt::Key_A)
    //{
    //    //pressed
    //}

    ev->accept();
}

void QtMapGLWidget::mousePressEvent(QMouseEvent *ev)
{
    m_lastPos = ev->localPos();

    if (ev->type() == QEvent::MouseButtonPress) 
    {
        if (ev->buttons() == (Qt::LeftButton | Qt::RightButton)) 
        {
             
        }
    }

    if (ev->type() == QEvent::MouseButtonDblClick) 
    {
        if (ev->buttons() == Qt::LeftButton) 
        {
            m_QMdMap->scaleBy(2.0, m_lastPos);
        }
        else if (ev->buttons() == Qt::RightButton) 
        {
            m_QMdMap->scaleBy(0.5, m_lastPos);
        }
    }

    ev->accept();
}

void QtMapGLWidget::mouseMoveEvent(QMouseEvent *ev)
{
    QPointF delta = ev->localPos() - m_lastPos;

    if (!delta.isNull()) 
    {
        if (ev->buttons() == Qt::LeftButton && 
            ev->modifiers() & Qt::ShiftModifier) 
        {
            m_QMdMap->pitchBy(delta.y());
        }
        else if (ev->buttons() == Qt::LeftButton) 
        {
            m_QMdMap->moveBy(delta);
        }
        else if (ev->buttons() == Qt::RightButton) 
        {
            m_QMdMap->rotateBy(m_lastPos, ev->localPos());
        }
    }

    m_lastPos = ev->localPos();
    ev->accept();
}

void QtMapGLWidget::wheelEvent(QWheelEvent *ev)
{
    if (ev->orientation() == Qt::Horizontal)
    {
        return;
    }

    float factor = ev->delta() / 1200.;
    if (ev->delta() < 0) {
        factor = factor > -1 ? factor : 1 / factor;
    }
          
    //qDebug("scaleBy factor=%f", factor);        
      
    //m_QMdMap->scaleBy(1 + factor, ev->pos()); 
    m_QMdMap->zoomBy( factor );
    ev->accept();
}


// Q{,Open}GLWidget implementation.
void QtMapGLWidget::initializeGL()
{
    m_QMdMap = std::make_shared<QMapGL4MdMap>();
    m_QMdMap->init( this->devicePixelRatioF() );
    m_QMdMap->setDefaultFboId(this->defaultFramebufferObject() );
    QObject::connect(m_QMdMap.get(), SIGNAL(needsRendering()),
                     this,            SLOT(update()) );

    //// Set default location to beijing.
    //m_QMdMap->jumpTo(116.333333, 39.93333333, 2.5); 

    QString styleName = "mapstyle_streets-v10_v2.json";
    m_QMdMap->setStyle( styleName.toStdString() );
    //setWindowTitle(QString("MapStyle: ") + styleName);
	setWindowTitle(QString("map demo^-^ ^-^"));
}

void QtMapGLWidget::resizeGL(int w, int h)
{
    m_QMdMap->resize(w, h);
}

void QtMapGLWidget::paintGL()
{    
    double pixelRatio = this->devicePixelRatioF(); 
    QSize widgetSize = this->size() * pixelRatio;
    m_QMdMap->resize(widgetSize.width(), widgetSize.height() );

    m_QMdMap->drawFrame();
}

 