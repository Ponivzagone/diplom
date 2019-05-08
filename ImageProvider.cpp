#include "ImageProvider.h"
#include <QDebug>
#include <QImage>
#include <poppler/qt5/poppler-qt5.h>

LiveImageProvider::LiveImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

    QString filename;
    filename = "/srv/dev/Diplom/source/programs/lilyPond.pdf";
    Poppler::Document* document = Poppler::Document::load(filename);
    if (!document || document->isLocked()) {
        // ... error message ....
        delete document;

    }

    // Paranoid safety check
    if (document == nullptr) {
      // ... error message ...

    }
    // Access page of the PDF file
    Poppler::Page* pdfPage = document->page(0);  // Document starts at page 0
    if (pdfPage == nullptr) {
      // ... error message ...

    }


    this->no_image = pdfPage->renderToImage();
    this->blockSignals(false);


    // Generate a QImage of the rendered page
    QImage image_ = pdfPage->renderToImage();
    if (image_.isNull()) {
      // ... error message ...

    }

    image_.save("/srv/dev/Diplom/source/programs/1.png");
    delete pdfPage;

    delete document;
}

QImage LiveImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage result = this->image;

    if(result.isNull()) {
        result = this->no_image;
    }

    if(size) {
        *size = result.size();
    }

    if(requestedSize.width() > 0 && requestedSize.height() > 0) {
        result = result.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);
    }

    return result;
}

void LiveImageProvider::updateImage(const QImage &image)
{
    if(this->image != image) {
        this->image = image;
        emit imageChanged();
    }
}
