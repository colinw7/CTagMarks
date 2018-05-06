#include <QApplication>
#include <QMainWindow>
#include <CQTagMarks.h>

#ifdef CQ_APP_H
#include <CQApp.h>
#else
#include <QApplication>
#endif

int
main(int argc, char **argv)
{
#ifdef CQ_APP_H
  CQApp app(argc, argv);
#else
  QApplication app(argc, argv);
#endif

  QMainWindow *mw = new QMainWindow;

  CQTagMarks *tag_marks = new CQTagMarks;

  mw->setCentralWidget(tag_marks);

  mw->show();

  app.exec();

  return 0;
}
