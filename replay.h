#ifndef REPLAY_H
#define REPLAY_H

#include "deck.h"

class Replay
{
public:
    Replay();
    void parse(QString filename, const Config& config);
    Deck getDeckById(QString id);
    QMap<uint, QList<Deck>> decks;
    QMap<QString, QString> game;

private:
    QMap<QString, QVariant> datamap;

};

#endif // REPLAY_H
