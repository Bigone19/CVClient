#include "client.h"
#include "ui_client.h"
#include <QMessageBox>

#define __MUTI_FILE_TEST__ 1

CClient::CClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CClient)
{
	m_resRecevicer = CResTransRecevicer::GetInstance();

    ui->setupUi(this);

#if 0
    ui->labelImgDisplay->setPixmap(QPixmap("./results/LUD07456.jpg"));
#endif

    ui->btnTransfer->setEnabled(false);
    ui->btnCancel->setEnabled(false);
    m_fileSender = new CFileTransSender(parent);
	connect(m_fileSender, SIGNAL(progressValue(const QString&, int)), this, SLOT(on_progressValueChanged(const QString&, int)));
	connect(m_fileSender, SIGNAL(errorState(int, int)), this, SLOT(on_error_state(int, int)));
    connect(m_fileSender, &CFileTransSender::fileTransferFinish, this, [this]{
        //ui->barProgress->setValue(100);
        //QMessageBox::information(this, "传输完成 ", "传输完成 ");
		ui->barProgress->setValue(0);
     });
}

CClient::~CClient()
{
    m_fileList.clear();
    delete ui;
}

void CClient::on_error_state(int code, int subCode)
{
    qDebug() << __FUNCTION__ << " error: code=" << code << "\tsubCode=" << subCode;
}

void CClient::on_progressValueChanged(const QString& fileName, int progressVal)
{
	qDebug() << "current transfer fileName===>" << fileName << ",progressVal===>" << progressVal;
	ui->barProgress->setValue(progressVal);
	ui->btnCancel->setEnabled(progressVal);
}

// 连接服务器
void CClient::on_btnConnectServer_clicked()
{
    bool isConnect = m_fileSender->connectToServer(IP_ADDR, PORT_COMMAND);
    qDebug() << (isConnect ? "连接成功 " : "连接超时 ");
}

// 取消上传
void CClient::on_btnCancel_clicked()
{
	ui->barProgress->setValue(0);
    m_fileSender->cancelSendFile();
}

// 上传
void CClient::on_btnTransfer_clicked()
{
    ui->barProgress->setValue(0);

#if not __MUTI_FILE_TEST__
	QString filePath = ui->filePostion->text();
	if (!filePath.isEmpty())
	{
		m_fileSender->sendFile(filePath);
	}
#else
	if (!m_fileList.isEmpty())
	{
		m_fileSender->sendFiles(m_fileList);
	}
#endif
}

// 打开文件
void CClient::on_btnSelectFile_clicked()
{
#if not __MUTI_FILE_TEST__
	QString filePath = QFileDialog::getOpenFileName(this, "open", "../");
	ui->filePostion->setText(filePath);
	if (!filePath.isEmpty())
	{
		ui->btnTransfer->setEnabled(true);
	}
#else
    QString filters = "file(*.jpg *.png *.dng *.JPEG *.jpeg)";
    m_fileList = QFileDialog::getOpenFileNames(this, "选择文件", "", filters);
    QString fileDirPath = m_fileList.at(0).left(m_fileList.at(0).lastIndexOf("/") + 1);
	ui->filePostion->setText(fileDirPath);
	if (!fileDirPath.isEmpty())
	{
		ui->btnTransfer->setEnabled(true);
	}

#endif
}

