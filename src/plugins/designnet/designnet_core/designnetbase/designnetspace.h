#ifndef DESIGNNETSPACE_H
#define DESIGNNETSPACE_H
#include "Processor.h"
#include "Utils/XML/xmldeserializer.h"


namespace DesignNet
{
class DesignNetSpace : public Processor
{
	Q_OBJECT

public:

	void addProcessor(Processor* p);
	void removeProcessor(Processor* processor, bool bNotifyModify = false); //!< ÒÆ³ý´¦ÀíÆ÷,²¢delete


	void serialize(Utils::XmlSerializer& s) const override;
	void deserialize(Utils::XmlDeserializer& s) override;

private:


	QList<Processor*> m_processors;
};
}



#endif