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
    explicit WaitingWindow(QWidget* parent = nullptr);
    void setMessage(const QString& msg);

    signals:
    void cancelled();
    void gameShouldStart();

private
    slots:
    void onCancelClicked();

private:
    QLabel *loadingLabel;
    QLabel *textLabel;
    QPushButton *cancelButton;
};

#endif //WAITINGWINDOW_H
