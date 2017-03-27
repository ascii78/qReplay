#ifndef CARD_H
#define CARD_H

#include "config.h"

class Card
{
public:
    Card();
    void process(const Config& conf, QString country);
    operator QString() const { return unit.name; }

    Unit unit;
    Unit trns;
    Unit lacr;

    uint vtrn = 0;
    bool amph = false;
    QString type;
    QString price_text;



};
#endif // CARD_H
