/**
 * @file TrajectoryView.cpp
 * @brief Implementation of the interactive 3D viewport.
 */

#include "TrajectoryView.h"

#include <QQuaternion>
#include <QtMath>

TrajectoryView::TrajectoryView(QWidget *parent)
    : QOpenGLWidget(parent),
      m_renderer(),
      m_positions(),
      m_projection(),
      m_cameraPos(200.0f, -200.0f, 300.0f),
      m_cameraTarget(0.0f, 0.0f, -40.0f),
      m_lastPos(),
      m_timer(this)
{
    m_timer.setInterval(16);
    connect(&m_timer, &QTimer::timeout, this, &TrajectoryView::onFrame);
    m_timer.start();
    setFocusPolicy(Qt::StrongFocus);
}

void TrajectoryView::updateTrajectory(const std::vector<float> &positions)
{
    m_positions = positions;
    makeCurrent();
    m_renderer.updateTrajectory(m_positions);
    doneCurrent();
    update();
}

void TrajectoryView::initializeGL()
{
    m_renderer.initialise();
}

void TrajectoryView::resizeGL(int w, int h)
{
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, float(w) / float(h ? h : 1), 1.0f, 5000.0f);
}

void TrajectoryView::paintGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.08f, 0.1f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 view;
    view.lookAt(m_cameraPos, m_cameraTarget, QVector3D(0.0f, 0.0f, 1.0f));
    QMatrix4x4 viewProj = m_projection * view;
    m_renderer.render(viewProj, 2.0f, QVector3D(0.9f, 0.3f, 0.1f));
}

void TrajectoryView::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void TrajectoryView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = event->pos() - m_lastPos;
    m_lastPos = event->pos();
    const float sensitivity = 0.4f;
    QVector3D forward = (m_cameraTarget - m_cameraPos).normalized();
    QVector3D right = QVector3D::crossProduct(forward, QVector3D(0.0f, 0.0f, 1.0f)).normalized();
    QQuaternion yaw = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 1.0f), -delta.x() * sensitivity);
    QQuaternion pitch = QQuaternion::fromAxisAndAngle(right, -delta.y() * sensitivity);
    QVector3D offset = m_cameraPos - m_cameraTarget;
    offset = yaw.rotatedVector(offset);
    offset = pitch.rotatedVector(offset);
    m_cameraPos = m_cameraTarget + offset;
    update();
}

void TrajectoryView::wheelEvent(QWheelEvent *event)
{
    float scale = 1.0f - (event->angleDelta().y() / 1200.0f);
    QVector3D direction = (m_cameraPos - m_cameraTarget);
    m_cameraPos = m_cameraTarget + direction * scale;
    update();
}

void TrajectoryView::onFrame()
{
    update();
}
