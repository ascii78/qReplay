#ifndef UNIT_H
#define UNIT_H

#include <QtCore>

#include "unit.h"

class Unit
{
public:

    Unit();
    operator QString() const { return name; }

    QString name;
    QString category;
    QString veterancy;

    QList<uint> availability_list;

    uint amount;
    uint price;

};
#endif // UNIT_H
