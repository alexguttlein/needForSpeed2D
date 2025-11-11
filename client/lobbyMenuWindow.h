#ifndef LOBBYMENUWINDOW_H
#define LOBBYMENUWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

class LobbyMenuWindow : public QWidget {
    Q_OBJECT
public:
    explicit LobbyMenuWindow(const QString &playerName, QWidget *parent = nullptr);

    QPushButton* getCreateButton();
    QPushButton* getJoinButton();
    QPushButton* getSelectCarButton();
    QString getPlayerName() const;

private:
    QPushButton *createButton;
    QPushButton *joinButton;
    QPushButton *selectCarButton;
    QString playerName;
};

#endif //LOBBYMENUWINDOW_H