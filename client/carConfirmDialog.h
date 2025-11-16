#ifndef TALLER_TP_CARCONFIRMDIALOG_H
#define TALLER_TP_CARCONFIRMDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class CarConfirmDialog : public QDialog {
    Q_OBJECT

public:
    explicit CarConfirmDialog(const QPixmap& carImage, QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Confirm Car");
        setFixedSize(400, 300);

        QVBoxLayout* layout = new QVBoxLayout(this);

        QLabel* preview = new QLabel(this);
        preview->setPixmap(carImage.scaled(300, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        preview->setAlignment(Qt::AlignCenter);

        QPushButton* confirm = new QPushButton("Select", this);
        QPushButton* cancel  = new QPushButton("Cancel", this);

        confirm->setStyleSheet("background-color: rgba(255,0,0,0.85); color:white; font-size:18px;");
        cancel->setStyleSheet("background-color: gray; color:white; font-size:18px;");

        connect(confirm, &QPushButton::clicked, this, [this]() {
            done(1);
        });

        connect(cancel, &QPushButton::clicked, this, [this]() {
            done(0);
        });

        layout->addWidget(preview);
        layout->addWidget(confirm);
        layout->addWidget(cancel);
    }
};

#endif //TALLER_TP_CARCONFIRMDIALOG_H
