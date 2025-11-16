#ifndef TALLER_TP_CARSELECTWINDOW_H
#define TALLER_TP_CARSELECTWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSignalMapper>
#include "carConfirmDialog.h"

class CarSelectionWindow : public QWidget {
    Q_OBJECT
public:
    explicit CarSelectionWindow(QWidget* parent = nullptr);

    signals:
        void carChosen(int carId); // emitida al seleccionar un auto

private slots:
    void onCarClicked(int carId);

private:
    QVector<QPushButton*> carButtons;
};

#endif //TALLER_TP_CARSELECTWINDOW_H
