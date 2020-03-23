#include "NVParser.h"
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <iostream>
#include <QMapIterator>

using namespace std;

#define HEADER_MIN_Y_BORDER 540
#define FOOTER_MAX_Y_BORDER 14
#define LINE_DIST           10

int PID_X =              14;
int OWNERNAME_X        = 87;
int REPORTEDADDRESS_X  = 168;
int HOLDERNAME_X       = 317;
int REPYEAR_X          = 423;
int PROPTYPE_X         = 487;
int CASH_X             = 560;
int ISSUENAME_X        = 624;
int SHARES_X           = 724;
int SK_X               = 761;


NVParser::NVParser() : m_InFileName("C:/webwareTech/uprs/temp/output_nv_2020.txt"),
    m_out(0), m_outFile(0), totalCash(0), nrPages(0), newRecord(true),
    firstRecordFlushed(false), process(true), nrRecords(0)
{
    pagesRx = new QRegExp("^Page  [0-9]{1,6}$");
    totalCash = nrPages = 0;

    m_logFile = new QFile("log.txt");
    m_logFile->open(QFile::WriteOnly);
    m_logStream = new QTextStream(m_logFile);
}

void NVParser::startParsing()
{
    cout << "Started parsing ..." << endl;

    QFile fileIn(m_InFileName);
    fileIn.open(QFile::ReadOnly);
    cout << "Opened Input file: " << m_InFileName.toStdString() << endl;

    m_outFile = new QFile(m_OutFileName);
    m_outFile->open(QFile::WriteOnly);
    m_out = new QTextStream(m_outFile);
    cout << "Opened Output file: " << m_OutFileName.toStdString() << endl;

    *m_out << "PID, OwnerName, ReportedAddress, HolderName, ReportYear, PropertyType, Cash, IssueNum, Shares, SK";

    while(!fileIn.atEnd() && process) {
        const QString line = fileIn.readLine().trimmed();

        if(!line.isEmpty())
            parseLine(line);
    }

    // Flushing the last record
    flushRecord();
    fileIn.close();

    *m_logStream << "Total nr records: " << getTotalNrRecords();
}

void NVParser::parseLine(const QString& line)
{
    if(pagesRx->exactMatch(line)) {
        ++nrPages;
        recCoords.reset();

        if( nrPages % 10000 == 0 )
            cout << "Processed " << nrPages << " pages" << endl;
    }
    else {
        const QStringList list = line.split(' ', QString::SkipEmptyParts);
        cout << "is list an array? " << line.toStdString() << "  its size: " << list.size() << endl;
        if (list.size() >= 7) {
            Q_ASSERT(list.size() >= 7);
            Q_ASSERT(list.at(4) == "0");
            Q_ASSERT(list.at(5) == "Aria:" || list.at(5) == "ArBo:");

            bool ok;

            // Extracting the Word's bounting rectangle coordinates.
            rect.x = qRound( list.at(0).toDouble(&ok) ); Q_ASSERT(ok);
            rect.y = qRound( list.at(1).toDouble(&ok) ); Q_ASSERT(ok);
            rect.h = qRound( list.at(2).toDouble(&ok) ); Q_ASSERT(ok);
            rect.w = qRound( list.at(3).toDouble(&ok) ); Q_ASSERT(ok);

            // We reached the end of the file and don't need to process further.
            if(nrPages == 779 && rect.y < 42) {
                process = false;
                return;
            }

            // We're processing only words that are not in the header or the footer.
            if( rect.y < HEADER_MIN_Y_BORDER &&
                rect.y > FOOTER_MAX_Y_BORDER )
            {
                QString word = "";
                for(int i = 6; i < list.size() - 1; ++i)
                    word += list.at(i) + " ";

                word += list.last();

                processWord(word);
            }
        }

    }
}

