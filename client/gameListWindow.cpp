#include "gameListWindow.h"
#include "client_class.h"
#include <QHeaderView>
#include <QMessageBox>

GameListWindow::GameListWindow(Client* client, QWidget* parent)
    : QWidget(parent), client(client)
{
    setWindowTitle("Select a Game to Join");
    resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    table = new QTableWidget(this);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Game ID", "Players"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(table, &QTableWidget::cellDoubleClicked,
            this, &GameListWindow::onTableDoubleClicked);

    refreshButton = new QPushButton("Refresh", this);
    cancelButton  = new QPushButton("Cancel", this);

    connect(refreshButton, &QPushButton::clicked, this, &GameListWindow::onRefreshClicked);
    connect(cancelButton, &QPushButton::clicked, this, &GameListWindow::onCancelClicked);

    layout->addWidget(table);
    layout->addWidget(refreshButton);
    layout->addWidget(cancelButton);

    setLayout(layout);
}

void GameListWindow::loadGameList(const std::vector<GameInfo>& games) {
    table->setRowCount(static_cast<int>(games.size()));
    for (int i = 0; i < static_cast<int>(games.size()); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(games[i].id)));
        table->setItem(i, 1, new QTableWidgetItem(QString("%1 / %2")
                         .arg(games[i].players)
                         .arg(Constants::MAX_PLAYERS_IN_GAME)));
    }
}

void GameListWindow::onRefreshClicked() {
    auto snapshotOpt = client->getSnapshotQueue().pop();
    if (!snapshotOpt.gameList.data()) {
        QMessageBox::warning(this, "Error", "No se pudo obtener la lista de juegos.");
        return;
    }
    loadGameList(snapshotOpt.gameList);
}

void GameListWindow::onCancelClicked() {
    this->close();
}

void GameListWindow::onTableDoubleClicked(int row, int) {
    uint32_t gameId = table->item(row, 0)->text().toUInt();
    emit gameSelected(gameId);
    this->close();
}
