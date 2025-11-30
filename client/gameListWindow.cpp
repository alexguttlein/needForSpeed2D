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
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"ID", "Creator", "Players"});
    for (int i = 0; i < static_cast<int>(games.size()); ++i) {
        // Columna 0: ID
        table->setItem(i, 0, new QTableWidgetItem(QString::number(games[i].id)));

        // Columna 1: Creador
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(games[i].name)));

        // Columna 2: players / MAX_PLAYERS_IN_GAME
        table->setItem(i, 2, new QTableWidgetItem(
            QString("%1 / %2")
            .arg(games[i].players)
            .arg(Constants::MAX_PLAYERS_IN_GAME)
        ));
    }
    table->resizeColumnsToContents();
}

void GameListWindow::onRefreshClicked() {
    // solicitar lista al servidor y esperar en background
    if (!client->sendLobbyOption(Constants::INPUT_LISTAR,"",0)) {
        QMessageBox::warning(this, "Error", "No se pudo solicitar la lista de partidas.");
        return;
    }

    // esperar snapshot (fuera del hilo UI)
    (void)QtConcurrent::run([this]() {
        Snapshot snapshot = client->getSnapshotQueue().pop();
        QMetaObject::invokeMethod(this, [this, snapshot]() {
            if (snapshot.gameList.empty()) {
                QMessageBox::information(this, "Info", "No hay partidas activas.");
                return;
            }
            loadGameList(snapshot.gameList);
        }, Qt::QueuedConnection);
    });
}

void GameListWindow::onCancelClicked() {
    this->close();
}

void GameListWindow::onTableDoubleClicked(int row, int) {
    uint32_t gameId = table->item(row, 0)->text().toUInt();
    QString creator = table->item(row, 1)->text();
    emit gameSelected(gameId, creator);
    this->close();
}
