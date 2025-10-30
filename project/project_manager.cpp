/**
 * @file project_manager.cpp
 * @brief JSON based implementation of the *.bagetproj container.
 */

#include "project_manager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "utils/q16.h"
#include "utils/vec3.h"

ProjectManager::ProjectManager() = default;

static QJsonArray serializePoints(const cnc_waypoint_buffer &buffer)
{
    QJsonArray arr;
    for (size_t i = 0; i < buffer.count; ++i)
    {
        QJsonObject obj;
        obj["x"] = q16_to_float(buffer.points[i].x);
        obj["y"] = q16_to_float(buffer.points[i].y);
        obj["z"] = q16_to_float(buffer.points[i].z);
        arr.append(obj);
    }
    return arr;
}

static void deserializePoints(const QJsonArray &arr, cnc_waypoint_buffer &buffer)
{
    buffer.count = 0U;
    for (const QJsonValue &value : arr)
    {
        if (!value.isObject())
        {
            continue;
        }
        QJsonObject obj = value.toObject();
        if (buffer.count >= CNC_API_MAX_WAYPOINTS)
        {
            break;
        }
        buffer.points[buffer.count++] = vec3_from_float(static_cast<float>(obj.value("x").toDouble()),
                                                        static_cast<float>(obj.value("y").toDouble()),
                                                        static_cast<float>(obj.value("z").toDouble()));
    }
}

bool ProjectManager::saveToFile(const QString &path,
                                const cnc_waypoint_buffer &buffer,
                                spl_type_t type,
                                q16_16 period) const
{
    QJsonObject root;
    root["type"] = static_cast<int>(type);
    root["period"] = q16_to_float(period);
    root["points"] = serializePoints(buffer);

    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool ProjectManager::loadFromFile(const QString &path,
                                  cnc_waypoint_buffer &buffer,
                                  spl_type_t &type,
                                  q16_16 &period) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject())
    {
        return false;
    }
    QJsonObject root = doc.object();
    type = static_cast<spl_type_t>(root.value("type").toInt(static_cast<int>(SPL_BEZIER3)));
    period = q16_from_float(static_cast<float>(root.value("period").toDouble(1.0)));
    deserializePoints(root.value("points").toArray(), buffer);
    return true;
}
