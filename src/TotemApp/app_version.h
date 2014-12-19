#ifndef APP_VERSION_H
#define APP_VERSION_H

namespace Core {
namespace Constants {

//../share/totem 存放公共配置文件
#define SHARE_PATH "/../share"

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define TOTEM_VERSION 0.0.1
#define TOTEM_VERSION_STR STRINGIFY(TOTEM_VERSION)

#define TOTEM_VERSION_MAJOR 0
#define TOTEM_VERSION_MINOR 0
#define TOTEM_VERSION_RELEASE 1

const char * const TOTEM_VERSION_LONG      = TOTEM_VERSION_STR;
const char * const TOTEM_AUTHOR            = "Michael_BJFU";
const char * const TOTEM_YEAR              = "2012";


//settings保存路径
const char * const TOTEM_SETTINGSVARIANT_STR = "Michael";
} // Constants
} // Core

#endif //APP_VERSION_H