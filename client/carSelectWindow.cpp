#include "carSelectWindow.h"
#include <QPixmap>
#include <QMessageBox>
#include <QPalette>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

CarSelectionWindow::CarSelectionWindow(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle("Select Your Car");
    resize(800, 600);

    // background
    QPixmap bg("/home/alex/Documents/TP-Final-Taller_2C2025/v2.1/TP-Taller-G7/assets/need-for-speed/lobbyImg/wall3.jpg");
    if (!bg.isNull()) {
        bg = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPalette pal;
        pal.setBrush(QPalette::Window, QBrush(bg));
        setAutoFillBackground(true);
        setPalette(pal);
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QHBoxLayout* row1 = new QHBoxLayout();
    QHBoxLayout* row2 = new QHBoxLayout();

    QVector<QString> carImages = {
        "car1.jpg", "car2.jpg", "car3.jpg", "car4.jpg",
        "car5.jpg", "car6.jpg", "car7.jpg"
    };

    int id = 0;
    for (const QString& imgName : carImages) {
        QString fullPath =
            "/home/alex/Documents/TP-Final-Taller_2C2025/v2.1/TP-Taller-G7/assets/need-for-speed/lobbyImg/" + imgName;

        // Marco
        QFrame* frame = new QFrame(this);
        frame->setFixedSize(180, 150);
        frame->setStyleSheet(
            "QFrame {"
            " background-color: rgba(0,0,0,0.5);"
            " border: 3px solid rgba(180, 0, 0, 0.8);"   /* borde rojo tipo neon */
            " border-radius: 10px;"
            "}"
            "QFrame:hover {"
            " border: 3px solid rgba(255, 60, 60, 1);"  /* borde más brillante */
            "}"
        );

        QVBoxLayout* frameLayout = new QVBoxLayout(frame);
        frameLayout->setAlignment(Qt::AlignCenter);

        QPushButton* btn = new QPushButton(frame);
        btn->setFixedSize(160, 120);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            " border: none;"
            " background-color: transparent;"
            "}"
            "QPushButton:hover {"
            " background-color: rgba(255,255,255,0.05);"
            "}"
        );

        QPixmap pix(fullPath);
        if (!pix.isNull()) {
            btn->setIcon(QIcon(pix));
            btn->setIconSize(btn->size());
        } else {
            btn->setText("No Image");
        }

        frameLayout->addWidget(btn);

        // ID del auto
        int carId = id++;
        connect(btn, &QPushButton::clicked, this, [this, carId]() {
            emit carChosen(carId);
            QMessageBox::information(this, "Car Selected",
                                     QString("You selected car %1").arg(carId + 1));
            close();
        });

        if (carId < 4) row1->addWidget(frame);
        else row2->addWidget(frame);

        // efecto brillante en el marco
        // auto* glow = new QGraphicsDropShadowEffect(frame);
        // glow->setBlurRadius(20);
        // glow->setOffset(0, 0);
        // glow->setColor(QColor(255, 0, 0));
        // frame->setGraphicsEffect(glow);
        //
        // auto* pulse = new QPropertyAnimation(glow, "blurRadius");
        // pulse->setDuration(1500);
        // pulse->setStartValue(10);
        // pulse->setEndValue(35);
        // pulse->setLoopCount(-1);
        // pulse->setEasingCurve(QEasingCurve::InOutQuad);
        // pulse->start();
    }

    mainLayout->addSpacing(50);
    mainLayout->addLayout(row1);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(row2);
    mainLayout->addSpacing(50);

    setLayout(mainLayout);

}

void CarSelectionWindow::onCarClicked(int carId) {
    emit carChosen(carId);
    QMessageBox::information(this, "Car Selected", QString("You selected car %1").arg(carId+1));
    close();
}
