#ifndef DECK_H
#define DECK_H

#include <QtCore>
#include "config.h"
#include "card.h"


class Deck {

public:

    Deck(QString deckString, const Config& config, QString version);

    QString player_name;
    QString player_level;
    QString player_deckname;
    QString player_id;
    QString version;
    QString string;
    QString faction;
    QString country;
    QString type;
    QString era;
    QString game_mode;

    uint transport2;
    uint transport1;
    uint nfaction;
    uint ncountry;

    QHash<QString, QList<Card>> units;
    operator QString() const { return string; }

private:

    const Config& conf;
    uint bitCursor=0;
    QByteArray deckBytes;

    uint getBits( const QByteArray& array, uint from, uint to );
    uint getInt(uint size);
    QString getLookup(uint size, QMap<uint, QString> lookup );
    void getDeck();

};

#endif // DECK_H
