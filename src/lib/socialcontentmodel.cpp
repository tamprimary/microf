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

#include "socialcontentmodel.h"
#include "socialcontentmodel_p.h"
#include <QtCore/QDebug>
#include <QtCore/QVariantMap>
#include "socialnetwork_p.h"
#include "socialcontentmodelbuilder_p.h"

SocialContentModelPrivate::SocialContentModelPrivate(SocialContentModel *q)
    : AbstractSocialContentPrivate(q), m_hasNext(false), m_hasPrevious(false)
    , m_loadMode(SocialRequest::Load), m_socialNetwork(0), m_request(0), m_builder(0)
{
}

void SocialContentModelPrivate::setContentModelData(SocialContentModel &contentModel,
                                                    const QList<QVariantMap> &data,
                                                    bool hasNext, bool hasPrevious,
                                                    const QVariantMap &metadata)
{
    contentModel.d_func()->setContentModelData(data, hasNext, hasPrevious, metadata);
}

void SocialContentModelPrivate::setContentModelError(SocialContentModel &contentModel,
                                                     SocialNetworkError::type error,
                                                     const QString &errorMessage,
                                                     const QString &errorCode)
{
    contentModel.d_func()->setError(error, errorMessage, errorCode);
}

bool SocialContentModelPrivate::build(QNetworkReply::NetworkError error, const QString &errorMessage,
                                      const QByteArray &data)
{
    Q_Q(SocialContentModel);
    if (!m_builder) {
        return false;
    }
    SocialContentModelBuilderPrivate::build(*m_builder, *q, error, errorMessage, data, metadata());
    return true;
}

bool SocialContentModelPrivate::load(SocialRequest::Mode mode)
{
    if (status == SocialNetworkStatus::Busy) {
        qWarning() << "SocialContentModelPrivate::load() called while status is Busy";
        return false;
    }

    if (!m_socialNetwork) {
        qWarning() << "SocialContentModelPrivate::load() called without socialNetwork";
        return false;
    }

    if (!m_request) {
        qWarning() << "SocialContentModelPrivate::load() called without request";
        return false;
    }

    if (!m_builder) {
        qWarning() << "SocialContentModelPrivate::load() called without builder";
        return false;
    }

    bool ok = SocialNetworkPrivate::socialContentLoad(*m_socialNetwork, *this, *m_request, mode);
    if (ok) {
        m_loadMode = mode;
        setStatus(SocialNetworkStatus::Busy);
    }
    return ok;
}

void SocialContentModelPrivate::setContentModelData(const QList<QVariantMap> &data,
                                                    bool hasNext, bool hasPrevious,
                                                    const QVariantMap &metadata)
{
    Q_Q(SocialContentModel);
    switch (m_loadMode) {
    case SocialRequest::Load:
        setNewData(data);
        break;
    case SocialRequest::LoadNext:
        appendNewData(data);
        break;
    case SocialRequest::LoadPrevious:
        prependNewData(data);
        break;
    }

    if (m_hasNext != hasNext) {
        m_hasNext = hasNext;
        emit q->hasNextChanged();
    }

    if (m_hasPrevious != hasPrevious) {
        m_hasPrevious = hasPrevious;
        emit q->hasPreviousChanged();
    }

    setMetadata(metadata);
    setStatus(SocialNetworkStatus::Ready);
    emit q->finished(true);
}

void SocialContentModelPrivate::setNewData(const QList<QVariantMap> &data)
{
    Q_Q(SocialContentModel);
    if (data.isEmpty()) {
        return;
    }

    if (q->rowCount() > 0) {
        q->beginRemoveRows(QModelIndex(), 0, q->rowCount() - 1);
        m_data.clear();
        if (data.isEmpty()) {
            emit q->countChanged();
        }
        q->endRemoveRows();
    }

    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), 0, data.count() - 1);
        m_data = data;
        emit q->countChanged();
        q->endInsertRows();
    }
}

