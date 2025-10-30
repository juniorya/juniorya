/**
 * @file opengl_renderer.h
 * @brief Lightweight OpenGL helper for drawing CNC trajectories.
 */

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include <QMatrix4x4>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <vector>

/**
 * @brief Rendering context storing GPU state.
 */
class OpenGlRenderer : protected QOpenGLFunctions_4_1_Core
{
public:
    OpenGlRenderer();
    ~OpenGlRenderer();

    void initialise();
    void updateTrajectory(const std::vector<float> &positions);
    void render(const QMatrix4x4 &viewProj, float thickness, const QVector3D &color);

private:
    QOpenGLShaderProgram m_program;
    GLuint m_vbo;
    GLsizei m_vertexCount;
};

#endif /* OPENGL_RENDERER_H */
