/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
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

#include "roommessageevent.h"

#include <QtCore/QJsonObject>
#include <QtCore/QMimeDatabase>
#include <QtCore/QDebug>

using namespace QMatrixClient;

class RoomMessageEvent::Private
{
    public:
        Private() : msgtype(MessageEventType::Unknown), content(nullptr) {}
        
        QString userId;
        MessageEventType msgtype;
        QString plainBody;
        MessageEventContent::Base* content;
};

RoomMessageEvent::RoomMessageEvent()
    : Event(EventType::RoomMessage)
    , d(new Private)
{ }

RoomMessageEvent::~RoomMessageEvent()
{
    delete d;
}

QString RoomMessageEvent::userId() const
{
    return d->userId;
}

MessageEventType RoomMessageEvent::msgtype() const
{
    return d->msgtype;
}

QString RoomMessageEvent::plainBody() const
{
    return d->plainBody;
}

QString RoomMessageEvent::body() const
{
    return plainBody();
}

MessageEventContent::Base* RoomMessageEvent::content() const
{
    return d->content;
}

RoomMessageEvent* RoomMessageEvent::fromJson(const QJsonObject& obj)
{
    RoomMessageEvent* e = new RoomMessageEvent();
    e->parseJson(obj);
    if( obj.contains("sender") )
    {
        e->d->userId = obj.value("sender").toString();
    } else {
        qDebug() << "RoomMessageEvent: user_id not found";
    }
    if( obj.contains("content") )
    {
        using namespace MessageEventContent;

        QJsonObject content = obj.value("content").toObject();
        QString msgtype = content.value("msgtype").toString();

        if( msgtype == "m.text" )
        {
            e->d->msgtype = MessageEventType::Text;
            e->d->content = new TextContent(obj);
        }
        else if( msgtype == "m.emote" )
        {
            e->d->msgtype = MessageEventType::Emote;
            e->d->content = new TextContent(obj);
        }
        else if( msgtype == "m.notice" )
        {
            e->d->msgtype = MessageEventType::Notice;
            e->d->content = new TextContent(obj);
        }
        else if( msgtype == "m.image" )
        {
            e->d->msgtype = MessageEventType::Image;
            ImageContent* c = new ImageContent;
            c->url = QUrl(content.value("url").toString());
            QJsonObject info = content.value("info").toObject();
            c->height = info.value("h").toInt();
            c->width = info.value("w").toInt();
            c->size = info.value("size").toInt();
            c->mimetype = info.value("mimetype").toString();
            e->d->content = c;
        }
        else if( msgtype == "m.file" )
        {
            e->d->msgtype = MessageEventType::File;
            FileContent* c = new FileContent;
            c->filename = content.value("filename").toString();
            c->url = QUrl(content.value("url").toString());
            QJsonObject info = content.value("info").toObject();
            c->size = info.value("size").toInt();
            c->mimetype = info.value("mimetype").toString();
            e->d->content = c;
        }
        else if( msgtype == "m.location" )
        {
            e->d->msgtype = MessageEventType::Location;
            LocationContent* c = new LocationContent;
            c->geoUri = content.value("geo_uri").toString();
            c->thumbnailUrl = QUrl(content.value("thumbnail_url").toString());
            QJsonObject info = content.value("thumbnail_info").toObject();
            c->thumbnailHeight = info.value("h").toInt();
            c->thumbnailWidth = info.value("w").toInt();
            c->thumbnailSize = info.value("size").toInt();
            c->thumbnailMimetype = info.value("mimetype").toString();
            e->d->content = c;
        }
        else if( msgtype == "m.video" )
        {
            e->d->msgtype = MessageEventType::Video;
            VideoContent* c = new VideoContent;
            c->url = QUrl(content.value("url").toString());
            QJsonObject info = content.value("info").toObject();
            c->height = info.value("h").toInt();
            c->width = info.value("w").toInt();
            c->duration = info.value("duration").toInt();
            c->size = info.value("size").toInt();
            c->thumbnailUrl = QUrl(info.value("thumnail_url").toString());
            QJsonObject thumbnailInfo = content.value("thumbnail_info").toObject();
            c->thumbnailHeight = thumbnailInfo.value("h").toInt();
            c->thumbnailWidth = thumbnailInfo.value("w").toInt();
            c->thumbnailSize = thumbnailInfo.value("size").toInt();
            c->thumbnailMimetype = thumbnailInfo.value("mimetype").toString();
            e->d->content = c;
        }
        else if( msgtype == "m.audio" )
        {
            e->d->msgtype = MessageEventType::Audio;
            AudioContent* c = new AudioContent;
            c->url = QUrl(content.value("url").toString());
            QJsonObject info = content.value("info").toObject();
            c->duration = info.value("duration").toInt();
            c->mimetype = info.value("mimetype").toString();
            c->size = info.value("size").toInt();
            e->d->content = c;
        }
        else
        {
            qDebug() << "RoomMessageEvent: unknown msgtype: " << msgtype;
            qDebug() << obj;
            e->d->msgtype = MessageEventType::Unknown;
            e->d->content = new Base;
        }

        if( content.contains("body") )
        {
            e->d->plainBody = content.value("body").toString();
        } else {
            qDebug() << "RoomMessageEvent: body not found";
        }
    }
    return e;
}

using namespace MessageEventContent;

TextContent::TextContent(const QJsonObject& json)
{
    QMimeDatabase db;

    // Special-casing the custom matrix.org's (actually, Vector's) way
    // of sending HTML messages.
    if (json["format"].toString() == "org.matrix.custom.html")
    {
        mimeType = db.mimeTypeForName("text/html");
        body = json["formatted_body"].toString();
    } else {
        // Best-guessing from the content
        body = json["body"].toString();
        mimeType = db.mimeTypeForData(body.toUtf8());
    }
}
