#include "mopenglwidget.h"

MOpenGLWidget::MOpenGLWidget()
{

}

MOpenGLWidget::~MOpenGLWidget() {
    delete [] vertices;
    delete [] indices;
}

void MOpenGLWidget::initializeGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0, 0.0, 0.0, 0.0);

    simple.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vsh");
    simple.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.fsh");
    simple.link();

    if(!simple.isLinked()) {
        qDebug() << "Error: " << simple.log();
        return;
    }

    m_posAttr = simple.attributeLocation("posAttr");
    m_texCoordAttr = simple.attributeLocation("texCoordAttr");
    m_matrixUniform = simple.uniformLocation("matrix");
    m_heightScaleAttr = simple.uniformLocation("heightScaleAttr");



    vertices = new GLfloat[(planeSegments + 1) * (planeSegments + 1) * 3];
    for(unsigned int col = 0; col < planeSegments + 1; col++) {
        for(unsigned int row = 0; row < planeSegments + 1; row++) {
            unsigned int index = 3 * (row * (planeSegments + 1) + col);
            /*qDebug() << "X: " << col << " Y: " << row << " Index: " << index << " = (" << col / float(planeSegments) << "|"
                     << row / float(planeSegments) << ")"; */
            vertices[index + 0] = col / float(planeSegments);
            vertices[index + 1] = row / float(planeSegments);
            vertices[index + 2] = 0.0f;
        }
    }

    indices = new GLshort[planeSegments * planeSegments * 6];
    unsigned int i = 0;
    for(unsigned int col = 0; col < planeSegments; col++) {
        for(unsigned int row = 0; row < planeSegments; row++) {
            // (col|row) is always the upper left point for the triangle
            // vertices has the position
            indices[i++] = row * (planeSegments + 1) + col;
            indices[i++] = row * (planeSegments + 1) + col + 1;
            indices[i++] = (row + 1) * (planeSegments + 1) + col + 1;

            indices[i++] = row * (planeSegments + 1) + col;
            indices[i++] = (row + 1) * (planeSegments + 1) + col;
            indices[i++] = (row + 1) * (planeSegments + 1) + col + 1;
            //qDebug() << "Index: " << i;

        }
    }

    qDebug() << i;
    //for(unsigned int i=0; i<24; i++) qDebug() << "Index=" << indices[i];

    /* Initialize Texture */
    tex = new QOpenGLTexture(QImage(1024, 1024, QImage::Format_RGB888));

    tex->setMinificationFilter(QOpenGLTexture::Linear);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::ClampToEdge);

    f->glEnable(GL_DEPTH_TEST);
    //f->glEnable(GL_CULL_FACE);

}

void MOpenGLWidget::showImage(QImage img) {
    QOpenGLTexture *old = tex;
    tex = new QOpenGLTexture(img);
    tex->setMinificationFilter(QOpenGLTexture::Linear);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::ClampToEdge);


    old->release();
    delete old;
}

void MOpenGLWidget::resizeGL(int w, int h) {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glViewport(0, 0, w, h);

    mvpMatrix.setToIdentity();
    mvpMatrix.perspective(60.0f, width() / float(height()), 0.1f, 100.0f);
    mvpMatrix.translate(0, 0, -2);
}

void MOpenGLWidget::paintGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);

    simple.bind();
    tex->bind();

    mvpMatrix.setToIdentity();
    mvpMatrix.perspective(60.0f, width() / float(height()), 0.1f, 100.0f);
    mvpMatrix.translate(0, 0, -zoom);
    mvpMatrix.rotate(rotY, 1, 0, 0);
    mvpMatrix.rotate(rotX, 0, 0, 1);
    mvpMatrix.translate(-0.5, -0.5, 0);

    simple.setUniformValue(m_matrixUniform, mvpMatrix);
    simple.setUniformValue(m_heightScaleAttr, heightScale);

    f->glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    f->glEnableVertexAttribArray(0);

    simple.setAttributeArray(m_texCoordAttr, GL_FLOAT, vertices, 2, sizeof(GL_FLOAT) * 3);
    simple.enableAttributeArray(m_texCoordAttr);


    f->glDrawElements(GL_TRIANGLES, planeSegments * planeSegments * 6, GL_UNSIGNED_SHORT, indices);

    f->glDisableVertexAttribArray(0);

    ++frameNr;
    update();
}

void MOpenGLWidget::setHeightScale(int hs) {
    heightScale = hs / 600.0;
}

void MOpenGLWidget::mousePressEvent(QMouseEvent *evt) {
    mouseStartX = evt->globalX();
    mouseStartY = evt->globalY();
}

void MOpenGLWidget::mouseMoveEvent(QMouseEvent *evt) {
    float deltaX = evt->globalX() - mouseStartX;
    float deltaY = evt->globalY() - mouseStartY;

    rotX += deltaX * 0.4;
    rotY += deltaY * 0.4;

    mouseStartX = evt->globalX();
    mouseStartY = evt->globalY();
}
void MOpenGLWidget::mouseReleaseEvent(QMouseEvent *evt) {
    Q_UNUSED(evt)
}

void MOpenGLWidget::wheelEvent(QWheelEvent *event) {
    zoom -= event->angleDelta().y() * 0.001f;
    qDebug() << "Zoom: " << zoom;
}
