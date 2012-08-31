#ifndef CESCTOOLGUI_H
#define CESCTOOLGUI_H

#include <QWidget>
#include <QList>
#include "ccsvparser.h"
#include "cconfigmodel.h"
#include "ceditordelegate.h"

class CProgrammer;
class CEscConf;

namespace Ui {
  class CEscToolGui;
}

class CEscToolGui : public QWidget
{
  Q_OBJECT
  
public:
  explicit CEscToolGui(QWidget *parent = 0);
  ~CEscToolGui();
  
private:
  Ui::CEscToolGui *ui;

  CProgrammer * c_pProgrammer;
  CEscConf * c_pConfig;

  QList<sEEpromData*> c_configList;

  CConfigModel * c_pConfModel;
  CEditorDelegate * c_pEditorDelegate;

private slots:
  // Configuration
  void slot_readConfig();
  void slot_writeConfig();

  // Programmer
  void slot_chooseHexFile();
  void slot_writeFlash();
  void slot_verifyFlash();

  // common
  void slot_dumpString(QString qsData);

};

#endif // CESCTOOLGUI_H
