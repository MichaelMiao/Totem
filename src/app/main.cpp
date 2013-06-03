#include "totem_gui_pch.h"
#include "mainapp.h"
#include <QTranslator>
int main(int argc, char **argv)
{
    MainApp app(QLatin1String("Totem"), argc, argv);
    app.init();
    return app.exec();
}
