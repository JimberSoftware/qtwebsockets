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

#include "qwebsocketpmcdeflateextenstion_p.h"

#include <QtCore/QtEndian>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

/// Default value for s2c_max_window_bits as defined by draft-ietf-hybi-permessage-compression-19
static uint8_t const default_s2c_max_window_bits = 15;
/// Minimum value for s2c_max_window_bits as defined by draft-ietf-hybi-permessage-compression-19
static uint8_t const min_s2c_max_window_bits = 8;
/// Maximum value for s2c_max_window_bits as defined by draft-ietf-hybi-permessage-compression-19
static uint8_t const max_s2c_max_window_bits = 15;

/// Default value for c2s_max_window_bits as defined by draft-ietf-hybi-permessage-compression-19
static uint8_t const default_c2s_max_window_bits = 15;
/// Minimum value for c2s_max_window_bits as defined by draft-ietf-hybi-permessage-compression-19
static uint8_t const min_c2s_max_window_bits = 8;
/// Maximum value for c2s_max_window_bits as defined by draft-ietf-hybi-permessage-compression-19
static uint8_t const max_c2s_max_window_bits = 15;

/*!
    \internal
 */
QWebSocketPerMessageDeflateExt::QWebSocketPerMessageDeflateExt() :
    m_enabled(false),
    m_s2c_no_context_takeover(false),
    m_c2s_no_context_takeover(false),
    m_s2c_max_window_bits(15),
    m_c2s_max_window_bits(15),
    m_initialized(false),
    m_compress_buffer_size(16384)
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

QWebSocketPerMessageDeflateExt::~QWebSocketPerMessageDeflateExt()
{
    destroy();
}

void QWebSocketPerMessageDeflateExt::destroy()
{
    if (!m_initialized) {
        return;
    }

    int ret = deflateEnd(&m_dstate);

    if (ret != Z_OK) {
        //std::cout << "error cleaning up zlib compression state"
        //          << std::endl;
    }

    ret = inflateEnd(&m_istate);

    if (ret != Z_OK) {
        //std::cout << "error cleaning up zlib decompression state"
        //          << std::endl;
    }
}

void QWebSocketPerMessageDeflateExt::init()
{
    uint8_t deflate_bits;
    uint8_t inflate_bits;

//    if (true /*is_server*/) {
//        deflate_bits = m_s2c_max_window_bits;
//        inflate_bits = m_c2s_max_window_bits;
//    } else {
        deflate_bits = m_c2s_max_window_bits;
        inflate_bits = m_s2c_max_window_bits;
//    }

    int ret = deflateInit2(
        &m_dstate,
        Z_DEFAULT_COMPRESSION,
        Z_DEFLATED,
        -1*deflate_bits,
        8, // memory level 1-9
        /*Z_DEFAULT_STRATEGY*/Z_FIXED
    );

    if (ret != Z_OK) {
        return;// make_error_code(error::zlib_error);
    }

    ret = inflateInit2(
        &m_istate,
        -1*inflate_bits
    );

    if (ret != Z_OK) {
        return;// make_error_code(error::zlib_error);
    }

    m_compress_buffer.resize(m_compress_buffer_size);
    m_initialized = true;
    return;// lib::error_code();
}

bool QWebSocketPerMessageDeflateExt::isEnabled()
{
    return m_enabled;
}

void QWebSocketPerMessageDeflateExt::setEnabled(bool e)
{
    if (m_enabled != e)
        m_enabled = e;
}

QString QWebSocketPerMessageDeflateExt::name() const
{
    return QLatin1Literal("permessage-deflate");
}

QString QWebSocketPerMessageDeflateExt::offer() const
{
    return QLatin1Literal("permessage-deflate; client_no_context_takeover; client_max_window_bits");
}

bool QWebSocketPerMessageDeflateExt::acceptable(bool rsv1, bool rsv2, bool rsv3) const
{
    if (rsv1 && !rsv2 && !rsv3)
        return true;

    return false;
}

bool QWebSocketPerMessageDeflateExt::canCompress() const
{
    return true;
}

void QWebSocketPerMessageDeflateExt::setOptions(const QString &in)
{
    if (in.indexOf(QLatin1Literal("client_max_window_bits=8")) != -1) {
        m_c2s_max_window_bits = 8;
    }
    if (in.indexOf(QLatin1Literal("client_max_window_bits=15")) != -1) {
        m_c2s_max_window_bits = 15;
    }
    if (in.indexOf(QLatin1Literal("client_no_context_takeover")) != -1) {
        m_c2s_no_context_takeover = true;
    } else {
        m_c2s_no_context_takeover = false;
    }
}

QString QWebSocketPerMessageDeflateExt::response() const
{
    return QLatin1Literal("permessage-deflate");
}

bool QWebSocketPerMessageDeflateExt::validate(const QString &/*in*/) const
{
    return true;
}

bool QWebSocketPerMessageDeflateExt::negotiate(const QString &in, QString &out) const
{
    Q_UNUSED(in)
    Q_UNUSED(out)
    return false;
}

void QWebSocketPerMessageDeflateExt::serverJob(const QByteArray &in, QByteArray &out) //compress
{
    if (!m_initialized)
        init();

    size_t output;
    int ret;

    out.clear();

    m_dstate.avail_out = m_compress_buffer_size;
    m_dstate.avail_in = in.size();
    m_dstate.next_in = (unsigned char *)(in.constData());

    do {
        // Output to local buffer
        m_dstate.avail_out = m_compress_buffer_size;
        m_dstate.next_out = (unsigned char *)(m_compress_buffer.data());

        ret = deflate(&m_dstate, Z_SYNC_FLUSH);
        Q_ASSERT(ret != Z_STREAM_ERROR);

        output = m_compress_buffer_size - m_dstate.avail_out;

        out.append(m_compress_buffer.data(), output);
    } while (m_dstate.avail_out == 0);
}

void QWebSocketPerMessageDeflateExt::clientJob(const QByteArray &in, QByteArray &out) //decompress
{
    if (!m_initialized)
        init();

    int ret;

    m_istate.avail_in = in.length();
    m_istate.next_in = (unsigned char *)(in.constData());;

    do {
        m_istate.avail_out = m_compress_buffer_size;
        m_istate.next_out = (unsigned char *)(m_compress_buffer.data());

        ret = inflate(&m_istate, Z_SYNC_FLUSH);

        if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            qDebug() << "zlib error"
                     << ", ret == Z_NEED_DICT:" << (ret == Z_NEED_DICT)
                     << ", ret == Z_DATA_ERROR:" << (ret == Z_DATA_ERROR)
                     << ", ret == Z_MEM_ERROR:" << (ret == Z_MEM_ERROR);
            //return out;//make_error_code(error::zlib_error);
        }

        out.append(
            m_compress_buffer.data(),
            m_compress_buffer_size - m_istate.avail_out
        );
    } while (m_istate.avail_out == 0);
}

QT_END_NAMESPACE
