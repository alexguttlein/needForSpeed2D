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
    // explicit WaitingWindow(QWidget* parent = nullptr);
    explicit WaitingWindow(bool isCreator, QWidget* parent = nullptr);
    void setMessage(const QString& msg);
    void setGameInfo(uint32_t gameId);

    signals:
    void cancelled();
    void gameShouldStart();
    void startGamePressed();

private
    slots:
    void onCancelClicked();

private:
    bool isCreator;
    QLabel *loadingLabel;
    QLabel *textLabel;
    QLabel *textGameIdLabel;
    QPushButton *cancelButton;
    QPushButton *startGameButton;
    QLabel *loadingRace;
};

#endif //WAITINGWINDOW_H
