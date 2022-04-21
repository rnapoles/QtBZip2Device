#include <QCoreApplication>
#include <QCoreApplication>
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlInputSource>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <qbzip2device.h>

int main(int argc, char *argv[])
{

    //QCoreApplication a(argc, argv);

    QFile file("input.xml.bz2");
    //emit FileSize(file.size());

    if(file.exists()){
        QBZip2Device compressor(&file);
        if(compressor.open(QIODevice::ReadOnly)){
            //connect(&compressor,SIGNAL(CurrentPos(int)),this,SLOT(SlotReportProgress(int)));

            /*
            QXmlSimpleReader reader;
            //reader.setContentHandler(&handler);
            //reader.setErrorHandler(&handler);
            QXmlInputSource *source = new QXmlInputSource(&compressor);

            bool ok = reader.parse(source);
            if(ok){
                while (source->) {
                    QString line = source->next();
                    qDebug() << line << "\n";
                }
            } else {
                qDebug() << "Error processing file" << "\n";
            }
            */

            QTextStream in(&compressor);
            QString line = in.readLine();
            while (!line.isNull()) {
                line = in.readLine();
                qDebug() << line << "\n";
            }


        } else {
            qDebug() << "Error opening:" << file.fileName() << "\nError" << file.errorString();
        }

    } else {
        qDebug() << "File:" << file.fileName() << " not exists\n";
    }

    /*
    QBuffer data;
    QtIOCompressor compressor(&data);
    compressor.open(QIODevice::WriteOnly);
    compressor.write(text.toUtf8());
    compressor.close();
    */

    return 0;
    //return a.exec();
}

