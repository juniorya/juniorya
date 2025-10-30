/**
 * @file TrajectoryView.h
 * @brief Qt widget that renders the CNC trajectory in 3D.
 */

#ifndef TRAJECTORY_VIEW_H
#define TRAJECTORY_VIEW_H

#include "render/opengl_renderer.h"

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QPoint>
#include <QTimer>
#include <QVector3D>

#include <vector>

class TrajectoryView : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit TrajectoryView(QWidget *parent = nullptr);
    void updateTrajectory(const std::vector<float> &positions);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void onFrame();

private:
    OpenGlRenderer m_renderer;
    std::vector<float> m_positions;
    QMatrix4x4 m_projection;
    QVector3D m_cameraPos;
    QVector3D m_cameraTarget;
    QPoint m_lastPos;
    QTimer m_timer;
};

#endif /* TRAJECTORY_VIEW_H */
