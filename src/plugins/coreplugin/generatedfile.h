#ifndef GENERATEDFILE_H
#define GENERATEDFILE_H

#include "core_global.h"
#include "id.h"
#include <QSharedDataPointer>

namespace Core{
class GeneratedFilePrivate;
class CORE_EXPORT GeneratedFile
{
public:
	enum Attribute { // Open this file in editor
		OpenEditorAttribute			= 0x01,
		// Open project
		OpenProjectAttribute		= 0x02,
		CustomGeneratorAttribute	= 0x04,
		KeepExistingFileAttribute	= 0x08
	};
	Q_DECLARE_FLAGS(Attributes, Attribute)

		GeneratedFile();
	explicit GeneratedFile(const QString &path);
	GeneratedFile(const GeneratedFile &);
	GeneratedFile &operator=(const GeneratedFile &);
	~GeneratedFile();

	// Full path of the file should be created, or the suggested file name
	QString path() const;
	void setPath(const QString &p);

	// Contents of the file (UTF8)
	QString contents() const;
	void setContents(const QString &c);

	QByteArray binaryContents() const;
	void setBinaryContents(const QByteArray &c);

	// Defaults to false (Text file).
	bool isBinary() const;
	void setBinary(bool b);

	// Id of editor to open the file with
	Id editorId() const;
	void setEditorId(const Id &id);

	bool write(QString *errorMessage) const;

	Attributes attributes() const;
	void setAttributes(Attributes a);

private:
	QSharedDataPointer<GeneratedFilePrivate> m_d;
};

typedef QList<GeneratedFile> GeneratedFiles;
}
#endif // GENERATEDFILE_H