void NVParser::processWord(const QString& word)
{
    // If true we enter a new record
    if( rect.x >= PID_X && rect.x < OWNERNAME_X - 2 )
    {
        if(!record.propertyID.isEmpty() && !record.ownerName.isEmpty()) {
            flushRecord();

            //recCoords.maxOwnerY = rect.y;
        }

        //recCoords.maxOwnerY = qMax(recCoords.maxOwnerY, rect.y);

        if(record.propertyID.isEmpty())
            record.propertyID = word;
        else
            record.propertyID += "" + word;

        return;
    }

    // Address information(reportedAddress)

    if (rect.x >= OWNERNAME_X && rect.x < REPORTEDADDRESS_X - 2) {
        if(record.ownerName.isEmpty())
            record.ownerName = word.trimmed();
        else
            record.ownerName += " " + word.trimmed();
    } else if( rect.x >= REPORTEDADDRESS_X && rect.x < HOLDERNAME_X - 2 ) {
        if(record.reportedAddress.isEmpty())
            record.reportedAddress = word. trimmed();
        else
            record.reportedAddress += " " + word.trimmed();
    } else if( rect.x >= HOLDERNAME_X && rect.x < REPYEAR_X - 2 ) {
        if(record.holderName.isEmpty())
            record.holderName = word.trimmed();
        else
            record.holderName += " " + word.trimmed();
    } else if( rect.x >= REPYEAR_X && rect.x < PROPTYPE_X) {
        if(record.reportYear.isEmpty())
            record.reportYear = word.trimmed();
        else
            record.reportYear +=  word.trimmed();
    } else if( rect.x >= PROPTYPE_X && rect.x < CASH_X - 2) {
        if(record.propertyType.isEmpty())
            record.propertyType = word.trimmed();
        else
            record.propertyType += " " + word.trimmed();

    } else if( rect.x >= CASH_X && rect.x < ISSUENAME_X) {
        record.cash = word;

        totalCash += record.cash.toDouble();

    } else if( rect.x >= ISSUENAME_X && rect.x < SHARES_X - 2) {
        record.issuerName = word;
    } else if( rect.x >= SHARES_X && rect.x < SK_X - 2) {
        record.shares = word;
    } else if( rect.x >= SK_X ) {
        record.sk = word;
    }
    /*else if( rect.x >= PROPDESC_X && rect.x < PROPID_X - 2) {
        if(record.propertyDesc.isEmpty())
            record.propertyDesc = word;
        else
            record.propertyDesc += " " + word;
    }
    else if( rect.x >= PROPID_X && rect.x < CASH_X - 2) {
        record.propertyID = word;
    }
    else if( rect.x >= CASH_X && rect.x < SHARES_X - 2) {
        record.cash = word;

        bool ok;
        const double cash = record.cash.toDouble(&ok); Q_ASSERT(ok);
        totalCash += cash;

        if(cash > 50000) {
            const QString logData = QString("Big cash(%1) at page %2 for %3\n")
                    .arg(cash).arg(nrPages).arg(record.owner1);
            *m_logStream << logData;
            cout << logData.toStdString();
        }
    }
    else if( rect.x >= SHARES_X ) {
        record.shares = word;
    }*/
}

void NVParser::flushRecord()
{
    // Gathering City, State and Zip code information
    //Q_ASSERT(!record.state.isEmpty());
    /*Q_ASSERT(!record.reportYear.isEmpty());
    Q_ASSERT(!record.propertyType.isEmpty());
    Q_ASSERT(!record.cash.isEmpty());
*/

    QMapIterator<int, LineData> it(record.addressMap);
    it.toBack();
    while(it.hasPrevious()) {
        it.previous();

        const int tY = it.key();
        const LineData& lineData = it.value();

        QString* pKey = 0;

        // This is an address.
        if( tY > recCoords.prevStateY )
            pKey = &record.reportedAddress;


        QMap<int, QString>::const_iterator itAddress = lineData.constBegin();
        while(itAddress != lineData.constEnd()) {

            if(pKey->isEmpty())
                *pKey = itAddress.value();
            else
                *pKey += " " + itAddress.value();

            ++itAddress;
        }
    }

    *m_out << "\r\n" << record.propertyID << ',' << '"' << record.ownerName << '"' << ','
          << '"' << record.reportedAddress << '"' << ','
          << '"' << record.holderName << '"' << ',' << record.reportYear << ',' << '"' << record.propertyType << '"' << ','
          << '"' << record.cash << '"' << ',' << '"' << record.issuerName << '"' << ','
          << '"' << record.shares << '"' << ',' << record.sk;

    ++nrRecords;
    record.reset();
}

double NVParser::getTotalCash() const
{
    return totalCash;
}

uint NVParser::getProcessedNrPages() const
{
    return nrPages;
}

uint NVParser::getTotalNrRecords() const
{
    return nrRecords;
}

void NVParser::setInputFileName(const QString& fileName)
{
    m_InFileName = fileName;
}

void NVParser::setOutputFileName(const QString& fileName)
{
    m_OutFileName = fileName;
}

NVParser::~NVParser()
{
    delete pagesRx;

    m_out->flush();
    m_logStream->flush();

    delete m_out;
    delete m_outFile;

    delete m_logStream;
    delete m_logFile;
}
