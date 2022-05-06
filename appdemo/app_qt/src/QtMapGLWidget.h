#ifndef QT_MAPGLWIDGET_H
#define QT_MAPGLWIDGET_H

#include<memory>

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QStringList>
 
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QOpenGLWidget>
 
class QMapGL4MdMap;

class QtMapGLWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
    QtMapGLWidget(QWidget *parent = nullptr);
	~QtMapGLWidget();

private:
    void changeStyle();

    // QWidget implementation.
    void keyPressEvent(QKeyEvent *ev) final;
    void mousePressEvent(QMouseEvent *ev) final;
    void mouseMoveEvent(QMouseEvent *ev) final;
    void wheelEvent(QWheelEvent *ev) final;

	// Q{,Open}GLWidget implementation.
    virtual void initializeGL() final;
    virtual void resizeGL(int w, int h) final;
    virtual void paintGL() final;

private:
    // QMapGL4MdMap是enable_shared_from_this的子类，所以只能std::shared_ptr,
    // 不能用std::unique_ptr。
    std::shared_ptr<QMapGL4MdMap>  m_QMdMap;

    QPointF m_lastPos;
    bool m_sourceAdded = false;
};

#endif  //#ifndef QT_MAPWINDOW_H