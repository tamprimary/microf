/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "facebookitembuilder.h"
#include "socialcontentitembuilder_p.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include "facebook_p.h"

class FacebookItemBuilderPrivate: public SocialContentItemBuilderPrivate
{
public:
    explicit FacebookItemBuilderPrivate(FacebookItemBuilder *q);
    static void properties_append(QQmlListProperty<FacebookProperty> *list, FacebookProperty *property);
    static int properties_count(QQmlListProperty<FacebookProperty> *list);
    static FacebookProperty * properties_at(QQmlListProperty<FacebookProperty> *list, int index);
    static void properties_clear(QQmlListProperty<FacebookProperty> *list);
private:
    void clear();
    QList<FacebookProperty *> m_properties;
    Q_DECLARE_PUBLIC(FacebookItemBuilder)
};

FacebookItemBuilderPrivate::FacebookItemBuilderPrivate(FacebookItemBuilder *q)
    : SocialContentItemBuilderPrivate(q)
{
}

void FacebookItemBuilderPrivate::properties_append(QQmlListProperty<FacebookProperty> *list, FacebookProperty *property)
{
    FacebookItemBuilder *builder = qobject_cast<FacebookItemBuilder *>(list->object);
    if (!builder || !property) {
        return;
    }
    builder->d_func()->m_properties.append(property);
}

int FacebookItemBuilderPrivate::properties_count(QQmlListProperty<FacebookProperty> *list)
{
    FacebookItemBuilder *builder = qobject_cast<FacebookItemBuilder *>(list->object);
    if (!builder) {
        return 0;
    }
    return builder->d_func()->m_properties.count();
}

FacebookProperty * FacebookItemBuilderPrivate::properties_at(QQmlListProperty<FacebookProperty> *list, int index)
{
    FacebookItemBuilder *builder = qobject_cast<FacebookItemBuilder *>(list->object);
    if (!builder) {
        return nullptr;
    }

    const QList<FacebookProperty *> &properties = builder->d_func()->m_properties;
    if (index < 0 || index >= properties.count()) {
        return nullptr;
    }

    return properties.at(index);
}

void FacebookItemBuilderPrivate::properties_clear(QQmlListProperty<FacebookProperty> *list)
{
    FacebookItemBuilder *builder = qobject_cast<FacebookItemBuilder *>(list->object);
    if (!builder) {
        return;
    }

    builder->d_func()->clear();
}

void FacebookItemBuilderPrivate::clear()
{
    qDeleteAll(m_properties);
    m_properties.clear();
}

FacebookItemBuilder::FacebookItemBuilder(QObject *parent)
    : SocialContentItemBuilder(*(new FacebookItemBuilderPrivate(this)), parent)
{
}

FacebookItemBuilder::~FacebookItemBuilder()
{
    Q_D(FacebookItemBuilder);
    d->clear();
}

void FacebookItemBuilder::build(SocialContentItem &contentItem,
                                 QNetworkReply::NetworkError error, const QString &errorMessage,
                                 const QByteArray &data, const QVariantMap &metadata)
{
    Q_D(FacebookItemBuilder);
    SocialNetworkError::type outError = SocialNetworkError::None;
    QString outErrorMessage;
    QString outErrorCode;

    QJsonObject root = FacebookPrivate::prebuild(error, errorMessage, data, metadata,
                                                 outError, outErrorMessage, outErrorCode);
    if (outError != SocialNetworkError::None) {
        setError(contentItem, outError, outErrorMessage, outErrorCode);
        return;
    }

    root.remove("__type__");
    setObject(contentItem, FacebookPrivate::buildProperties(root, d->m_properties));
}

QQmlListProperty<FacebookProperty> FacebookItemBuilder::properties()
{
    return QQmlListProperty<FacebookProperty>(this, 0,
                                              &FacebookItemBuilderPrivate::properties_append,
                                              &FacebookItemBuilderPrivate::properties_count,
                                              &FacebookItemBuilderPrivate::properties_at,
                                              &FacebookItemBuilderPrivate::properties_clear);
}
