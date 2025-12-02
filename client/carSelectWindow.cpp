#include "carSelectWindow.h"
#include <QPixmap>
#include <QMessageBox>
#include <QPalette>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QLabel>
#include <QProgressBar>

// Estructura para las estadísticas de cada auto
struct CarStats {
    float control;
    float health;
    float maxSpeed;
};

// Estadísticas de los 7 autos (ford, mazda, corolla, bmw, jeep, civic, truck)
static const CarStats carStats[7] = {
    {40.0f, 90.0f, 50.0f},   // ford
    {70.0f, 80.0f, 70.0f},   // mazda
    {55.0f, 100.0f, 50.0f},  // corolla
    {70.0f, 70.0f, 50.0f},   // bmw
    {70.0f, 110.0f, 50.0f},  // jeep
    {70.0f, 100.0f, 50.0f},  // civic
    {90.0f, 150.0f, 50.0f}   // truck
};

CarSelectionWindow::CarSelectionWindow(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle("Select Your Car");
    resize(800, 600);

    // background
    QPixmap bg("assets/need-for-speed/lobbyImg/wall3.jpg");
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
        "car1.png", "car2.png", "car3.png", "car4.png",
        "car5.png", "car6.png", "car7.png"
    };

    int id = 0;
    for (const QString& imgName : carImages) {
        QString fullPath =
            "assets/need-for-speed/lobbyImg/" + imgName;

        // Marco
        QFrame* frame = new QFrame(this);
        frame->setFixedSize(180, 240);
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
        frameLayout->setAlignment(Qt::AlignTop);
        frameLayout->setSpacing(5);
        frameLayout->setContentsMargins(10, 10, 10, 10);

        QPushButton* btn = new QPushButton(frame);
        btn->setFixedSize(160, 100);
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
        
        // Agregar estadísticas
        const CarStats& stats = carStats[id];
        
        // Crear función helper para crear barra de stat
        auto createStatBar = [](const QString& label, float value, float maxValue, const QString& color) -> QWidget* {
            QWidget* statWidget = new QWidget();
            QHBoxLayout* statLayout = new QHBoxLayout(statWidget);
            statLayout->setContentsMargins(0, 0, 0, 0);
            statLayout->setSpacing(5);
            
            QLabel* statLabel = new QLabel(label);
            statLabel->setStyleSheet("color: white; font-size: 10px; font-weight: bold;");
            statLabel->setFixedWidth(35);
            
            QFrame* barBg = new QFrame();
            barBg->setFixedHeight(8);
            barBg->setStyleSheet("background-color: rgba(50, 50, 50, 0.8); border-radius: 4px;");
            
            QFrame* barFill = new QFrame(barBg);
            int fillWidth = static_cast<int>((value / maxValue) * 100);
            barFill->setFixedSize(fillWidth, 8);
            barFill->setStyleSheet(QString("background-color: %1; border-radius: 4px;").arg(color));
            
            statLayout->addWidget(statLabel);
            statLayout->addWidget(barBg);
            
            return statWidget;
        };
        
        frameLayout->addSpacing(5);
        frameLayout->addWidget(createStatBar("CTRL", stats.control, 100.0f, "rgba(100, 150, 255, 0.9)"));
        frameLayout->addWidget(createStatBar("HP", stats.health, 150.0f, "rgba(255, 100, 100, 0.9)"));
        frameLayout->addWidget(createStatBar("SPD", stats.maxSpeed, 70.0f, "rgba(100, 255, 150, 0.9)"));

        // ID del auto
        int carId = id++;
        connect(btn, &QPushButton::clicked, this, [this, carId, pix]() {
            CarConfirmDialog dialog(pix, this);

            int result = dialog.exec();
            if (result == 1) {  // Confirmó
                    emit carChosen(carId);
                    close();
                }
        });

        if (carId < 4) row1->addWidget(frame);
        else row2->addWidget(frame);
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
