#ifndef CONNECTION_H
#define CONNECTION_H

namespace DesignNet
{
class Processor;
class Link : public QObject
{
	Q_OBJECT

public:

	Processor* m_pProcessorFrom;
	Processor* m_pProcessorTo;
};

}


#endif
