#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>

#include "config.h"

QScopedPointer<QFile> m_logFile;

Q_LOGGING_CATEGORY(fcIo, "fc.io")
Q_LOGGING_CATEGORY(fcRender, "fc.render")
Q_LOGGING_CATEGORY(fcLogic, "fc.logic")

Config::Config(QString config_location, QString replay_file, QString units_file)
{

    readJsonConfig(config_location + "/" + replay_file);
    readJsonUnits(config_location + "/" + units_file);

    modifiers["Motorized"]  = {{"Recon", 1}, {"Infantry", 1}, {"Vehicle", 1}};
    modifiers["Armored"]    = {{"Armor", 2}};
    modifiers["Support"]    = {{"Support", 1}};
    modifiers["Marines"]    = {{"Plane", 1}, {"Infantry", 1}};
    modifiers["Mechanized"] = {{"Vehicle", 1}, {"Infantry", 1}};
    modifiers["Airborne"]   = {{"Infantry", 1}, {"Helicopter", 1}, {"Plane", 1}};

//    m_logFile.reset(new QFile("C:/temp/File.txt"));
//    m_logFile.data()->open(QFile::Append | QFile::Text);
//    qInstallMessageHandler(logFileHandler);

    QLoggingCategory::setFilterRules("*.debug=false\n"
                                     "fc.io.debug=true");

}

QJsonDocument Config::readJsonFile(QString file) {

    QFile mFile(file);
    if(!mFile.open(QFile::ReadOnly)) {
        qDebug() << "File Error" << mFile.errorString();
    }

    QTextStream in(&mFile);
    QString mText = in.readAll();
    mFile.close();

    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(mText.toUtf8(), &jerror);
    if(jerror.error != QJsonParseError::NoError) {
        qDebug() << "Parse Error" << jerror.errorString();
    }
    return jdoc;

}

void Config::readJsonConfig(QString file) {

    QVariant j = readJsonFile(file).toVariant();

    QMap<QString, QVariant> dataMap = j.value<QVariantMap>();
    QMap<QString, QVariant> map;

    for(auto e: QStringList { "type", "faction", "era", "veterancy", "category" }) {
        map = dataMap.value(e).toMap();
        for(auto i : map.keys()) {
            lookup[e][i.toInt()] = map.value(i).toString();
        }
    }
    map = dataMap.value("country").toMap();
    for(auto i: map.keys()) {
        versions.append(i);
        for(auto e: QStringList {"0", "1"}) {
            auto faction = map.value(i).toMap()[e].toMap();
            for(auto f: faction.keys()) {
                country[i][e.toInt()][f.toInt()] = faction[f].toString();
            }
        }
    }

    map = dataMap.value("availability").toMap();
    for(auto f: map.keys()) {
        availability[f] = map[f].toDouble();
     }
}

void Config::readJsonUnits(QString file) {

    QJsonObject j = readJsonFile(file).object();

    QMap<QString, QVariant> unitsMap;

    for(int i = 0; i <= 1; i++ ) {

        QString faction = QString::number(i);
        unitsMap =  j.value(faction).toObject().toVariantMap();

        foreach(QString key, unitsMap.keys()) {

            QMap<QString, QVariant> unit = unitsMap[key].toMap();

            Unit u;
            u.name = unit["Name"].toString();
            u.category = lookup["category"][unit["Category"].toUInt()];

            QList<QVariant> price_list = unit["Price"].toList();

            if(price_list.size() > 0) {
                u.price = price_list.first().toInt();
            } else {
                u.price = 0;
            }

            foreach(QVariant value, unit["Units"].toList())
                u.availability_list.append(value.toInt());

            units[faction.toUInt()][key.toUInt()] = u;
        }
    }
}

void logFileHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(m_logFile.data());
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }
    out << context.category << ": "
        << msg << endl;
    out.flush();
}

