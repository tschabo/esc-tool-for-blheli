#include "cesctoolgui.h"
#include "ui_cesctoolgui.h"

#include <QFileDialog>
#include "cprogrammer.h"
#include "cescconf.h"
#include "qextserialenumerator.h"

CEscToolGui::CEscToolGui(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CEscToolGui),
  c_pProgrammer(new CProgrammer(parent)),
  c_pConfig(new CEscConf(parent)),
  c_pConfModel(new CConfigModel(parent)),
  c_pEditorDelegate(new CEditorDelegate(parent))
{
  ui->setupUi(this);

  foreach(QextPortInfo port, QextSerialEnumerator::getPorts())
  {
#if defined(Q_OS_WIN)
    ui->c_pCbPort->insertItem(0, port.portName);
#endif
#if defined(Q_OS_UNIX)
    ui->c_pCbPort->insertItem(0, port.physName);
#endif
  }
  ui->c_pCbPort->setCurrentIndex(0);

  ui->c_pConfigTable->setModel(c_pConfModel);
  ui->c_pConfigTable->setItemDelegate(c_pEditorDelegate);

  connect(c_pProgrammer, SIGNAL(signal_dumpString(QString)), this, SLOT(slot_dumpString(QString)));
  connect(c_pConfig, SIGNAL(signal_dumpString(QString)), this, SLOT(slot_dumpString(QString)));
  connect(ui->c_pPbReadConfig, SIGNAL(clicked()), this, SLOT(slot_readConfig()));
  connect(ui->c_pPbWriteConfig, SIGNAL(clicked()), this, SLOT(slot_writeConfig()));
  connect(ui->c_pPbChooseFile, SIGNAL(clicked()), this, SLOT(slot_chooseHexFile()));
  connect(ui->c_pPbUpload, SIGNAL(clicked()), this, SLOT(slot_writeFlash()));
  connect(ui->c_pPbVerify, SIGNAL(clicked()), this, SLOT(slot_verifyFlash()));
}

CEscToolGui::~CEscToolGui()
{
  delete ui;
}

void CEscToolGui::slot_readConfig()
{
  if(!c_configList.isEmpty())
  {
    c_configList.clear();
    c_pConfModel->initSetData(&c_configList);
  }
  c_pConfig->setPortName(ui->c_pCbPort->currentText());
  c_pConfig->readConfig(&c_configList);
  c_pConfModel->initSetData(&c_configList);
  ui->c_pConfigTable->resizeColumnsToContents();
}

void CEscToolGui::slot_writeConfig()
{
  if(c_configList.isEmpty())
  {
    return;
  }
  c_pConfig->setPortName(ui->c_pCbPort->currentText());
  c_pConfig->writeConfig(&c_configList);
}

void CEscToolGui::slot_chooseHexFile()
{
  ui->c_pLeFile->setText(QFileDialog::getOpenFileName());
}

void CEscToolGui::slot_writeFlash()
{
  c_pProgrammer->setPortName(ui->c_pCbPort->currentText());
  c_pProgrammer->writeProgramToDevice(ui->c_pLeFile->text());
}

void CEscToolGui::slot_verifyFlash()
{
  c_pProgrammer->setPortName(ui->c_pCbPort->currentText());
  c_pProgrammer->verifyProgramOnDevice(ui->c_pLeFile->text());
}

void CEscToolGui::slot_dumpString(QString qsData)
{
  ui->textBrowser->append(qsData);
  QApplication::processEvents();
}
