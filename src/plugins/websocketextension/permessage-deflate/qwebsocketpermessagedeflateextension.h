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
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
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
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWEBSOCKETPERMESSAGEDEFLATEEXTENSION_H
#define QWEBSOCKETPERMESSAGEDEFLATEEXTENSION_H

#include <QtCore/qbytearray.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qstring.h>

#include "qwebsockets_global.h"
#include "qwebsocketprotocol.h"
#include "qwebsocketextension.h"

#include <zlib.h>

QT_BEGIN_NAMESPACE

class QWebSocketPerMessageDeflateExtension : public QWebSocketExtension
{
public:
    QWebSocketPerMessageDeflateExtension();
    virtual ~QWebSocketPerMessageDeflateExtension();

    bool serverJob(const QByteArray &input, QByteArray &output) override;
    bool clientJob(const QByteArray &input, QByteArray &output) override;

    QString name() const override;

    QString offer() const override;
    bool acceptable(bool rsv1, bool rsv2, bool rsv3) const override;
    Capabilities capabilities() const override;
    void setOptions(const QString &input) override;
    QString response() const override;

private:
    void init();
    bool deflate(const QByteArray &input, QByteArray &output);
    bool inflate(const QByteArray &input, QByteArray &output);

    bool m_s2c_no_context_takeover = false;
    bool m_c2s_no_context_takeover = false;
    quint8 m_s2c_max_window_bits;
    quint8 m_c2s_max_window_bits;

    bool m_initialized;
    size_t m_compress_buffer_size;
    QByteArray m_compress_buffer;
    z_stream m_dstate;
    z_stream m_istate;
};

QT_END_NAMESPACE

#endif // QWEBSOCKETPERMESSAGEDEFLATEEXTENSION_H
