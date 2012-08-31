#include <QtGui/QApplication>
#include "cesctoolgui.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  CEscToolGui w;
  w.show();
  
  return a.exec();
}
