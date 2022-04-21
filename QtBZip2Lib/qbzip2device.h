#ifndef QBZIP2DEVICE_H
#define QBZIP2DEVICE_H

#include <QObject>
#include <QIODevice>
#include "bzlib.h"
#include "qtbzip2lib_global.h"


#define INPUT_BUF_SIZE  65536
#define OUTPUT_BUF_SIZE 65536

const char *decodeBZ2Err(int code);


class  QTBZIP2LIBSHARED_EXPORT QBZip2Device : public QIODevice
{

public:
//	QBZip2Device(const QBZip2Device &);
    QBZip2Device(QIODevice *bzdata);
    ~QBZip2Device();
    bool isSequential() const;
    bool open(OpenMode mode);
    void close();
    void flush();
    qint64 bytesAvailable() const;
protected:
    qint64 readData(char * data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);
private:
    enum State {
        // Read state
        NotReadFirstByte,
        InStream,
        EndOfStream,
        // Write state
        NoBytesWritten,
        BytesWritten,
        // Common
        Closed,
        Error
    };
    State state;
    QIODevice* src;
    bz_stream inh, outh;
    char input_buf[INPUT_BUF_SIZE];
    char output_buf[OUTPUT_BUF_SIZE];
    Q_DISABLE_COPY(QBZip2Device)
    Q_OBJECT
signals:
    void currentPos(qint64 pos);
};


#endif // QBZIP2DEVICE_H
