#include "card.h"

Card::Card()
{
}

void Card::process(const Config& conf, QString country) {

    // Set the amphibious toggle, since we don't get it from the
    // calling deck function
    if((trns.name == "YUQING") || trns.name == "LCM 8" || lacr.name == "YUQING" || lacr.name == "LCM 8") {
        amph = true;
        qCDebug(fcIo) << unit.name << vtrn << unit.veterancy << country << unit.category;
    }

    // for specialized decks the veterancy modifier is already in the
    // deckstring, but if we use normal quantity it comes out a step to high,
    // so we need to subtract the modifier for the correct quantity, not
    // needed for navy units
    uint adjust_amount = vtrn;
    if(conf.modifiers.contains(type)) {
        if(conf.modifiers[type].contains(unit.category) && !amph) {
            adjust_amount = vtrn - conf.modifiers[type][unit.category];
        }
    }

    // Set the veterancy text
    unit.veterancy = conf.lookup["veterancy"][vtrn];

    // Each country has it's own availability bonus, not applicable for navy units
    if(unit.category != "Navy" && !amph) {
        unit.amount = qRound( unit.availability_list[adjust_amount] * conf.availability[country]);
    } else {
        unit.amount = unit.availability_list[vtrn];
    }

    // However if a unit on a general deck has only 1 availability on elite, and we're
    // using airborne lowering the availability get's us at 0...
    // TODO: should be better but looks only planes are affected
    if(unit.amount == 0 && unit.category == "Plane" && unit.veterancy == "Elite") {
        unit.amount = 1;
    }

    // Set the transport variables
    if (trns.name != "") {
        trns.veterancy = conf.lookup["veterancy"][vtrn];
        // More expensive transports restrict the availability for unit, If
        // availability of transport is less then availability of unit, use
        // transport availability
        if(trns.availability_list[adjust_amount] < unit.availability_list[adjust_amount]) {
            if (unit.category != "Navy" && !amph) {
                // Still add the availability bonus if it's not a navy unit
                unit.amount = qRound( trns.availability_list[adjust_amount] * conf.availability[country]);
            } else {
                unit.amount = trns.availability_list[adjust_amount];
            }
        }
        trns.amount = unit.amount;
    }

    // Set landing craft amounts
    if (lacr.name != "") {
        lacr.veterancy = conf.lookup["veterancy"][vtrn];
        lacr.amount = unit.amount;
    }

    // Set prettified price names for UI
    if (lacr.name != "")
    {
        if (trns.name != "") {
            price_text.sprintf("%d + %d + 5", unit.price, trns.price);
        } else {
            price_text.sprintf("%d + 5", unit.price);
        }
    } else {
        if (trns.name != "") {
            price_text.sprintf("%d + %d", unit.price, trns.price);
        } else {
            price_text.sprintf("%d", unit.price);
        }
    }

}
