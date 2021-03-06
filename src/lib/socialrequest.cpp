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

#include "socialrequest.h"
#include "socialrequest_p.h"
#ifdef MICROF_DEBUG
#include <QtCore/QDebug>
#endif

SocialRequestPrivate::SocialRequestPrivate(SocialRequest *q)
    : q_ptr(q)
{
}

QNetworkRequest SocialRequestPrivate::createRequest(const SocialRequest &request,
                                                    const SocialNetwork &socialNetwork,
                                                    const QByteArray &postData,
                                                    SocialRequest::Mode mode,
                                                    const QVariantMap &metadata)
{
#ifdef MICROF_DEBUG
    const QNetworkRequest &result = request.createRequest(socialNetwork, postData, mode, metadata);
    qDebug() << "SocialRequestPrivate::createRequest";
    qDebug() << "Mode: " << mode;
    qDebug() << "Metadata: " << metadata;
    qDebug() << "Request url:" << result.url();
    qDebug() << "Header:";
    for (const QByteArray &header : result.rawHeaderList()) {
        qDebug() << header << "=" << result.rawHeader(header);
    }
    return result;
#else
    return request.createRequest(socialNetwork, postData, mode, metadata);
#endif
}

QByteArray SocialRequestPrivate::createPostData(const SocialRequest &request,
                                                const SocialNetwork &socialNetwork,
                                                SocialRequest::Mode mode,
                                                const QVariantMap &metadata)
{
#ifdef MICROF_DEBUG
    const QByteArray &result = request.createPostData(socialNetwork, mode, metadata);
    qDebug() << "SocialRequestPrivate::createPostData";
    qDebug() << "Post data:" << result;
    return result;
#else
    return request.createPostData(socialNetwork, mode, metadata);
#endif
}

QVariantMap SocialRequestPrivate::createMetadata(const SocialRequest &request,
                                                 const SocialNetwork &socialNetwork,
                                                 SocialRequest::Mode mode,
                                                 const QVariantMap &metadata)
{
#ifdef MICROF_DEBUG
    const QVariantMap &result = request.createMetadata(socialNetwork, mode, metadata);
    qDebug() << "SocialRequestPrivate::createMetadata";
    qDebug() << "Metadata:" << result;
    return result;
#else
    return request.createMetadata(socialNetwork, mode, metadata);
#endif
}

SocialRequest::SocialRequest(QObject *parent)
    : QObject(parent), d_ptr(new SocialRequestPrivate(this))
{
}

SocialRequest::SocialRequest(SocialRequestPrivate &dd, QObject *parent)
    : QObject(parent), d_ptr(&dd)
{
}

QVariantMap SocialRequest::createMetadata(const SocialNetwork &socialNetwork, Mode mode,
                                          const QVariantMap &metadata) const
{
    Q_UNUSED(socialNetwork);
    Q_UNUSED(mode);
    Q_UNUSED(metadata);
    return QVariantMap();
}

SocialRequest::~SocialRequest()
{
}

void SocialRequest::classBegin()
{
}

void SocialRequest::componentComplete()
{
}
