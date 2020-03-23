#include <iostream>
#include "NVParser.h"
#include <QTime>

using namespace std;

int main(int argc, char *argv[])
{
    QTime startTime = QTime::currentTime();

    NVParser*  parser = new NVParser;

//#ifndef QT_NO_DEBUG
//    parser->setInputFileName("G:/Kituri/PDF/CLS400PreWIN32/bin/output.txt");
//    parser->setOutputFileName("Extracted_Data.csv");
//#else

//    if( argc != 3 ) {
//        cout << "Invalid nr of arguments. Please specify Input and Output files!";
//        return 1;
//    }

//    parser->setInputFileName(argv[1]);
//    parser->setOutputFileName(argv[2]);
//#endif

    if( argc == 3 ) {
        parser->setInputFileName(argv[1]);
        parser->setOutputFileName(argv[2]);
    }
    else {
        parser->setInputFileName("C:/webwareTech/uprs/temp/output_nv_2020.txt");
        parser->setOutputFileName("C:/webwareTech/uprs/temp/2020/Extracted_Data_2020_nv.csv");
    }

    parser->startParsing();

    cout << "Total nr pages processed: " << parser->getProcessedNrPages() << endl;
    cout << "Total nr records: " << parser->getTotalNrRecords() << endl;
    cout << "Total cash: " << parser->getTotalCash() << endl;
    cout << "Total processed time: " << startTime.secsTo(QTime::currentTime()) << endl;

    delete parser;
    return 0;
}
