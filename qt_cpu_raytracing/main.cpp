#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QtDebug>

#include <omp.h>
#include <QElapsedTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Setup Dark GUI
    a.setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    QColor darkColor = QColor(77,77,77);
    QColor disabledColor = QColor(127,127,127);
    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(70,70,70));
    darkPalette.setColor(QPalette::AlternateBase, darkColor);
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(150, 150, 150));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);
    a.setPalette(darkPalette);

    MainWindow w;
    w.show();

//    QElapsedTimer timer;
//    timer.start();
//    qDebug() << "start: ";
//    constexpr int N = 50000;
//    int aa[N] = {0};
////    int bb[N] = {0};

//    int i,j;
//    long sum = 0;

//    #pragma omp parallel num_threads(4)
//    qDebug() << "hello";

//    #pragma omp parallel for
//    for (i=0; i<N; i++) {
//        aa[i] = 1;
//        for (j=0; j<N; j++) {
//            sum += aa[j];
//        }
//    }

//    qDebug() << "sum: " << sum;
//    qDebug() << "fin: " << timer.elapsed();

    return a.exec();
}
