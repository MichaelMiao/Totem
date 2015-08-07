#ifndef MAINAPP_H
#define MAINAPP_H
#include <QObject>
#include "qtsingleapplication/qtsingleapplication.h"
#include <QApplication>
#include <QString>
#include <QFutureInterface>

/*!
 * \brief 主APP类，完成插件的加载以及保证只有一个该类型的应用程序
 *
 *
 */
class MainApp : public QApplication
{
    Q_OBJECT
public:
    explicit MainApp(const QString &id, int &argc, char **argv);
    ~MainApp();
    int init();//!< 初始化，加载插件
//    int exec();//!< 执行app.exe

//   SharedTools::QtSingleApplication app;
    QFutureInterface<void> *m_futureInterface;//!< 进度监控

    ///
    /// 加载皮肤
    bool loadSkin(const QString &strFileName);
	
public slots:
    void onStart(); //!< 开始加载插件
    void onFinish();//!< 完成所有插件的加载


private:

};

#endif // MAINAPP_H
