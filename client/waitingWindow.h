#ifndef WAITINGWINDOW_H
#define WAITINGWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMovie>
#include <QPushButton>
#include <QString>
#include <QTimer>

class WaitingWindow : public QWidget {
    Q_OBJECT

public:
    explicit WaitingWindow(const QString& playerName, QWidget *parent = nullptr);

private slots:
    void onCancelClicked();

private:
    QLabel *loadingLabel;
    QLabel *textLabel;
    QPushButton *cancelButton;
    QString playerName;
};

#endif //WAITINGWINDOW_H
