#include "wxreaddwgthumbnail.h"
#include <QFile>
#include <QImage>

namespace WxReadDwgThumbnail {

#pragma pack(2)
typedef struct tagBITMAPFILEHEADER {
    quint16    bfType;//must be 0x4D42.
    quint32   bfSize;//the size of the whole bitmap file.
    quint16    bfReserved1;
    quint16    bfReserved2;
    quint32   bfOffBits;//the sum bits of BITMAPFILEHEADER,BITMAPINFOHEADER and RGBQUAD;the index byte of the image data.
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pop()

QImage ReadThumbnail(const QString strDWGFileName)
{
    int test = sizeof(tagBITMAPFILEHEADER);
    QFile file(strDWGFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return QImage();
    }

    int posSentinel = 0;
    int typePreview = 0;
    int posBMP = 0;
    short lenBMP = 0;
    short biBitCount = 0;
    QByteArray bmpInfo;
    BITMAPFILEHEADER biH; //BMP文件头，DWG文件中不包含位图文件头，要自行加上去

    file.seek(13);
    QByteArray posSentinelData = file.read(4);
    posSentinel = *((int *)posSentinelData.data());
    file.seek(posSentinel + 30);
    QByteArray typePreviewData = file.read(1);
    typePreview = *(typePreviewData.data());
    if (typePreview == 1)
    {
        file.close();
        return QImage();
    }
    else if (typePreview == 2 || typePreview == 3)
    {
        file.seek(posSentinel + 31);
        QByteArray posBMPData = file.read(4);
        posBMP = *((int *)posBMPData.data());
        file.seek(posSentinel + 35);
        QByteArray lenBmpData = file.read(2);
        lenBMP = *((short *)lenBmpData.data());
        file.seek(posBMP + 14);
        QByteArray biBitCountData = file.read(2);
        biBitCount = *((short *)biBitCountData.data());
        file.seek(posBMP);
        bmpInfo = file.read(lenBMP);
        file.close();

        biH.bfType = 19778; //建立位图文件头
        if (biBitCount < 9)
        {
            biH.bfSize = 54 + 4 * (int)(pow(2, biBitCount)) + lenBMP;
        }
        else
        {
            biH.bfSize = 54 + lenBMP;
        }
        biH.bfReserved1 = 0; //保留字节
        biH.bfReserved2 = 0; //保留字节
        biH.bfOffBits = 14 + 40 + 1024; //图像数据偏移

        QByteArray imgData;
        imgData.append((char *)&biH, sizeof(BITMAPFILEHEADER));
        imgData.append(bmpInfo, lenBMP);

        QImage image = QImage::fromData(imgData);
        return image;
    }

    return QImage();
}

}
