#include <QtCore>
#include <QCoreApplication>
#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "deck.h"
#include "replay.h"

class TableNumberItem : public QTableWidgetItem
{
public:
    TableNumberItem(const QString txt = QString("0"))
        :QTableWidgetItem(txt)
    {
    }
    bool operator <(const QTableWidgetItem &other) const
    {
        if( this->text().toInt() < other.text().toInt() )
            return true;
        else
            return false;
    }
};

MainWindow::MainWindow(const Config& config, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    conf(config)

{
    ui->setupUi(this);

    this->setWindowTitle(title);

    statusLabel = new QLabel(this);
    statusLabel->setText(replayDir);
    ui->statusBar->addPermanentWidget(statusLabel);

    ui->topTable->setColumnWidth(0, 40);
    ui->topTable->setColumnWidth(2, 100);
    ui->topTable->setColumnWidth(3, 80);
    ui->topTable->setColumnWidth(4, 180);
    ui->topTable->setColumnWidth(6, 45);
    ui->topTable->setColumnHidden(6, true);
//    ui->topTable->setStyleSheet("QTableView { selection-background-color: palette(Highlight); }");

    ui->bottomTable->setColumnWidth(0, 40);
    ui->bottomTable->setColumnWidth(2, 100);
    ui->bottomTable->setColumnWidth(3, 80);
    ui->bottomTable->setColumnWidth(4, 180);
    ui->bottomTable->setColumnWidth(6, 45);
    ui->bottomTable->setColumnHidden(6, true);
//    ui->bottomTable->setStyleSheet("QTableView { selection-background-color: palette(Highlight); }");

    QHeaderView *rverticalHeader = ui->bottomTable->verticalHeader();
    rverticalHeader->sectionResizeMode(QHeaderView::Fixed);

    QHeaderView *bverticalHeader = ui->topTable->verticalHeader();
    bverticalHeader->sectionResizeMode(QHeaderView::Fixed);

    auto gameTable = ui->detailsTab->widget(0)->findChild<QTableWidget*>();

    gameTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    gameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->deckTable->setColumnWidth(1, 20);
    ui->deckTable->setColumnWidth(2, 200);
    ui->deckTable->setColumnWidth(3, 150);
    ui->deckTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    readSettings();

    this->on_pushButton_refresh_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    replayDirSettings = settings.value("replaydir", QString()).toString();

}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("replaydir", replayDir);
}


void MainWindow::on_pushButton_refresh_clicked()
{

    if(replayDir == "") {
        if(replayDirSettings != "") {
            replayDir = replayDirSettings;
        } else {
            replayDir = replayDirDefault;
        }
    }

    QDir mDir(replayDir);

    ui->replayTable->clear();
    int found = 0;
    QList<QTreeWidgetItem *> items;
    foreach (QFileInfo mItm, mDir.entryInfoList()) {
        QString fn = mItm.fileName();
        if(fn.endsWith(".wargamerpl2")) {
            found++;
            fn.chop(12);
            items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(fn)));
        }
    }

    ui->replayTable->insertTopLevelItems(0, items);
    ui->replayTable->sortByColumn(0, Qt::DescendingOrder);

    if(found == 0) {

        QString message = QString(
                    "<b>No replay files found in the replay directory:</b>"
                    "<br><br>%1<br><br>"
                    "Select the replay directory, it should contain at least "
                    "one *.wargamepl2 file<br>The default directory is usually:"
                    "<br><br>%2<br><br>").arg(replayDir).arg(replayDirDefault);

        int ret = QMessageBox::warning(this, tr("Warning"), message,
                                       QMessageBox::Ok | QMessageBox::Cancel);
        switch (ret) {
          case QMessageBox::Ok:
              this->on_pushButton_dir_clicked();
              break;
          case QMessageBox::Cancel:
              break;
          default:
              // should never be reached
              break;
        }

    } else {
        ui->replayTable->setCurrentItem(items[found-1]);
    }

}


void MainWindow::on_pushButton_dir_clicked()
{
    // QFileDialog::Directory "However, the native Windows file dialog does not
    // support displaying files in the directory chooser." we might need this
    // since we also want to view the *.wargamepl2 files when we're browsing for
    // the right directory.
    replayDir = QFileDialog::getExistingDirectory(this, tr("Open Replay Directory"),
                                                  QDir::home().absolutePath(),
                                                  QFileDialog::DontResolveSymlinks
                                                  // | QFileDialog::DontUseNativeDialog
                                                  );

    statusLabel->setText(replayDir);

    this->on_pushButton_refresh_clicked();
}


