#include <QtCore>

#include "replay.h"

Replay::Replay()
{
}


void Replay::parse(QString filename, const Config& config) {

    QFile file(filename);

    if(!file.open(QFile::ReadOnly)) {
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    int beg = text.indexOf("{\"game");
    int end = text.indexOf("}}")-beg+2;

    QString t = text.mid(beg, end);

    QJsonDocument jdoc = QJsonDocument::fromJson(t.toUtf8());
    QVariant j = jdoc.toVariant();
    QMap<QString, QVariant> replay =  j.value<QVariantMap>();

    for(auto p : replay.keys())
    {
        if (p.startsWith("game")) {
            QMap<QString, QVariant> gameMap = replay.value(p).toMap();
            QMapIterator<QString, QVariant> i(gameMap);
            while (i.hasNext()) {
                i.next();
                game[i.key()] = i.value().toString();
            }

        }

        if(p.startsWith("player_")) {
            QMap<QString, QVariant> player = replay.value(p).toMap();

            Deck d(player.value("PlayerDeckContent").toString(), config, game["Version"]);

            d.player_level = player.value("PlayerLevel").toString();
            d.player_name = player.value("PlayerName").toString().toLocal8Bit();
            d.player_deckname = player.value("PlayerDeckName").toString().toLocal8Bit();
            d.player_id = p;

            qDebug() << ">" << d.player_name << d.player_deckname;

            uint alliance = player.value("PlayerAlliance").toUInt();
            // cannot make hash based on id, deleted operator, probably because of
            // const config
            decks[alliance].append(d);

        }
    }
}

Deck Replay::getDeckById(QString id) {
    for(auto alliance : decks.keys()) {
        for(auto deck : decks[alliance]) {
            if(deck.player_id == id) {
                return deck;
            }
        }
    }
}
