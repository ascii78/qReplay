#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableWidget>
#include <QtCore>
#include "config.h"
#include "replay.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const Config& config, QWidget *parent = 0 );
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_pushButton_refresh_clicked();
    void on_pushButton_dir_clicked();

    void on_topTable_cellClicked(int row, int column);
    void on_bottomTable_cellClicked(int row, int column);

    void on_replayTable_itemSelectionChanged();

private:
    Ui::MainWindow *ui;

    void on_factionTable_cellClicked(QString table, int row, int column);
    void readSettings();
    void writeSettings();

    const Config& conf;

    QHash<QString, Replay> replays;

    QString title = "Wargame Replay Browser";

    QString replayDirSettings;
    QString replayDirDefault = QDir::home().absolutePath() + "/Saved Games/EugenSystems/WarGame3";
    QString replayDir;

    QStringList types = QStringList(
        {"Logistics", "Infantry", "Support", "Armor", "Recon", "Vehicle", "Helicopter", "Plane", "Navy" });

    QLabel *statusLabel;



};

#endif // MAINWINDOW_H