void MainWindow::on_factionTable_cellClicked(QString table, int row, int column)
{
    QTableWidgetItem *item = new QTableWidgetItem();
    QString user;

    if (!ui->bottomTable->item(row, column) || !ui->topTable->item(row, column))
        return;

    if (table == "top") {
        item = ui->topTable->item(row, column);
        user = ui->topTable->item(row, 6)->text();
    } else {
        item = ui->bottomTable->item(row, column);
        user = ui->bottomTable->item(row, 6)->text();
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(item->text());
    ui->statusBar->showMessage("Cell copied to clipboard", 4000);

    QString filename = ui->replayTable->currentItem()->text(0);
    QString filepath = QString("%1/%2.wargamerpl2").arg(replayDir).arg(filename);

    Deck deck = replays[filepath].getDeckById(user);

    ui->deckTable->setRowCount(0);
    ui->deckTable->setSortingEnabled(false);

    int d = 0;

    for(auto type: types) {

        for (auto card: deck.units[type]) {

            ui->deckTable->insertRow(ui->deckTable->rowCount());

            ui->deckTable->setItem(d, 0, new QTableWidgetItem(type));
            ui->deckTable->setItem(d, 1, new QTableWidgetItem(QString::number(card.unit.amount)));
            ui->deckTable->setItem(d, 2, new QTableWidgetItem(card.unit.name));
            ui->deckTable->setItem(d, 3, new QTableWidgetItem(card.trns.name));
            ui->deckTable->setItem(d, 4, new QTableWidgetItem(card.unit.veterancy));
            ui->deckTable->setItem(d, 5, new QTableWidgetItem(card.price_text));

            d = d + 1;
        }
    }
}

void MainWindow::on_topTable_cellClicked(int row, int column)
{
    ui->bottomTable->clearSelection();
    this->on_factionTable_cellClicked("top", row, column);
}

void MainWindow::on_bottomTable_cellClicked(int row, int column)
{
    ui->topTable->clearSelection();
    this->on_factionTable_cellClicked("bottom", row, column);
}

void MainWindow::on_replayTable_itemSelectionChanged()
{

    QString filename = ui->replayTable->currentItem()->text(0);
    QString filepath = QString("%1/%2.wargamerpl2").arg(replayDir).arg(filename);

    Replay replay;

    if (replays.contains(filepath)) {
        replay = replays[filepath];
    } else {
        replay.parse(filepath, conf);
        replays[filepath] = replay;
    }

    ui->topTable->setSortingEnabled(false);
    ui->bottomTable->setSortingEnabled(false);

    ui->topTable->clearContents();
    ui->bottomTable->clearContents();

    for(auto team : replay.decks.keys())
    {
        QTableWidget *table = new QTableWidget();

        uint c = 0;

        if(team == 0) {
            table = ui->topTable;
        } else {
            table = ui->bottomTable;
        }

        for(auto deck : replay.decks[team]) {
            table->setItem(c, 0, new TableNumberItem(deck.player_level));
            table->setItem(c, 1, new QTableWidgetItem(deck.player_name));
            if (deck.country == "") {
                table->setItem(c, 2, new QTableWidgetItem(QString::number(deck.ncountry)));
            } else {
                table->setItem(c, 2, new QTableWidgetItem(deck.country));
            }
            table->setItem(c, 3, new QTableWidgetItem(deck.type));
            table->setItem(c, 4, new QTableWidgetItem(deck.player_deckname));
            table->setItem(c, 5, new QTableWidgetItem(deck.string));
            table->setItem(c, 6, new QTableWidgetItem(deck.player_id));

            c++;
        }
    }

    uint g = 0;

    for(auto key : replay.game.keys()) {
        ui->gameTable->setItem(g, 0, new QTableWidgetItem(key));
        if (!conf.versions.contains(replay.game["Version"]) && key == "Version") {
            QString version = QString("%1 (Supported: %2)")
                    .arg(replay.game[key]).arg(conf.versions.join(","));
            ui->gameTable->setItem(g, 1, new QTableWidgetItem(version));
        } else {
            ui->gameTable->setItem(g, 1, new QTableWidgetItem(replay.game[key]));
       }
       g++;
    }

    ui->topTable->sortItems(0,Qt::DescendingOrder);
    ui->topTable->setSortingEnabled(true);
    ui->bottomTable->sortItems(0,Qt::DescendingOrder);
    ui->bottomTable->setSortingEnabled(true);

    if (!conf.versions.contains(replay.game["Version"])) {
        ui->statusBar->setStyleSheet("color: red");
        ui->statusBar->showMessage("Unsupported patch version, units, availability and country names may be wrong", 8000);
        ui->statusBar->setStyleSheet("color: black");
    } else {
        ui->statusBar->setStyleSheet("");
        ui->statusBar->showMessage("Loaded "+filepath, 4000);
    }

}

