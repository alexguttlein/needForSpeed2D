#include "waitingWindow.h"
#include <iostream>
#include <QPalette>
#include <QPixmap>
#include <QMessageBox>
#include <QMovie>

WaitingWindow::WaitingWindow(QWidget* parent)
    : QWidget(parent), eventQueuePtr(nullptr) {

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
    connect(cancelButton, &QPushButton::clicked, this, &WaitingWindow::onCancelClicked);

    layout->addWidget(textLabel);
    layout->addWidget(loadingLabel);
    layout->addWidget(cancelButton);

    setLayout(layout);
}

WaitingWindow::WaitingWindow(Queue<Event>& eventQueueRef, QWidget* parent)
    : WaitingWindow(parent) {
    eventQueuePtr = &eventQueueRef;
}

void WaitingWindow::setMessage(const QString& msg) {
    textLabel->setText(msg);
}

void WaitingWindow::onCancelClicked() {
    QMessageBox::information(this, "Cancelled", "You left the lobby.");
    this->close();
}
