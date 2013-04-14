#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include "utils_global.h"

#include <QStyle>
#include <QColor>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QPalette;
class QPainter;
class QRect;
QT_END_NAMESPACE

namespace Utils{

class TOTEM_UTILS_EXPORT StyleHelper
{
public:
    static const unsigned int DEFAULT_BASE_COLOR = 0xd6d6d6;

    static qreal sidebarFontSize();
    static void drawCornerImage(const QImage &img, QPainter *painter, QRect rect,
                                      int left, int top, int right, int bottom);
    static QColor requestedBaseColor() { return m_requestedBaseColor; }
    static QColor baseColor(bool lightColored = false);
    static QColor panelTextColor(bool lightColored = false);
    static QColor highlightColor(bool lightColored = false);
    static QColor shadowColor(bool lightColored = false);
    static QColor borderColor(bool lightColored = false);
    static void drawIconWithShadow(const QIcon &icon, const QRect &rect, QPainter *p, QIcon::Mode iconMode,
                                   int radius = 3, const QColor &color = QColor(0, 0, 0, 130),
                                   const QPoint &offset = QPoint(1, -2));

    static void setBaseColor(const QColor &newcolor);
    
	static QColor sidebarHighlight() { return QColor(255, 255, 255, 40); }
    static QColor sidebarShadow() { return QColor(0, 0, 0, 40); }
	
    static bool usePixmapCache() { return true; }

    //gradient
    static void horizontalGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect, bool lightColored = false);
    static void verticalGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect, bool lightColored = false);
    static void menuGradient(QPainter *painter, const QRect &spanRect, const QRect &clipRect);
    static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50);


private:
    static QColor m_baseColor;
    static QColor m_requestedBaseColor;
};
}

#endif // STYLEHELPER_H