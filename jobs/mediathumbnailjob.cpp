/******************************************************************************
 * Copyright (C) 2016 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "mediathumbnailjob.h"

#include <QtNetwork/QNetworkReply>
#include <QtCore/QDebug>

using namespace QMatrixClient;

MediaThumbnailJob::MediaThumbnailJob(QUrl url, QSize requestedSize,
                                     ThumbnailType thumbnailType)
    : BaseJob(HttpVerb::Get, "MediaThumbnailJob",
              QStringLiteral("/_matrix/media/v1/thumbnail/%1%2")
                  .arg(url.host(), url.path()),
              Query(
                { { "width", QString::number(requestedSize.width()) }
                , { "height", QString::number(requestedSize.height()) }
                , { "method",
                    thumbnailType == ThumbnailType::Scale ? "scale" : "crop" }
                }))
{ }

QImage MediaThumbnailJob::thumbnail() const
{
    return _thumbnail;
}

QImage MediaThumbnailJob::scaledThumbnail(QSize toSize) const
{
    return _thumbnail.scaled(toSize,
                             Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

BaseJob::Status MediaThumbnailJob::parseReply(QNetworkReply* reply)
{
    if( !_thumbnail.loadFromData(reply->readAll()) )
    {
        qCDebug(JOBS) << "MediaThumbnailJob: could not read image data";
    }
    return Success;
}
