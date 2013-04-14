#include "totem_gui_pch.h"
#include "mainapp.h"
#include "extensionsystem/pluginmanager.h"
#include "extensionsystem/pluginmanagerprivate.h"
#include "extensionsystem/pluginspec.h"
#include "extensionsystem/progressmanagerprivate.h"
#include "pluginloaderview.h"

#include <app_version.h>

#include <QMessageBox>
#include <QThreadPool>
#include <QLibraryInfo>
#include <QSettings>
#include <QNetworkProxyFactory>
#include <QDir>
#include <QDesktopServices>
#include <QTranslator>
#include <QFile>
#include <QtConcurrentRun>
#include <QMetaObject>

const char appNameC[] = "Totem"; //!< 应用程序名称
static const char corePluginNameC[] = "Core"; //!< 核心插件的名称
static const char pluginLoaderNameC[] = "PluginLoader";
static const char defaultQssFileC[] = "skins/totem.qss"; //!< 默认皮肤文件

typedef QList<ExtensionSystem::PluginSpec *> PluginSpecSet;

///
/// 插件描述文件的后缀名，默认为.pluginspec
/// \note 若需要更换后缀名不仅需要修改这里，还要修改.pro文件中的配置
static const char PLUGINSPEC_FILE_EXTENTION[] = "pluginspec";
static inline int askMsgSendFailed()
{
    return QMessageBox::question(0, QApplication::translate("Application", "Could not send message"),
                                 QCoreApplication::translate("Application", "Unable to send command line arguments to the already running instance."
                                                             "It appears to be not responding."),
                                 QMessageBox::Yes | QMessageBox::No | QMessageBox::Retry,
                                 QMessageBox::Retry);
}
/*!
 * 显示错误信息，该应用程序需要有窗口才可以。
 * \param[in] t 要显示的错误信息
 */
static void displayError(const QString &t)
{
    QMessageBox::critical(0, QLatin1String(appNameC), t);
}
/*!
 * 载入插件失败
 * \param[in] pluginName 插件名称
 * \param[in] why   载入插件失败原因
 * \return 返回翻译后的结果
 */
static inline QString msgLoadFailure(const QString &pluginName, const QString &why)
{
    return QCoreApplication::translate("Application", "Failed to load plugin%1: %2").arg(pluginName).arg(why);
}
/*!
 * \brief 获取插件路径
 *
 * 插件路径包括：
 * 1、应用程序所在文件夹的上一级目录的lib下的totem/plugins
 * 2、系统用户数据文件夹（\em QDesktopServices::DataLocation \em）所在路径下的Michael/totem/plugins
 * \return 路径列表
 */
static inline QStringList getPluginPaths()
{
    QStringList rc;
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    //用于快捷方式问题
    const QString rootDirPath = rootDir.canonicalPath();
    QString pluginPath = rootDirPath;
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("plugins");
    rc.push_back(pluginPath);
    pluginPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    pluginPath += QLatin1Char('/')
            + QLatin1String(Core::Constants::TOTEM_SETTINGSVARIANT_STR)
            + QLatin1Char('/');

    pluginPath += QLatin1String("totem");
    pluginPath += QLatin1String("/plugins/");
    pluginPath += QLatin1String(Core::Constants::TOTEM_VERSION_LONG);
    rc.push_back(pluginPath);
    return rc;
}
/*!
 * 根据插件状态输出错误信息（如果没有错误返回0，否则返回1）
 * \param[in] plugin
 * \param[in] pluginName
 * \return 如果没有错误返回0，否则返回1
 */
int checkPlugin(ExtensionSystem::PluginSpec *plugin, const QString &pluginName)
{
    if(!plugin)
    {
        QStringList pluginPaths = ExtensionSystem::PluginManager::instance()->pluginPaths();
        QString nativePaths = QDir::toNativeSeparators(pluginPaths.join(QLatin1String(",")));
        const QString reason = QCoreApplication::translate("Application", "Could not find '%1.pluginspec' in %2").arg(pluginName).arg(nativePaths);
        displayError(msgLoadFailure(pluginName, reason));
        return 1;
    }
    if (plugin->hasError())
    {
        displayError(msgLoadFailure(pluginName, plugin->errorString()));
        return 1;
    }
    return 0;//0表示成功
}
/*!
 * \brief 该函数最初是在子线程中完成的，但是由于在子线程中完成创建对象的任务后，还需要将该对象moveToThread到
 *  主线，给很多操作带来不小的麻烦，所以这里还是在主线程中完成的。
 * \param[] receiver 用来传输数据
 * \return 返回0表示成功
 */
int loadPluginThread(MainApp *receiver)
{
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope,
                       QCoreApplication::applicationDirPath() + QLatin1String(SHARE_PATH));
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                        QLatin1String(Core::Constants::TOTEM_SETTINGSVARIANT_STR),
                                        QLatin1String("Totem"));
    QSettings *globalSettings = new QSettings(QSettings::IniFormat, QSettings::SystemScope,
                                        QLatin1String(Core::Constants::TOTEM_SETTINGSVARIANT_STR),
                                        QLatin1String("Totem"));

    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    pm->setFileExtension(QLatin1String(PLUGINSPEC_FILE_EXTENTION));
    pm->setGlobalSettings(globalSettings);
    pm->setSettings(settings);

    QTranslator translator;
    QTranslator qtTranslator;
    QStringList uiLanguages;

