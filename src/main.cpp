#include "tvsegwindow.h"
#include <QApplication>

#include "tvseg/settings/backend.h"
#include "tvseg/settings/serializerqt.h"
#include "tvseg/util/logging.h"


INITIALIZE_EASYLOGGINGPP


int main(int argc, char *argv[])
{

    // Launch application
    QApplication a(argc, argv);
    tvseg_ui::TVSegWindow w(NULL, "tvseg.ini");
    w.show();
//    w.initFromSettings();


    int result = a.exec();

    return result;
}
