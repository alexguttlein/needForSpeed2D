#include "waitingWindow.h"
#include <iostream>
#include <QPalette>
#include <QPixmap>
#include <QMessageBox>
#include <QMovie>

WaitingWindow::WaitingWindow(bool isCreator, QWidget* parent)
    : QWidget(parent), isCreator(isCreator) {

    setWindowTitle("Waiting for Players...");
    resize(800, 600);

    QPixmap bg("assets/need-for-speed/lobbyImg/wall3.jpg");
    if (!bg.isNull()) {
        bg = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPalette pal;
        pal.setBrush(QPalette::Window, QBrush(bg));
        setAutoFillBackground(true);
        setPalette(pal);
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignCenter);

    textGameIdLabel = new QLabel("", this);
    textGameIdLabel->setStyleSheet(
        "QLabel { color:white; font-size:26px; font-weight:bold; "
        "background: rgba(0,0,0,0.5); padding:10px; border-radius:10px; }");
    textGameIdLabel->setAlignment(Qt::AlignCenter);

    layout->setSpacing(20);

    textLabel = new QLabel("Waiting for other racers...", this);
    textLabel->setStyleSheet("QLabel { color:white; font-size:24px; font-weight:bold; background: rgba(0,0,0,0.5); padding:10px; border-radius:10px; }");
    textLabel->setAlignment(Qt::AlignCenter);

    loadingLabel = new QLabel(this);
    QMovie* movie = new QMovie(":/assets/need-for-speed/lobbyImg/loading.gif");
    if (!movie->isValid()) {
        loadingLabel->setText("Loading...");
        loadingLabel->setStyleSheet("color:white; font-size:20px;");
    } else {
        loadingLabel->setMovie(movie);
        movie->start();
    }
    loadingLabel->setAlignment(Qt::AlignCenter);

    cancelButton = new QPushButton("Cancel", this);
    cancelButton->setFixedSize(150, 40);
    cancelButton->setStyleSheet("QPushButton { background-color: rgba(255,0,0,0.8); color: white; font-weight:bold; }");
    cancelButton->hide();
    connect(cancelButton, &QPushButton::clicked, this, &WaitingWindow::onCancelClicked);

    startGameButton = new QPushButton("Start Game", this);
    startGameButton->setFixedSize(180, 70);
    startGameButton->setStyleSheet("QPushButton { background-color: red; color: white; font-weight:bold; font-size: 25px;}");
    startGameButton->setCursor(Qt::PointingHandCursor);
    connect(startGameButton, &QPushButton::clicked, this, &WaitingWindow::startGamePressed);

    if (!isCreator) {
        startGameButton->hide();
    }

    QHBoxLayout* buttonRow = new QHBoxLayout();
    buttonRow->addStretch();
    buttonRow->addWidget(startGameButton);
    buttonRow->addStretch();

    layout->addWidget(textGameIdLabel);
    layout->addWidget(textLabel);
    layout->addWidget(loadingLabel);
    layout->addWidget(cancelButton);
    layout->addLayout(buttonRow);

    setLayout(layout);
}

void WaitingWindow::onCancelClicked() {
    emit cancelled();
    close();
}

void WaitingWindow::setMessage(const QString& msg) {
    textLabel->setText(msg);
}

void WaitingWindow::setGameInfo(uint32_t gameId) {
    QString msg = QString("You are on Game: %1").arg(gameId);
    textGameIdLabel->setText(msg);
}
