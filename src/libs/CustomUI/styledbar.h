#ifndef STYLEDBAR_H
#define STYLEDBAR_H

#include <QWidget>
#include "customui_global.h"
namespace CustomUI{
class CUSTOMUI_EXPORT StyledBar : public QWidget
{
	Q_OBJECT
public:
	StyledBar(QWidget *parent = 0);
	void setSingleRow(bool singleRow);
	bool isSingleRow() const;

	void setLightColored(bool lightColored);
	bool isLightColored() const;

protected:
	void paintEvent(QPaintEvent *event);
};

}

#endif // STYLEDBAR_H
