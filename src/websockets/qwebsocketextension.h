/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWebSockets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWEBSOCKETEXTENSION_H
#define QWEBSOCKETEXTENSION_H

#include <QtCore/qglobal.h>
#include <QtCore/qlist.h>

#include "qwebsockets_global.h"

QT_BEGIN_NAMESPACE

class Q_WEBSOCKETS_EXPORT QWebSocketExtension
{
public:
    QWebSocketExtension();
    virtual ~QWebSocketExtension();

    virtual bool isEnabled();
    virtual void setEnabled(bool e);

    virtual void init() = 0;
    virtual void destroy() = 0;

    virtual void clientJob(const QByteArray &input, QByteArray &output) = 0;
    virtual void serverJob(const QByteArray &input, QByteArray &output) = 0;

    virtual QString name() const;

    virtual QString offer() const;
    virtual bool acceptable(bool rsv1, bool rsv2, bool rsv3) const;
    virtual bool canCompress() const;
    virtual void setOptions(const QString &input);
    virtual bool validate(const QString &input) const;
    virtual bool negotiate(const QString &input, QString &output) const;
    virtual QString response() const;

    virtual void virtual_hook(int id, void *data);
private:
    Q_DISABLE_COPY(QWebSocketExtension)
};

QT_END_NAMESPACE

#endif // QWEBSOCKETEXTENSION_H
