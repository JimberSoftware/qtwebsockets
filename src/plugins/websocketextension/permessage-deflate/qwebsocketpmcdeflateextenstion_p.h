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

#ifndef QWEBSOCKETPERMESSAGE_DEFLATE_EXT_P_H
#define QWEBSOCKETPERMESSAGE_DEFLATE_EXT_P_H

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <limits.h>

#include "qwebsockets_global.h"
#include "qwebsocketprotocol.h"
#include "qwebsocketextension.h"

#include <zlib.h>

QT_BEGIN_NAMESPACE

class QWebSocketPerMessageDeflateExt : public QWebSocketExtension
{
public:
    QWebSocketPerMessageDeflateExt();
    virtual ~QWebSocketPerMessageDeflateExt();

    bool isEnabled() override;
    void setEnabled(bool e) override;

    void init() override;
    void destroy() override;

    void decompress(const QByteArray &input, QByteArray &output) override;
    void compress(const QByteArray &input, QByteArray &output) override;

    QString name() const override;

    QString offer() const override;
    bool acceptable(bool rsv1, bool rsv2, bool rsv3) const override;
    bool canCompress() const override;
    void setOptions(const QString &input) override;
    QString response() const override;

private:
    bool m_enabled;

    bool m_s2c_no_context_takeover;
    bool m_c2s_no_context_takeover;
    quint8 m_s2c_max_window_bits;
    quint8 m_c2s_max_window_bits;

    bool m_initialized;
    size_t m_compress_buffer_size;
    QByteArray m_compress_buffer;
    z_stream m_dstate;
    z_stream m_istate;
};

QT_END_NAMESPACE

#endif // QWEBSOCKETPERMESSAGE_DEFLATE_EXT_P_H
