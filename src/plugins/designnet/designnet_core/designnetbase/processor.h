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

    virtual bool beforeProcess() = 0;		//!< ����֮ǰ��׼��,�����ȷ�������Ѿ�׼������
    virtual bool process() = 0;				//!< ��ʽ����
    virtual void afterProcess() = 0;		//!< ��ɴ���
	virtual bool connectionTest(Port* pOutput, Port* pInput);
	virtual bool connectionTest(Processor* father);

signals:

    void connected(Processor* father, Processor* pChild);
	void onDisconnected(Processor* father, Processor* pChild);

private:

    QList<Port*>	m_outputPort;			//!< ���е�����˿�
    QList<Port*>	m_inputPort;			//!< ����˿�
    QIcon			m_icon;					//!< ͼ��
    QString			m_name;					//!< ������������
    Core::Id		m_idCategory;			//!< ����

	Processor*		m_pParent;
};
}