void SocialContentModelPrivate::appendNewData(const QList<QVariantMap> &data)
{
    Q_Q(SocialContentModel);
    if (data.isEmpty()) {
        return;
    }

    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), q->rowCount(), q->rowCount() + data.count() - 1);
        m_data.append(data);
        emit q->countChanged();
        q->endInsertRows();
    }
}

void SocialContentModelPrivate::prependNewData(const QList<QVariantMap> &data)
{
    Q_Q(SocialContentModel);
    if (data.isEmpty()) {
        return;
    }

    if (data.count() > 0) {
        q->beginInsertRows(QModelIndex(), 0, data.count() - 1);
        QList<QVariantMap> oldData = m_data;
        m_data = data;
        m_data.append(oldData);
        emit q->countChanged();
        q->endInsertRows();
    }
}

SocialContentModel::SocialContentModel(QObject *parent)
    : QAbstractListModel(parent), d_ptr(new SocialContentModelPrivate(this))
{
}

SocialContentModel::~SocialContentModel()
{
}

QHash<int, QByteArray> SocialContentModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(ObjectRole, "object");
    return roles;
}

int SocialContentModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const SocialContentModel);
    Q_UNUSED(parent);
    return d->m_data.count();
}

QVariant SocialContentModel::data(const QModelIndex &index, int role) const
{
    Q_D(const SocialContentModel);
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }

    switch (role) {
    case ObjectRole:
        return d->m_data.value(row);
        break;
    default:
        return QVariant();
        break;
    }
}

void SocialContentModel::classBegin()
{
}

void SocialContentModel::componentComplete()
{
}

int SocialContentModel::count() const
{
    Q_D(const SocialContentModel);
    return d->m_data.count();
}

bool SocialContentModel::hasNext() const
{
    Q_D(const SocialContentModel);
    return d->m_hasNext;
}

bool SocialContentModel::hasPrevious() const
{
    Q_D(const SocialContentModel);
    return d->m_hasPrevious;
}

SocialNetwork * SocialContentModel::socialNetwork() const
{
    Q_D(const SocialContentModel);
    return d->m_socialNetwork;
}

void SocialContentModel::setSocialNetwork(SocialNetwork *socialNetwork)
{
    Q_D(SocialContentModel);
    if (d->m_socialNetwork != socialNetwork) {
        d->m_socialNetwork = socialNetwork;
        emit socialNetworkChanged();
    }
}

SocialRequest * SocialContentModel::request() const
{
    Q_D(const SocialContentModel);
    return d->m_request;
}

void SocialContentModel::setRequest(SocialRequest *request)
{
    Q_D(SocialContentModel);
    if (d->m_request != request) {
        d->m_request = request;
        emit requestChanged();
    }
}

SocialContentModelBuilder * SocialContentModel::builder() const
{
    Q_D(const SocialContentModel);
    return d->m_builder;
}

void SocialContentModel::setBuilder(SocialContentModelBuilder *replyParser)
{
    Q_D(SocialContentModel);
    if (d->m_builder != replyParser) {
        d->m_builder = replyParser;
        emit builderChanged();
    }
}

SocialNetworkStatus::type SocialContentModel::status() const
{
    Q_D(const SocialContentModel);
    return d->status;
}

SocialNetworkError::type SocialContentModel::error() const
{
    Q_D(const SocialContentModel);
    return d->error;
}

QString SocialContentModel::errorMessage() const
{
    Q_D(const SocialContentModel);
    return d->errorMessage;
}

QString SocialContentModel::errorCode() const
{
    Q_D(const SocialContentModel);
    return d->errorCode;
}

bool SocialContentModel::load()
{
    Q_D(SocialContentModel);
    return d->load(SocialRequest::Load);
}

bool SocialContentModel::loadNext()
{
    Q_D(SocialContentModel);
    return d->load(SocialRequest::LoadNext);
}

bool SocialContentModel::loadPrevious()
{
    Q_D(SocialContentModel);
    return d->load(SocialRequest::LoadPrevious);
}

