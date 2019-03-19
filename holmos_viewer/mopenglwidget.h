#ifndef MOPENGLWIDGET_H
#define MOPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDragMoveEvent>

class MOpenGLWidget : public QOpenGLWidget
{
public:
    MOpenGLWidget();
    virtual ~MOpenGLWidget();

public slots:
    void showImage(QImage);
    void setHeightScale(int);

protected:
    QOpenGLShaderProgram simple;
    GLuint m_posAttr, m_texCoordAttr, m_heightScaleAttr;
    GLuint m_matrixUniform;
    QMatrix4x4 mvpMatrix;
    int frameNr = 0;
    GLfloat *vertices;
    GLshort *indices;
    QOpenGLTexture *tex;

    float zoom = 2.0;
    float rotX = 0.0, rotY = 0.0;
    float heightScale = 0.4;

    float mouseStartX, mouseStartY;


    const unsigned int planeSegments = 200;
    const float planeSize = 1.0;

    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
};

#endif // MOPENGLWIDGET_H
