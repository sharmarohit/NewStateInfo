#ifndef RECORD_H
#define RECORD_H

#include <QString>
#include <QMap>

typedef QMap<int, QString> LineData;        //!< Typedef for mappingg X coordinates for one line of address data.

struct Record
{
    Record() : flushed(true) {}

    QString propertyID;
    QString ownerName;
    QString reportedAddress;
    QString holderName;
    QString reportYear;
    QString propertyType;
    QString issuerName;
    QString sk;
    QString cash;
    QString shares;

    void reset()
    {
        ownerName = reportedAddress =
        holderName = reportYear = propertyType = issuerName =
        propertyID = cash = shares = sk = "";
        flushed = true;

        addressMap.clear();
    }

    bool flushed;

    QMap<int, LineData> addressMap;      //!< Maps the Y coordinates to Address info for a given record.
};

#endif // RECORD_H
