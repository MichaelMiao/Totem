#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <QObject>


namespace DesignNet
{
class PropertyManagerPrivate;
class IPropertyWidget;
class Property;
class PropertyManager : public QObject
{
	Q_OBJECT

public:

	explicit PropertyManager(QObject* parent = 0);
	~PropertyManager();
	static PropertyManager* instance();
	IPropertyWidget* createWidget(Property* property, QWidget* parent);

public slots:

	void objectAdded(QObject* obj);

protected:

	static PropertyManager* m_instance;
	PropertyManagerPrivate* d;
};
}
#endif // PROPERTYMANAGER_H
