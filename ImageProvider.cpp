#include "ImageProvider.h"
#include <QDebug>


LiveImageProvider::LiveImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

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

void LiveImageProvider::updateImage(const QImage & image)
{

        this->image = image;
        emit imageChanged();

}
