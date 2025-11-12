#include "waitingWindow.h"

#include <iostream>
#include <QPalette>
#include <QPixmap>
#include <QMessageBox>

WaitingWindow::WaitingWindow(const QString& playerName, QWidget *parent)
    : QWidget(parent), playerName(playerName) {

    setWindowTitle("Waiting for Players...");
    resize(800, 600);

    // Fondo del lobby
    QPixmap background("/home/alex/Documents/TP-Final-Taller_2C2025/v2.1/TP-Taller-G7/assets/need-for-speed/lobbyImg/wall3.jpg");
    if (!background.isNull()) {
        background = background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPalette palette;
        palette.setBrush(QPalette::Window, QBrush(background));
        setAutoFillBackground(true);
        setPalette(palette);
    }

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(30);
    layout->setAlignment(Qt::AlignCenter);

    // Texto principal
    textLabel = new QLabel(QString("Waiting for other racers..."), this);
    textLabel->setStyleSheet(
        "QLabel {"
        " color: white;"
        " font-size: 24px;"
        " font-weight: bold;"
        " background-color: rgba(0, 0, 0, 0.5);"
        " padding: 10px;"
        " border-radius: 10px;"
        "}"
    );
    textLabel->setAlignment(Qt::AlignCenter);

    //Animación de carga
    loadingLabel = new QLabel(this);
    QMovie *movie = new QMovie(":/assets/need-for-speed/lobbyImg/loading.gif");
    if (!movie->isValid()) {
        // fallback: texto si no hay gif
        loadingLabel->setText("Loading...");
        loadingLabel->setStyleSheet("color: white; font-size: 20px;");
    } else {
        loadingLabel->setMovie(movie);
        movie->start();
    }
    loadingLabel->setAlignment(Qt::AlignCenter);

    // Botón de cancelar
    cancelButton = new QPushButton("Cancel", this);
    cancelButton->setFixedSize(150, 40);
    cancelButton->setStyleSheet(
        "QPushButton {"
        " background-color: rgba(255, 0, 0, 0.8);"
        " color: white;"
        " font-weight: bold;"
        " font-size: 16px;"
        " border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(255, 50, 50, 0.9);"
        "}"
    );
    connect(cancelButton, &QPushButton::clicked, this, &WaitingWindow::onCancelClicked);

    layout->addWidget(textLabel);
    layout->addWidget(loadingLabel);
    layout->addWidget(cancelButton);

    setLayout(layout);
}

void WaitingWindow::onCancelClicked() {
    // Acción de cancelar
    QMessageBox::information(this, "Cancelled", "You left the lobby.");
    this->close();
}
