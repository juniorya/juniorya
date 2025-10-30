/**
 * @file opengl_renderer.cpp
 * @brief Implementation of the OpenGL line renderer used by the IDE viewport.
 */

#include "opengl_renderer.h"

#include <QOpenGLShader>

OpenGlRenderer::OpenGlRenderer()
    : m_program(),
      m_vbo(0U),
      m_vertexCount(0)
{
}

OpenGlRenderer::~OpenGlRenderer()
{
    if (m_vbo != 0U)
    {
        glDeleteBuffers(1, &m_vbo);
    }
}

void OpenGlRenderer::initialise()
{
    initializeOpenGLFunctions();
    static const char *vertexSrc = R"( #version 410 core
layout(location = 0) in vec3 aPos;
uniform mat4 uMVP;
void main() { gl_Position = uMVP * vec4(aPos, 1.0); }
)";
    static const char *fragmentSrc = R"( #version 410 core
out vec4 FragColor;
uniform vec3 uColor;
void main() { FragColor = vec4(uColor, 1.0); }
)";
    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSrc);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSrc);
    m_program.link();
    glGenBuffers(1, &m_vbo);
}

void OpenGlRenderer::updateTrajectory(const std::vector<float> &positions)
{
    if (m_vbo == 0U)
    {
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(positions.size() * sizeof(float)),
                 positions.data(),
                 GL_DYNAMIC_DRAW);
    m_vertexCount = static_cast<GLsizei>(positions.size() / 3U);
}

void OpenGlRenderer::render(const QMatrix4x4 &viewProj, float thickness, const QVector3D &color)
{
    if ((m_vertexCount == 0) || (m_vbo == 0U))
    {
        return;
    }
    glEnable(GL_PROGRAM_POINT_SIZE);
    glLineWidth(thickness);
    m_program.bind();
    m_program.setUniformValue("uMVP", viewProj);
    m_program.setUniformValue("uColor", color);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_LINE_STRIP, 0, m_vertexCount);
    glDisableVertexAttribArray(0);
    m_program.release();
}
