#ifndef WAITINGWINDOW_H
#define WAITINGWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMovie>
#include <QPushButton>
#include <QString>
#include <QTimer>

#include "../common/queue.h"
#include "../common/eventType.h"

class WaitingWindow : public QWidget {
    Q_OBJECT

public:
    // explicit WaitingWindow(const QString& playerName, QWidget *parent = nullptr);
    explicit WaitingWindow(QWidget* parent = nullptr);
    explicit WaitingWindow(Queue<Event>& eventQueueRef, QWidget* parent = nullptr);

    void setMessage(const QString& msg);

private slots:
    void onCancelClicked();

private:
    QLabel *loadingLabel;
    QLabel *textLabel;
    QPushButton *cancelButton;
    Queue<Event>* eventQueuePtr = nullptr;
};

#endif //WAITINGWINDOW_H
