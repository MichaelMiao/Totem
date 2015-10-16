#ifndef IPROPERWIDGET_H
#define IPROPERWIDGET_H


class QLabel;
class QHBoxLayout;
namespace DesignNet
{
class Property;
class DESIGNNET_CORE_EXPORT IPropertyWidget : public QWidget
{
	Q_OBJECT

public:

	explicit IPropertyWidget(Property* prop, QWidget* parent = 0);

	Property* getProperty() const { return m_property; }
	void setLabelText(const QString& text) const;

protected:
	
	/*!
	 * \brief addWidget 在布局中添加\e widget ，不要修改布局，如果添加窗口，使用
	 * 该函数。该函数会在标签后面添加。
	 * \param[in] widget 要添加的widget
	 */
	
	void addWidget(QWidget* widget);
	
	/*!
	 * \brief addLayout 在布局中添加\e layout ，不要修改布局，如果添加子布局，使用
	 * 该函数。该函数会在标签后面添加。
	 * \param[in] widget 要添加的widget
	 */
	void addLayout(QLayout* layout);

	Property*   m_property; //!< 属性
	QLabel*     m_label;    //!< 标签
	QHBoxLayout* m_layout;  //!< 横向布局
};
}

#endif // IPROPERWIDGET_H
