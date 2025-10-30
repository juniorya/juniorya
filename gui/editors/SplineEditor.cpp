/**
 * @file SplineEditor.cpp
 * @brief Implementation of the spline editor panel.
 */

#include "SplineEditor.h"

#include <QComboBox>
#include <QtGlobal>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVariant>
#include <QVBoxLayout>

#include "utils/q16.h"
#include "utils/vec3.h"

SplineEditor::SplineEditor(QWidget *parent)
    : QWidget(parent),
      m_table(new QTableWidget(this)),
      m_typeCombo(new QComboBox(this)),
      m_period(new QDoubleSpinBox(this))
{
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"X", "Y", "Z"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);

    QPushButton *add = new QPushButton(tr("Add"), this);
    QPushButton *remove = new QPushButton(tr("Remove"), this);

    connect(add, &QPushButton::clicked, this, &SplineEditor::onAddPoint);
    connect(remove, &QPushButton::clicked, this, &SplineEditor::onRemovePoint);
    connect(m_table, &QTableWidget::itemChanged, this, &SplineEditor::splineChanged);

    m_typeCombo->addItem(tr("Bézier"), QVariant::fromValue<int>(SPL_BEZIER3));
    m_typeCombo->addItem(tr("B-spline"), QVariant::fromValue<int>(SPL_BSPLINE3));
    m_typeCombo->addItem(tr("Quintic"), QVariant::fromValue<int>(SPL_QUINTIC));
    m_typeCombo->addItem(tr("NURBS"), QVariant::fromValue<int>(SPL_NURBS3));
    m_typeCombo->addItem(tr("Brachistochrone"), QVariant::fromValue<int>(SPL_BRACHISTOCHRONE));

    connect(m_typeCombo, &QComboBox::currentIndexChanged, this, &SplineEditor::splineChanged);

    m_period->setSuffix(" ms");
    m_period->setRange(0.1, 10.0);
    m_period->setValue(1.0);
    connect(m_period, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &SplineEditor::splineChanged);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_table);

    QHBoxLayout *buttonRow = new QHBoxLayout();
    buttonRow->addWidget(add);
    buttonRow->addWidget(remove);
    layout->addLayout(buttonRow);

    QFormLayout *form = new QFormLayout();
    form->addRow(tr("Type"), m_typeCombo);
    form->addRow(tr("Sample period"), m_period);
    layout->addLayout(form);
    layout->addStretch();

    populateDefaults();
}

void SplineEditor::populateDefaults()
{
    cnc_waypoint_buffer buffer;
    cnc_api_waypoints_default(&buffer);
    setWaypoints(buffer, SPL_BEZIER3, q16_from_float(1.0f));
}

void SplineEditor::onAddPoint()
{
    int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, new QTableWidgetItem("0.0"));
    m_table->setItem(row, 1, new QTableWidgetItem("0.0"));
    m_table->setItem(row, 2, new QTableWidgetItem("0.0"));
    emit splineChanged();
}

void SplineEditor::onRemovePoint()
{
    int row = m_table->currentRow();
    if (row >= 0)
    {
        m_table->removeRow(row);
        emit splineChanged();
    }
}

cnc_waypoint_buffer SplineEditor::waypoints() const
{
    cnc_waypoint_buffer buffer;
    buffer.count = 0U;
    int rows = m_table->rowCount();
    for (int i = 0; i < rows && buffer.count < CNC_API_MAX_WAYPOINTS; ++i)
    {
        bool okX = false;
        bool okY = false;
        bool okZ = false;
        QTableWidgetItem *ix = m_table->item(i, 0);
        QTableWidgetItem *iy = m_table->item(i, 1);
        QTableWidgetItem *iz = m_table->item(i, 2);
        if ((ix == nullptr) || (iy == nullptr) || (iz == nullptr))
        {
            continue;
        }
        double x = ix->text().toDouble(&okX);
        double y = iy->text().toDouble(&okY);
        double z = iz->text().toDouble(&okZ);
        if (okX && okY && okZ)
        {
            buffer.points[buffer.count++] = vec3_from_float(static_cast<float>(x),
                                                            static_cast<float>(y),
                                                            static_cast<float>(z));
        }
    }
    return buffer;
}

spl_type_t SplineEditor::splineType() const
{
    return static_cast<spl_type_t>(m_typeCombo->currentData().toInt());
}

q16_16 SplineEditor::samplePeriod() const
{
    return q16_from_float(static_cast<float>(m_period->value()));
}

void SplineEditor::setWaypoints(const cnc_waypoint_buffer &buffer, spl_type_t type, q16_16 period)
{
    m_table->setRowCount(static_cast<int>(buffer.count));
    for (size_t i = 0; i < buffer.count; ++i)
    {
        const vec3_q16 point = buffer.points[i];
        m_table->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(q16_to_float(point.x))));
        m_table->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::number(q16_to_float(point.y))));
        m_table->setItem(static_cast<int>(i), 2, new QTableWidgetItem(QString::number(q16_to_float(point.z))));
    }
    int idx = m_typeCombo->findData(QVariant::fromValue<int>(type));
    if (idx < 0)
    {
        idx = 0;
    }
    m_typeCombo->setCurrentIndex(idx);
    m_period->setValue(q16_to_float(period));
    emit splineChanged();
}
