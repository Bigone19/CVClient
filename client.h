#ifndef CCLIENT_H
#define CCLIENT_H

#include <QMainWindow>
#include <QFileDialog>
#include "fileTransferSender.h"
#include "resTransferRecevicer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CClient; }
QT_END_NAMESPACE

class CClient : public QMainWindow
{
    Q_OBJECT

public:
    CClient(QWidget *parent = nullptr);
    ~CClient();

private slots:
    void on_error_state(int, int);
    void on_progressValueChanged(const QString&, int);

    void on_btnConnectServer_clicked();

    void on_btnSelectFile_clicked();

    void on_btnTransfer_clicked();

    void on_btnCancel_clicked();

private:
    Ui::CClient *ui;
    CFileTransSender* m_fileSender;
    CResTransRecevicer* m_resRecevicer;

    QStringList m_fileList;
};
#endif // CCLIENT_H
