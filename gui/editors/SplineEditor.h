/**
 * @file SplineEditor.h
 * @brief Widget for editing spline control points and sampling settings.
 */

#ifndef SPLINE_EDITOR_H
#define SPLINE_EDITOR_H

#include "core/include/cnc_api.h"

#include <QWidget>

class QComboBox;
class QDoubleSpinBox;
class QTableWidget;

class SplineEditor : public QWidget
{
    Q_OBJECT
public:
    explicit SplineEditor(QWidget *parent = nullptr);
    cnc_waypoint_buffer waypoints() const;
    spl_type_t splineType() const;
    q16_16 samplePeriod() const;
    void setWaypoints(const cnc_waypoint_buffer &buffer, spl_type_t type, q16_16 period);

signals:
    void splineChanged();

private slots:
    void onAddPoint();
    void onRemovePoint();

private:
    void populateDefaults();

    QTableWidget *m_table;
    QComboBox *m_typeCombo;
    QDoubleSpinBox *m_period;
};

#endif /* SPLINE_EDITOR_H */
