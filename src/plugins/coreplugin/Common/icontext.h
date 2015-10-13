#pragma once
#include "../core_global.h"


namespace Core {

class CORE_EXPORT Context
{
public:

	Context() {}

	explicit Context(const char *c1) { add(c1); }
	Context(const char *c1, const char *c2) { add(c1); add(c2); }
	Context(const char *c1, const char *c2, const char *c3) { add(c1); add(c2); add(c3); }
	Context(const char *base, int offset);
	void add(const char *c);
	bool contains(const char *c) const;
	bool contains(int c) const { return d.contains(c); }
	int size() const { return d.size(); }
	bool isEmpty() const { return d.isEmpty(); }
	int at(int i) const { return d.at(i); }
	
	typedef QList<int>::const_iterator const_iterator;
	const_iterator begin() const { return d.begin(); }
	const_iterator end() const { return d.end(); }
	int indexOf(int c) const { return d.indexOf(c); }
	void removeAt(int i) { d.removeAt(i); }
	void prepend(int c) { d.prepend(c); }
	void add(const Context &c) { d += c.d; }
	void add(int c) { d.append(c); }
	bool operator==(const Context &c) const { return d == c.d; }

private:

    QList<int> d;
};

class CORE_EXPORT IContext : public QObject
{
    Q_OBJECT
public:
    IContext(QObject *parent = 0) : QObject(parent), m_widget(0) {}

    virtual Context context() const { return m_context; }
    virtual QWidget *widget() const { return m_widget; }
    virtual QString contextHelpId() const { return m_contextHelpId; }

    virtual void setContext(const Context &context) { m_context = context; }
    virtual void setWidget(QWidget *widget) { m_widget = widget; }
    virtual void setContextHelpId(const QString &id) { m_contextHelpId = id; }

protected:

    Context m_context;
    QPointer<QWidget> m_widget;
    QString m_contextHelpId;
};

}
