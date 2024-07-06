#include "components/datavisualizer.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    DataVisualizerWindow w;
    w.showMaximized();
    return a.exec();
}

/*
 *
*/
