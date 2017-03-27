#include "deck.h"
#include "config.h"
#include "card.h"
#include "unit.h"

Deck::Deck(QString deckString, const Config& config, QString version):
    conf(config),
    string(deckString),
    version(version)

{
    getDeck();
}

uint Deck::getBits( const QByteArray& array, uint from, uint to )
{

    QBitArray bits(array.count()*8);

    // Convert from QByteArray to QBitArray
    for(int i=0; i<array.count(); ++i) {
        for(int b=0; b<8;b++) {
            bits.setBit( i*8+b, array.at(i)&(1<<(7-b)) );
        }
    }

    // Convert bits back to decimal
    uint key = 0;
    for(uint i = from; i <= to ; i++)
    {
        key <<= 1;
        key += (int)bits.at(i);
    }
    return key;

}

uint Deck::getInt(uint size) {

    uint key;
    key = this->getBits(deckBytes, bitCursor, size+bitCursor-1);
    bitCursor += size;
    return key;

}

QString Deck::getLookup(uint size, QMap<uint, QString> lookup) {

    uint key;
    key =  getInt(size);
    if(lookup[key] != "") {
        return lookup[key];
    } else {
        return QString::number(key);
    }

}

void Deck::getDeck() {

    deckBytes = QByteArray::fromBase64(string.toUtf8());

    nfaction = getInt(2);
    faction = conf.lookup["faction"][nfaction];

    ncountry = getInt(10);

    if(!conf.versions.contains(version)) {
        country = conf.country["0"][nfaction][ncountry];
        qCDebug(fcIo) << version << conf.versions[0] << ncountry << country;
    } else {
        country = conf.country[version][nfaction][ncountry];
    }

    type    = getLookup(3,  conf.lookup["type"]);
    era     = getLookup(2,  conf.lookup["era"]);

    qDebug() << "+" << string << version << game_mode << faction << nfaction << country << type;

    transport2 = getInt(4);
    transport1 = getInt(5);

    for(uint u=0; u<transport2; u++) {

        Card card;
        card.vtrn = getInt(3);
        card.unit = conf.units[nfaction][getInt(11)];
        card.trns = conf.units[nfaction][getInt(11)];
        card.lacr = conf.units[nfaction][getInt(11)];
        card.type = type;
        card.process(conf, country);

        units["Navy"].append(card);

    }

    for(uint u=0; u<transport1; u++) {

        Card card;
        card.vtrn = getInt(3);
        card.unit = conf.units[nfaction][getInt(11)];
        card.trns = conf.units[nfaction][getInt(11)];
        card.type = type;
        card.process(conf, country);
        if(card.amph) {
            units["Navy"].append(card);
        } else {
            units[card.unit.category].append(card);
        }

    }

    uint remaining = ((deckBytes.size()*8)-bitCursor)/14;

    for(uint u=0; u<remaining; u++) {

        Card card;
        card.vtrn = getInt(3);
        card.unit = conf.units[nfaction][getInt(11)];
        card.type = type;
        card.process(conf, country);

        units[card.unit.category].append(card);


    }
}
