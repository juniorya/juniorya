/**
 * @file main.cpp
 * @brief Entry point for the graphical IDE.
 */

#include "MainWindow.h"

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(1280, 720);
    window.show();
    return app.exec();
}
