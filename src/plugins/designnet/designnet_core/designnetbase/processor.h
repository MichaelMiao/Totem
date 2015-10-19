#pragma once
#include "property/propertyowner.h"
#include "coreplugin/Common/id.h"


namespace DesignNet
{
class Property;
class Port;

class Processor : public QObject
{
	Q_OBJECT

public:

	Processor() : m_pParent(0) {}
    ~Processor() {}

	void setParent(Processor* proc) { m_pParent = proc; }

    bool connectTo(Processor* child);
    bool isConnectTo(Processor* pChild);
    void detach(Processor* pChild);
    void detach();

    void setCategory(const Core::Id& idCategory) { m_idCategory = idCategory; }
	Core::Id category() const { return m_idCategory; }

    virtual bool beforeProcess() = 0;		//!< 处理之前的准备,这里会确保数据已经准备好了
    virtual bool process() = 0;				//!< 正式处理
    virtual void afterProcess() = 0;		//!< 完成处理
	virtual bool connectionTest(Port* pOutput, Port* pInput);
	virtual bool connectionTest(Processor* father);

signals:

    void connected(Processor* father, Processor* pChild);
	void onDisconnected(Processor* father, Processor* pChild);

private:

    QList<Port*>	m_outputPort;			//!< 所有的输出端口
    QList<Port*>	m_inputPort;			//!< 输入端口
    QIcon			m_icon;					//!< 图标
    QString			m_name;					//!< 处理器的名字
    Core::Id		m_idCategory;			//!< 分类

	Processor*		m_pParent;
};
}
