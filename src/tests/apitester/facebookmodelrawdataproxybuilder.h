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

#ifndef FACEBOOKMODELRAWDATAPROXYBUILDER_H
#define FACEBOOKMODELRAWDATAPROXYBUILDER_H

#include "socialcontentmodelbuilder.h"

class AbstractFacebookModelBuilder;
class FacebookModelRawDataProxyBuilder : public SocialContentModelBuilder
{
    Q_OBJECT
    Q_PROPERTY(AbstractFacebookModelBuilder * builder READ builder WRITE setBuilder NOTIFY builderChanged)
    Q_PROPERTY(QString rawData READ rawData NOTIFY rawDataChanged)
public:
    explicit FacebookModelRawDataProxyBuilder(QObject *parent = 0);
    AbstractFacebookModelBuilder * builder() const;
    void setBuilder(AbstractFacebookModelBuilder * builder);
    QString rawData() const;
signals:
    void builderChanged();
    void rawDataChanged();
protected:
    void build(SocialContentModel &contentModel, QNetworkReply::NetworkError error,
               const QString &errorMessage, const QByteArray &data,
               const QVariantMap &metadata) Q_DECL_OVERRIDE;
private:
    AbstractFacebookModelBuilder *m_builder;
    QString m_rawData;
};

#endif // FACEBOOKMODELRAWDATAPROXYBUILDER_H
