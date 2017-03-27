#ifndef CONFIG_H
#define CONFIG_H


#include "unit.h"

Q_DECLARE_LOGGING_CATEGORY(fcIo)
Q_DECLARE_LOGGING_CATEGORY(fcRender)
Q_DECLARE_LOGGING_CATEGORY(fcLogic)

void logFileHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

class Config
{
public:

    Config(QString config_location, QString replay_file, QString units_file);

    QHash<uint, QHash<uint, Unit>> units;
    QHash<QString, QMap<uint, QString>> lookup;
    QHash<QString, QMap<uint, QMap<uint, QString>>> country;
    QHash<QString, QHash<QString, uint>> modifiers;
    QHash<QString, double> availability;
    QStringList versions;

private:
    QJsonDocument readJsonFile(QString file);
    void readJsonConfig(QString file);
    void readJsonUnits(QString file);

};


#endif // CONFIG_H
