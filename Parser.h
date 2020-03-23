#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QList>
#include "Rectangle.h"
#include "Record.h"
#include "RecordCoords.h"

class QTextStream;
class QFile;
class QRegExp;

class Parser
{
public:
    Parser();
    ~Parser();

    void setInputFileName(const QString& fileName);
    void setOutputFileName(const QString& fileName);

    void startParsing();

    double getTotalCash() const;
    uint getProcessedNrPages() const;
    uint getTotalNrRecords() const;

private:
    void parseLine(const QString& line);
    void processWord(const QString& word);
    void flushRecord();

private:
    QString m_InFileName;
    QString m_OutFileName;

    QTextStream* m_out;
    QFile* m_outFile;
    QFile* m_logFile;
    QTextStream* m_logStream;
    double totalCash;
    uint nrPages;
    uint nrRecords;

    QRegExp* pagesRx;
    QRegExp* digitRx;

    Rectangle rect;
    Record record;

    bool newRecord;
    RecordCoords recCoords;
    bool firstRecordFlushed;
    bool process;
};

#endif // PARSER_H
