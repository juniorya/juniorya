/**
 * @file MainWindow.h
 * @brief Top-level IDE window combining editors and 3D view.
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class TrajectoryView;
class SplineEditor;
class ProjectManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void rebuildPreview();
    void onOpenProject();
    void onSaveProject();

private:
    void initialiseUi();
    void refreshPreview();

    TrajectoryView *m_view;
    SplineEditor *m_editor;
    ProjectManager m_projects;
};

#endif /* MAIN_WINDOW_H */
