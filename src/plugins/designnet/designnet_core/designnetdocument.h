#pragma once
#include "../../coreplugin/document/idocument.h"
#include "Utils/XML/xmldeserializer.h"


/**
 * @brief The DesignNetDocument class
 * 实现*.designnet文件的操作
 */
class DesignNetEditor;
class DesignNetDocument : public Core::IDocument
{
    Q_OBJECT

public:

    DesignNetDocument(DesignNetEditor *parent = 0);
    ~DesignNetDocument();

    QString defaultPath() const;
    QString suggestedFileName() const;
    QString suffixType() const;

    bool shouldAutoSave() const;

	bool isModified() const;
    bool isSaveAsAllowed() const;

    void rename(const QString &newName);
    void setModified(const bool &bModified = true);

    bool reload(QString *errorString, ReloadFlag flag, ChangeType type);
    bool save(QString *errorString, const QString &fileName = QString(), bool autoSave = false);
	virtual bool open(QString *errorString, const QString &fileName, const QString &realFileName);

signals:

	void deserialized(Utils::XmlDeserializer &);
	void serialized(Utils::XmlSerializer &);

public slots:

	void onModified();

private:

	bool				m_bOpening;
	bool				m_bModified;
	DesignNetEditor*	m_pEditor;
};
