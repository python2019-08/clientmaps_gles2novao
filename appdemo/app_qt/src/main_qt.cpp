//#include "mymaps_qt.h"

#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat> 

#include "QtMapGLWidget.h"
 
int main(int argc, char *argv[])
{ 

    QApplication a(argc, argv);

    //mymaps_qt w;
    //w.show();
    QtMapGLWidget  w;

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);  //format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    //w.setFormat(format); // must be called before the widget or its parent window gets shown
   
    w.resize(800, 600);
    w.show();
    //w.showMaximized();

    return a.exec();
}
