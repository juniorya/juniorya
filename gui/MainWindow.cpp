/**
 * @file MainWindow.cpp
 * @brief Implementation of the IDE main window.
 */

#include "MainWindow.h"

#include "adapters/trajectory_adapter.h"
#include "core/include/cnc_api.h"
#include "gui/editors/SplineEditor.h"
#include "gui/view3d/TrajectoryView.h"
#include "project/project_manager.h"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>

#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_view(new TrajectoryView(this)),
      m_editor(new SplineEditor(this))
{
    initialiseUi();
    refreshPreview();
}

void MainWindow::initialiseUi()
{
    setWindowTitle(tr("Delta CNC Studio"));
    setCentralWidget(m_view);

    QDockWidget *dock = new QDockWidget(tr("Spline Editor"), this);
    dock->setWidget(m_editor);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    connect(m_editor, &SplineEditor::splineChanged, this, &MainWindow::rebuildPreview);

    QToolBar *toolbar = addToolBar(tr("Playback"));
    QAction *refresh = toolbar->addAction(tr("Rebuild"));
    connect(refresh, &QAction::triggered, this, &MainWindow::rebuildPreview);

    QAction *openAction = toolbar->addAction(tr("Open"));
    QAction *saveAction = toolbar->addAction(tr("Save"));
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenProject);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveProject);
}

void MainWindow::rebuildPreview()
{
    refreshPreview();
}

void MainWindow::refreshPreview()
{
    cnc_waypoint_buffer buffer = m_editor->waypoints();
    cnc_preview_t preview;
    if (!cnc_api_build_preview(&buffer, m_editor->splineType(), m_editor->samplePeriod(), &preview))
    {
        return;
    }
    std::vector<float> positions;
    adapter_fill_vectors(preview, positions, nullptr);
    m_view->updateTrajectory(positions);
}

void MainWindow::onOpenProject()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Project"), QString(), tr("Baget Project (*.bagetproj)"));
    if (path.isEmpty())
    {
        return;
    }
    cnc_waypoint_buffer buffer;
    spl_type_t type;
    q16_16 period;
    if (!m_projects.loadFromFile(path, buffer, type, period))
    {
        QMessageBox::warning(this, tr("Load Failed"), tr("Unable to parse project file."));
        return;
    }
    m_editor->setWaypoints(buffer, type, period);
}

void MainWindow::onSaveProject()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save Project"), QStringLiteral("project.bagetproj"), tr("Baget Project (*.bagetproj)"));
    if (path.isEmpty())
    {
        return;
    }
    cnc_waypoint_buffer buffer = m_editor->waypoints();
    if (!m_projects.saveToFile(path, buffer, m_editor->splineType(), m_editor->samplePeriod()))
    {
        QMessageBox::warning(this, tr("Save Failed"), tr("Unable to write project file."));
    }
}
