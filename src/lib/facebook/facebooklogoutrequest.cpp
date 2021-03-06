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

#include "facebooklogoutrequest.h"
#include "facebook.h"
#include <QtCore/QUrlQuery>

FacebookLogoutRequest::FacebookLogoutRequest(QObject *parent)
    : SocialRequest(parent)
{
}

SocialRequest::Type FacebookLogoutRequest::type() const
{
    return SocialRequest::Post;
}

QNetworkRequest FacebookLogoutRequest::createRequest(const SocialNetwork &socialNetwork,
                                                     const QByteArray &postData, Mode mode,
                                                     const QVariantMap &metadata) const
{
    Q_UNUSED(mode);
    Q_UNUSED(metadata);
    const Facebook *facebook = qobject_cast<const Facebook *>(&socialNetwork);
    if (!facebook) {
        return QNetworkRequest();
    }

    QByteArray accessToken = facebook->accessToken().toLocal8Bit();
    if (accessToken.isEmpty()) {
        return QNetworkRequest();
    }

    accessToken.prepend("OAuth ");
    QNetworkRequest request (QUrl("https://b-api.facebook.com/method/auth.expireSession"));
    request.setRawHeader("Authorization", accessToken);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("Host", "b-api.facebook.com");
    request.setRawHeader("Connection", "Keep-Alive");
    request.setRawHeader("User-Agent", facebook->userAgent());
    request.setRawHeader("Content-Length", QByteArray::number(postData.size()));
    request.setRawHeader("X-FB-HTTP-Engine", "Apache");
    return request;
}

QByteArray FacebookLogoutRequest::createPostData(const SocialNetwork &socialNetwork, Mode mode,
                                                 const QVariantMap &metadata) const
{
    Q_UNUSED(mode);
    Q_UNUSED(metadata);
    const Facebook *facebook = qobject_cast<const Facebook *>(&socialNetwork);
    if (!facebook) {
        return QByteArray();
    }

    QUrlQuery query;
    query.addQueryItem("reason", "USER_INITIATED");
    query.addQueryItem("locale", facebook->locale());
    query.addQueryItem("client_country_code", facebook->countryCode());
    query.addQueryItem("method", "auth.expireSession");
    query.addQueryItem("fb_api_req_friendly_name", "logout");
    query.addQueryItem("fb_api_caller_class", "com.facebook.katana.server.handler.Fb4aAuthHandler");
    return query.toString(QUrl::FullyEncoded).toLocal8Bit();
}