#if (QT_VERSION >= 0x040801) || (QT_VERSION >= 0x040800 && !defined(Q_OS_WIN))
    uiLanguages = QLocale::system().uiLanguages();
#else
    uiLanguages << QLocale::system().name();
#endif
//    QString overrideLanguage = settings->value("General/OverrideLanguage").toString();
//    if (!overrideLanguage.isEmpty())
//        uiLanguages.prepend(overrideLanguage);
    const QString &totemTrPath = QCoreApplication::applicationDirPath()
            + QLatin1String(SHARE_PATH "/translations");
    foreach (const QString &locale, uiLanguages)
    {
        if (translator.load(QLatin1String("totem_") + locale, totemTrPath))
        {
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;

            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, totemTrPath))
            {
                receiver->app.installTranslator(&translator);
                receiver->app.installTranslator(&qtTranslator);
                receiver->app.setProperty("qtc_locale", locale);
                break;
            }
            translator.load(QString()); // unload()
        }
        else if (locale == QLatin1String("C") /* overrideLanguage == "English" */)
        {
            break;
        }
        else if (locale.startsWith(QLatin1String("en")) /* "English" is built-in */)
        {
            break;
        }
    }
    //这里没有考虑MacOS,Unix
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    //设置插件搜索路径
    const QStringList pluginPaths = getPluginPaths();
    pm->setPluginPaths(pluginPaths);

    //检查核心插件
    const PluginSpecSet plugins = pm->plugins();
    ExtensionSystem::PluginSpec *coreplugin = 0;
    foreach(ExtensionSystem::PluginSpec *spec, plugins)
    {
        if(spec->name() == QLatin1String(corePluginNameC))
        {
            coreplugin = spec;
            break;
        }
    }
    int status;
    if( status = checkPlugin(coreplugin, QLatin1String(corePluginNameC)))//如果核心插件检查有问题
    {
        return status;
    }

    //载入皮肤
    receiver->loadSkin(defaultQssFileC);
    QList<ExtensionSystem::PluginSpec *> queue = pm->loadQueue();
    receiver->m_futureInterface->setProgressRange(0, queue.size());
    QMetaObject::invokeMethod(receiver, "onStart", Qt::QueuedConnection);
    receiver->m_futureInterface->reportStarted();
    foreach(ExtensionSystem::PluginSpec *spec, queue)
    {
        pm->loadPlugin(spec,ExtensionSystem::PluginSpec::Loaded);
        receiver->m_futureInterface->setProgressValue(
                    receiver->m_futureInterface->progressValue() + 1);
    }
    receiver->m_futureInterface->reportFinished();
    pm->loadPluginsAuto();

    //初始化应用程序实例
    receiver->app.initialize();
    QObject::connect(&(receiver->app), SIGNAL(messageReceived(QString)),
                     pm, SLOT(remoteArguments(QString)));
    QObject::connect(&(receiver->app), SIGNAL(aboutToQuit()), pm, SLOT(shutdown()));

    QMetaObject::invokeMethod(receiver, "onFinish", Qt::QueuedConnection);


    return 0;
}
MainApp::MainApp(const QString &id, int &argc, char **argv) :
    app(id, argc, argv)
{

}

MainApp::~MainApp()
{
    if(m_pLoader)
    {
        delete m_pLoader;
        m_pLoader = 0;
    }
}

int MainApp::init()
{
    QTranslator ts;
    ts.load("translations/Totem_ZN");
    app.installTranslator(&ts);
    //------------------------------------------
    qint64 pid = -1;
    if (app.isRunning())
    {
        if (app.sendMessage(QLatin1String("ack"), 5000, pid))
            return 0;
        if (app.isRunning(pid))
        {
            int button = askMsgSendFailed();
            //如果无法一直是“重试”
            while(button == QMessageBox::Retry)
            {
                if (app.sendMessage("pm->serializedArguments()", 5000 /*timeout*/, pid))
                    return 0;
                if (!app.isRunning(pid)) //说明应用程序退出了，可以启动新的实例了。
                    button = QMessageBox::Yes;
                else
                    button = askMsgSendFailed();
            }
            if (button == QMessageBox::No)//说明用户放弃操作，选了“否”
                return -1;
        }
    }


    m_pLoader = new PluginLoaderView();
    m_futureInterface = new QFutureInterface<void>();
    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(qMax(4, 2 * threadCount));


    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    connect(pm->d, SIGNAL(showMessage(QString)), this->m_pLoader, SLOT(setProgressText(QString)));

    ExtensionSystem::ProgressManagerPrivate *progressMgr = ExtensionSystem::ProgressManagerPrivate::instance();
    progressMgr->addTask(m_futureInterface->future(), QLatin1String("LoadPlugin"), tr(""), m_pLoader);
    loadPluginThread(this);
//    QtConcurrent::run(loadPluginThread, this);
    return 0;
}
/**
 * 直接调用app.exec()
 */
int MainApp::exec()
{
    return app.exec();
}

bool MainApp::loadSkin(const QString &strFileName)
{
    QFile file(strFileName);
    if(file.open(QIODevice::ReadOnly))
    {
        app.setStyleSheet(file.readAll());
        file.close();
        return true;
    }
    return false;
}
/*!
 * 使Loader显示出来
 */
void MainApp::onStart()
{
    m_pLoader->show();
}

void MainApp::onFinish()
{
    m_pLoader->hide();

}
