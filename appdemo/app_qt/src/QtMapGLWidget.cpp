//
//  ��2020-02-17 Qt��ʹ��Opengl�ķ�����
//���ߣ�MathPhilosophy ���ӣ�https://www.jianshu.com/p/e1ca28078778
//// Qt�¿��Ի�ͨ��QOpenGLWindow����opengl, �ؼ���������, ����������Ҫ
//// ע��ĵط�����ע��˵������������ţ�
//void MyOpenGLWindow::initializeGL()
//{
//    //1. ���context����Ҫʹ�ô����Դ���context,����Լ�����һ��
//    //   context,��ô���������Ŵ��ڵ�������Ӧ�ı��С
//    QOpenGLContext* pContext = this->context();
//
//    //2.ͨ��QSurfaceFormat����ָ��ʹ�õ�opengl�汾��;
//    QSurfaceFormat sur;
//    sur.setVersion(3, 3);
//    //3.��仰һ��Ҫ���ã��������õİ汾�Ų���Ч
//    sur.setProfile(QSurfaceFormat::CoreProfile);
//    pContext->setFormat(sur);
//
//    //4.������һ��Ҫ���ã��������ĳ�ʼ��openglfunctions��ʧ��
//    pContext->create();
//    pContext->makeCurrent(this);
//
//    //5.��ʾʹ��opengl3.3�Ľӿڣ���ʼ����������֮�󣬲���ʹ��openglҵ�����
//    QOpenGLFunctions_3_3_Core* pCore = new QOpenGLFunctions_3_3_Core();
//    pCore->initializeOpenGLFunctions();
//
//    //openglҵ���ʼ������
//    //...  ҵ������ƺ�����ʹ��pCore�ĳ�Ա������Ҳ����ʹ��ԭ��opengl����Ч��һ��
//}
//
////paintGl�е��߼�Ҳ���Էŵ����������У���ͨ����ʱ������������������������
//void MyOpenGLWindow::paintGL()
//{
//    QOpenGLContext* pContext = this->context();
//
//    //6.��仰������ÿ�λ���ǰ���ã�������Ʋ���Ч��������
//    pContext->makeCurrent(this);
//
//    //7.����ֱ���һ��Ҫ���ã����scale��2����glViewport�еĲ���������scale�Ļ�
//    //��������ֻռ1/4
//    const qreal scale = devicePixelRatio();
//    //8 ,ע��glViewport��ȷ����λ�þ����������resizeGL()�����У�
//    glViewport(0, 0, width()*scale, height()* scale);
//    //opengl����ҵ���߼�
//    //...
//    glFlush();
//    //9.�ؼ���һ�仰��������Ʋ���Ч��Ҫʵ�ֶ�̬����Ч�����������
//    //  ��仰������ֻ���ǳ�ʼ������һ�Σ�������޷��޸��ˣ�
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

 