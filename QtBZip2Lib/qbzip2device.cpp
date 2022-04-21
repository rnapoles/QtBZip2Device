#include "qbzip2device.h"
#include <QDebug>

#define FXERRHBZ2(ret) { int _ret=(ret); if(_ret<0) qDebug()<< QString(decodeBZ2Err(_ret)); }

const char *decodeBZ2Err(int code)
{
    switch(code)
    {
    case BZ_SEQUENCE_ERROR:
        return "BZip2 sequence error";
    case BZ_PARAM_ERROR:
        return "BZip2 parameter error";
    case BZ_MEM_ERROR:
        return "BZip2 failed to allocate memory";
    case BZ_DATA_ERROR:
        return "BZip2 data integrity failure";
    case BZ_DATA_ERROR_MAGIC:
        return "BZip2 bad magic header";
    case BZ_IO_ERROR:
        return "BZip2 i/o error";
    case BZ_UNEXPECTED_EOF:
        return "BZip2 unexpected end of file";
    case BZ_OUTBUFF_FULL:
        return "BZip2 output buffer full";
    case BZ_CONFIG_ERROR:
        return "BZip2 configuration error";
    }
    return "Unknown BZip2 error";
}

QBZip2Device::QBZip2Device(QIODevice *bzdata):src(bzdata){
    state = QBZip2Device::Closed;
}

QBZip2Device::~QBZip2Device(){
    close();
}

bool QBZip2Device::isSequential() const{
    return true;
}

bool QBZip2Device::open(OpenMode mode){

    if (isOpen()) {
        qWarning("QBZip2Device::open: device already open");
        return false;
    }

    // Check for correct mode: ReadOnly xor WriteOnly
    const bool read = (mode & ReadOnly);
    const bool write = (mode & WriteOnly);
    const bool both = (read && write);
    const bool neither = !(read || write);
    if (both || neither) {
        qWarning("QBZip2Device::open: QBZip2Device can only be opened in the ReadOnly or WriteOnly modes");
        return false;
    }

    if (src->isOpen()) {
        const OpenMode deviceMode = src->openMode();
        if (read && !(deviceMode & ReadOnly)) {
            qWarning("QBZip2Device::open: underlying device must be open in one of the ReadOnly or WriteOnly modes");
            return false;
        } else if (write && !(deviceMode & WriteOnly)) {
            qWarning("QBZip2Device::open: underlying device must be open in one of the ReadOnly or WriteOnly modes");
            return false;
        }

        // If the underlying device is closed, open it.
    } else {
        if (src->open(mode) == false) {
            //setErrorString(QT_TRANSLATE_NOOP("QBZip2Device", "Error opening underlying device: ") + src->errorString());
            QString str = "Error opening underlying device: " + src->errorString();
            qWarning(str.toLatin1());
            return false;
        }
    }

    memset(&inh, 0, sizeof(inh));
    memset(&outh, 0, sizeof(outh));


    int status;

    if (read) {
        state = QBZip2Device::NotReadFirstByte;
        status = BZ2_bzDecompressInit(&inh,0,0);
    } else {
        state = QBZip2Device::NoBytesWritten;
        status =  BZ2_bzCompressInit(&outh,9,0,0);
    }


    if (status != BZ_OK) {
        FXERRHBZ2(status);
        return false;
    }
    return QIODevice::open(mode);
}

void QBZip2Device::close(){

    if (isOpen() == false)
        return;

    // Flush and close the zlib stream.
    if (openMode() & ReadOnly) {
        state = QBZip2Device::NotReadFirstByte;
        BZ2_bzDecompressEnd(&inh);
    } else {
        if (state == QBZip2Device::BytesWritten) { // Only flush if we have written anything.
            state = QBZip2Device::NoBytesWritten;
        }
        BZ2_bzCompressEnd(&outh);
    }

    // Close the underlying device if we are managing it.
        src->close();
    setOpenMode(NotOpen);
    QIODevice::close();


}

void QBZip2Device::flush(){
    if (isOpen() == false || openMode() & ReadOnly)
        return;
}

qint64 QBZip2Device::bytesAvailable() const{

    if ((openMode() & ReadOnly) == false)
        return 0;

    if(src->atEnd()) return 0;

    qint64 numBytes = 0;

    numBytes = src->bytesAvailable();

    numBytes += QIODevice::bytesAvailable();


    if (numBytes > 0)
        return 1;
    else
        return 0;
}

qint64 QBZip2Device::readData(char * data, qint64 maxSize){

    if(!this->isOpen()) return -1;

    if (state == QBZip2Device::EndOfStream)
        return 0;

    if (state == QBZip2Device::Error)
        return -1;

    emit currentPos(QIODevice::pos());

    // We are ging to try to fill the data buffer
    inh.next_out = reinterpret_cast<char *>(data);
    inh.avail_out = maxSize;

    int status;
    do {
        // Read data if if the input buffer is empty. There could be data in the buffer
        // from a previous readData call.
        if (inh.avail_in == 0) {
            qint64 bytesAvalible = src->read(reinterpret_cast<char *>(input_buf), INPUT_BUF_SIZE);
            inh.next_in = input_buf;
            inh.avail_in = bytesAvalible;
            if (bytesAvalible == -1) {
                state = QBZip2Device::Error;
                         return -1;
            }

            if (state != QBZip2Device::InStream) {            
            if(bytesAvalible == 0)
                return 0;
               else if (bytesAvalible > 0)
                    state = QBZip2Device::InStream;
            }
        }

        // Decompress.
        status = BZ2_bzDecompress(&inh);

        switch (status) {
            //case Z_NEED_DICT:
            case BZ_DATA_ERROR:
            case BZ_MEM_ERROR:
                state = QBZip2Device::Error;              
                return -1;
            break;
        }
        // Loop util data buffer is full or we reach the end of the input stream.
    } while (inh.avail_out != 0 && status != BZ_STREAM_END);

    if (status == BZ_STREAM_END) {
        state = QBZip2Device::EndOfStream;

        // Unget any data left in the read buffer.
        for (int i = inh.avail_in;  i >= 0; --i)
            src->ungetChar(*reinterpret_cast<char *>(inh.next_in + i));
    }

    const qint64 outputSize = maxSize - inh.avail_out;
    return outputSize;


}

qint64 QBZip2Device::writeData(const char * data, qint64 maxSize){

    int ret;
    char outbuffer[16384];
    outh.next_in=const_cast<char*>(data);
    outh.avail_in=(quint64) maxSize;
    do
    {
        outh.next_out=outbuffer;
        outh.avail_out=sizeof(outbuffer);
        ret=BZ2_bzCompress(&outh, BZ_RUN);

        FXERRHBZ2(ret);

        switch(ret)
        {
        case BZ_SEQUENCE_ERROR:
            return -1;
        case BZ_PARAM_ERROR:
            return -1;
        case BZ_MEM_ERROR:
            return -1;
        case BZ_DATA_ERROR:
            return -1;
        case BZ_DATA_ERROR_MAGIC:
            return -1;
        case BZ_IO_ERROR:
            return -1;
        case BZ_UNEXPECTED_EOF:
            return -1;
        case BZ_OUTBUFF_FULL:
            return -1;
        case BZ_CONFIG_ERROR:
            return -1;
        }
    } while(BZ_STREAM_END!=ret && outh.avail_in);

    return maxSize-outh.avail_in;
}
