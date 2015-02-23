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

#include "qwebsocketpermessagedeflateextension.h"

#include <QtCore/qendian.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

// Default, minimum, maximum value for max_window_bits as defined
// by draft-ietf-hybi-permessage-compression-19
enum class max_window_bits : quint8 {
    default_s2c = 15, min_s2c = 8, max_s2c = 15,
    default_c2s = 15, min_c2s = 8, max_c2s = 15
};

/*!
    \internal
 */
QWebSocketPerMessageDeflateExtension::QWebSocketPerMessageDeflateExtension() :
    m_s2c_max_window_bits(quint8(max_window_bits::default_s2c)),
    m_c2s_max_window_bits(quint8(max_window_bits::default_c2s)),
    m_compress_buffer_size(0x4000) // 16K
{
    m_dstate.zalloc = Z_NULL;
    m_dstate.zfree = Z_NULL;
    m_dstate.opaque = Z_NULL;

    m_istate.zalloc = Z_NULL;
    m_istate.zfree = Z_NULL;
    m_istate.opaque = Z_NULL;
    m_istate.avail_in = 0;
    m_istate.next_in = Z_NULL;
}

QWebSocketPerMessageDeflateExtension::~QWebSocketPerMessageDeflateExtension()
{
    if (!m_initialized)
        return;

    if (deflateEnd(&m_dstate) != Z_OK)
        qWarning("Error cleaning up zlib compression state");
    if (inflateEnd(&m_istate) != Z_OK)
        qWarning("Error cleaning up zlib decompression state");
}

bool QWebSocketPerMessageDeflateExtension::serverJob(const QByteArray &input, QByteArray &output)
{
    return deflate(input, output);
}

bool QWebSocketPerMessageDeflateExtension::clientJob(const QByteArray &input, QByteArray &output)
{
    return inflate(input, output);
}


void QWebSocketPerMessageDeflateExtension::init()
{
    uint8_t deflate_bits;
    uint8_t inflate_bits;

    deflate_bits = m_c2s_max_window_bits;
    inflate_bits = m_s2c_max_window_bits;

    const int memoryLevel = 8;
    if (Z_OK != deflateInit2(&m_dstate, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -deflate_bits,
                             memoryLevel, Z_FIXED)) {
        qCritical("QWebSocketPerMessageDeflateExt::destroy: zlib error");
        return;
    }

    if (Z_OK != inflateInit2(&m_istate, -inflate_bits)) {
        qCritical("QWebSocketPerMessageDeflateExt::destroy: zlib error");
        return;
    }

    m_compress_buffer.resize(m_compress_buffer_size);
    m_initialized = true;
}

QString QWebSocketPerMessageDeflateExtension::name() const
{
    return QLatin1Literal("permessage-deflate");
}

QString QWebSocketPerMessageDeflateExtension::offer() const
{
    return QLatin1Literal("permessage-deflate; client_no_context_takeover; client_max_window_bits");
}

bool QWebSocketPerMessageDeflateExtension::acceptable(bool rsv1, bool rsv2, bool rsv3) const
{
    return rsv1 && !rsv2 && !rsv3;
}

QWebSocketExtension::Capabilities QWebSocketPerMessageDeflateExtension::capabilities() const
{
    return Compress | Decompress;
}

void QWebSocketPerMessageDeflateExtension::setOptions(const QString &in)
{
    if (in.isEmpty())
        return;

    static const QString clientMaxWindowBitsString = QStringLiteral("client_max_window_bits=");
    const int index = in.indexOf(clientMaxWindowBitsString) + clientMaxWindowBitsString.size();
    QString subString = in.mid(index);
    QTextStream stream(&subString);
    int value;
    stream >> value;
    if (value >= int(max_window_bits::min_c2s) && value <= int(max_window_bits::max_c2s))
        m_c2s_max_window_bits = value;
    else
        qWarning("QWebSocketPerMessageDeflateExtension::setOptions: Invalid client_max_window_bits "
                 "value");

    m_c2s_no_context_takeover = in.indexOf(QLatin1Literal("client_no_context_takeover")) != -1;
}

QString QWebSocketPerMessageDeflateExtension::response() const
{
    return QLatin1Literal("permessage-deflate");
}

bool QWebSocketPerMessageDeflateExtension::deflate(const QByteArray &in, QByteArray &out)
{
    if (!m_initialized)
        init();

    out.clear();
    m_dstate.avail_in = in.size();
    m_dstate.next_in = (unsigned char *)(in.constData());

    do {
        // Output to local buffer
        m_dstate.avail_out = m_compress_buffer_size;
        m_dstate.next_out = (unsigned char *)(m_compress_buffer.data());

        int ret = ::deflate(&m_dstate, Z_SYNC_FLUSH);
        if (ret == Z_STREAM_ERROR) {
            qWarning("QWebSocketPerMessageDeflateExtension::inflate: zlib error: Z_NEED_ERROR");
            return false;
        }

        size_t output = m_compress_buffer_size - m_dstate.avail_out;

        out.append(m_compress_buffer.data(), output);
    } while (m_dstate.avail_out == 0);
    return true;
}

bool QWebSocketPerMessageDeflateExtension::inflate(const QByteArray &in, QByteArray &out)
{
    if (!m_initialized)
        init();

    out.clear();
    m_istate.avail_in = in.length();
    m_istate.next_in = (unsigned char *)(in.constData());

    do {
        m_istate.avail_out = m_compress_buffer_size;
        m_istate.next_out = (unsigned char *)(m_compress_buffer.data());

        switch (::inflate(&m_istate, Z_SYNC_FLUSH)) {
        case Z_NEED_DICT:
            qWarning("QWebSocketPerMessageDeflateExtension::inflate: zlib error: Z_NEED_DICT");
            return false;
        case Z_DATA_ERROR:
            qWarning("QWebSocketPerMessageDeflateExtension::inflate: zlib error: Z_DATA_ERROR");
            return false;
        case Z_MEM_ERROR:
            qWarning("QWebSocketPerMessageDeflateExtension::inflate: zlib error: Z_MEM_ERROR");
            return false;
        }

        out.append(
            m_compress_buffer.data(),
            m_compress_buffer_size - m_istate.avail_out
        );
    } while (m_istate.avail_out == 0);
    return true;
}

QT_END_NAMESPACE
