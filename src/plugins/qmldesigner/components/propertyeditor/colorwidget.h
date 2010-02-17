/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QWeakPointer>
#include <QtGui/QWidget>
#include <QLabel>
#include <QToolButton>
#include <QMouseEvent>
#include <modelnode.h>
#include <qml.h>
#include <propertyeditorvalue.h>

QT_BEGIN_NAMESPACE
class QtColorButton;
class QToolButton;
QT_END_NAMESPACE

namespace QmlDesigner {

class ColorButton : public QToolButton {

Q_OBJECT

Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)

public:

    ColorButton(QWidget *parent = 0) : QToolButton (parent), m_colorString("#ffffff")
    {
    }

    void setColor(const QString &colorStr)
    {
        if (m_colorString == colorStr)
            return;

        m_colorString = colorStr;
        update();
        emit colorChanged();
    }

    QString color() const
    {
        return m_colorString;
    }

signals:
    void colorChanged();

protected:
    void paintEvent(QPaintEvent *event);
private:
    QString m_colorString;
};

class ColorBox : public QWidget
{
Q_OBJECT

Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)
Q_PROPERTY(qreal hue READ hue WRITE setHue NOTIFY hueChanged)
Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)

public:

ColorBox(QWidget *parent = 0) : QWidget(parent), m_colorString("#ffffff"), m_hue(0), m_lastHue(0)
{
    setFixedWidth(130);
    setFixedHeight(130);
}

void setHue(qreal newHue)
{
    if (m_hue == newHue)
        return;

    m_hue = newHue;
    update();
    emit hueChanged();
    emit colorChanged();
}

qreal hue() const
{
    return m_hue;
}

void setColor(const QString &colorStr)
{
    if (m_colorString == colorStr)
        return;

    qreal oldsaturation = saturation();
    qreal oldvalue = value();
    m_colorString = colorStr;
    update();
    qreal newHue = QColor(m_colorString).hsvHueF();
    if (newHue >= 0)
        setHue(newHue);
    if (oldsaturation != saturation()) emit saturationChanged();
    if (oldvalue != value()) emit valueChanged();
    emit colorChanged();
}

QString color() const
{
    return m_colorString;
}


qreal saturation() const
{
  return QColor(m_colorString).hsvSaturationF();
}

void setSaturation(qreal newsaturation)
{
    QColor color(m_colorString);
    if (color.hsvSaturationF()==newsaturation) return;
    color.setHsvF(color.hsvHueF(),newsaturation,color.valueF());
    m_colorString=color.name();
    update();
    emit saturationChanged();
    emit colorChanged();
}

qreal value() const
{
  return QColor(m_colorString).valueF();
}

void setValue(qreal newvalue)
{
    QColor color(m_colorString);
    if (color.valueF()==newvalue) return;
    color.setHsvF(color.hsvHueF(),color.hsvSaturationF(),newvalue);
    m_colorString=color.name();
    update();
    emit valueChanged();
    emit colorChanged();
}

signals:
    void colorChanged();
    void hueChanged();
    void saturationChanged();
    void valueChanged();

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *e)
    {
        // The current cell marker is set to the cell the mouse is pressed in
        QPoint pos = e->pos();
        m_mousePressed = true;
        setCurrent(pos.x() - 5, pos.y() - 5);
    }

    void mouseReleaseEvent(QMouseEvent * /* event */)
    {
        m_mousePressed = false;
    }

void mouseMoveEvent(QMouseEvent *e)
{
    if (!m_mousePressed)
        return;
    QPoint pos = e->pos();
    setCurrent(pos.x() - 5, pos.y() - 5);
}

void setCurrent(int x, int y);


private:
    QString m_colorString;
    bool m_mousePressed;
    qreal m_hue;
    qreal m_lastHue;
    QPixmap m_cache;
};

class HueControl : public QWidget
{
Q_OBJECT

Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)
Q_PROPERTY(qreal hue READ hue WRITE setHue NOTIFY hueChanged)

public:

HueControl(QWidget *parent = 0) : QWidget(parent), m_colorString("#ffffff"), m_mousePressed(false), m_hue(0)
{
    setFixedWidth(40);
    setFixedHeight(130);
}

void setHue(qreal newHue)
{
    if (m_hue == newHue)
        return;

    m_hue = newHue;
    QColor color(m_colorString);
    color.setHsvF(newHue, color.hsvSaturationF(), color.valueF());
    m_colorString = color.name();
    update();
    emit hueChanged();
}

qreal hue() const
{
    return m_hue;
}

void setColor(const QString &colorStr)
{
    if (m_colorString == colorStr)
        return;

    m_colorString = colorStr;
    m_hue = QColor(m_colorString).hsvHueF();
    update();
    emit colorChanged();
}

QString color() const
{
    return m_colorString;
}

signals:
    void colorChanged();
    void hueChanged();

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *e)
    {
        // The current cell marker is set to the cell the mouse is pressed in
        QPoint pos = e->pos();
        m_mousePressed = true;
        setCurrent(pos.y() - 5);
    }

    void mouseReleaseEvent(QMouseEvent * /* event */)
    {
        m_mousePressed = false;
    }

void mouseMoveEvent(QMouseEvent *e)
{
    if (!m_mousePressed)
        return;
    QPoint pos = e->pos();
    setCurrent(pos.y() - 5);
}

void setCurrent(int y);


private:
    QString m_colorString;
    bool m_mousePressed;
    qreal m_hue;
    QPixmap m_cache;
};


} //QmlDesigner

QT_BEGIN_NAMESPACE
QML_DECLARE_TYPE(QmlDesigner::ColorButton);
QML_DECLARE_TYPE(QmlDesigner::HueControl);
QML_DECLARE_TYPE(QmlDesigner::ColorBox);
QT_END_NAMESPACE

#endif //COLORWIDGET_H
