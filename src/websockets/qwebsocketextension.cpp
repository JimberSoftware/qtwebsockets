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

#include "qwebsocketextension.h"

QT_BEGIN_NAMESPACE

QWebSocketExtension::QWebSocketExtension()
{
}

QWebSocketExtension::~QWebSocketExtension()
{
}

bool QWebSocketExtension::isEnabled()
{
    return false;
}

void QWebSocketExtension::setEnabled(bool /*e*/)
{
}

QString QWebSocketExtension::name() const
{
    return QString();
}

QString QWebSocketExtension::offer() const
{
    return QString();
}

bool QWebSocketExtension::acceptable(bool /*rsv1*/, bool /*rsv2*/, bool /*rsv3*/) const
{
    return false;
}

bool QWebSocketExtension::canCompress() const
{
    return false;
}

void QWebSocketExtension::setOptions(const QString &/*input*/)
{
    return;
}

bool QWebSocketExtension::validate(const QString &/*input*/) const
{
    return false;
}

bool QWebSocketExtension::negotiate(const QString &/*input*/, QString &/*output*/) const
{
    return false;
}

QString QWebSocketExtension::response() const
{
    return QString();
}

void QWebSocketExtension::virtual_hook(int /*id*/, void */*data*/)
{
}

QT_END_NAMESPACE
