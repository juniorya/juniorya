/**
 * @file project_manager.h
 * @brief Facilities for loading and saving *.bagetproj scenes.
 */

#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include "core/include/cnc_api.h"

#include <QString>

class ProjectManager
{
public:
    ProjectManager();
    bool loadFromFile(const QString &path, cnc_waypoint_buffer &buffer, spl_type_t &type, q16_16 &period) const;
    bool saveToFile(const QString &path, const cnc_waypoint_buffer &buffer, spl_type_t type, q16_16 period) const;
};

#endif /* PROJECT_MANAGER_H */
