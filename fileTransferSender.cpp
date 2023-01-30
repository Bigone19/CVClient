#include "fileTransferSender.h"

#include <QSignalSpy>
#include <QtConcurrent/QtConcurrent>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

CFileTransSender::CFileTransSender(QObject* parent /*= nullptr*/)
	: QObject(parent)
	, m_isCancel(false)
	, m_isRunning(false)
	, m_nextBlockSize(0)
	, m_recvDataSize(0)
	, m_timeoutMsec(3000)
	, m_fileSize(0)
	, m_sendSize(0)
{
	m_commSocket = new QTcpSocket(this);
	m_pool.setMaxThreadCount(1);
}

CFileTransSender::~CFileTransSender()
{
}

bool CFileTransSender::connectToServer(const QString& ipAddr, int port)
{
	m_ipAddress = ipAddr;
	m_commSocket->connectToHost(QHostAddress(ipAddr), port);
	bool isConnect = m_commSocket->waitForConnected(m_timeoutMsec);
	if (isConnect)
	{
		connect(m_commSocket, SIGNAL(readyRead()), this, SLOT(on_read_comm()));
	}
	else
	{
		emit errorState(ERROR_CLASSIFY_EM::K_NETWORK_ERROR, K_NETWORK_ERROR_EM::K_NETWORK_DISCONNECTED_COMMAND_ERROR);
	}
	return isConnect;
}

void CFileTransSender::sendFile(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	if (0 == fileInfo.size())
	{
		emit progressValue(filePath, 100);
		emit fileTransferFinish();
	}
	if (set_file(filePath))
	{
		send_command(SEND_TYPE::FILE_HEAD_CODE);
	}
}

void CFileTransSender::sendFiles(QStringList& fileDirPath)
{
	for (QString& filePath : fileDirPath)
	{
		if (set_file(filePath))
		{
			if (send_command(SEND_TYPE::FILE_HEAD_CODE))
			{
				quitFileTransferThread();
				m_fileTransferFuture = QtConcurrent::run(&m_pool, [this]() {send_file(); });
				m_isRunning = true;
				m_cancelFileTransMutex.lock();
				m_isCancel = false;
				m_cancelFileTransMutex.unlock();
			}
		}
	}
}

bool CFileTransSender::cancelSendFile()
{
	send_command(SEND_TYPE::FILE_CANCEL);
	QSignalSpy spy(this, SIGNAL(cancelSendFileRspSig(int, QVariant)));
	m_isCancel = true;
	if (spy.wait(m_timeoutMsec))
	{
		QList<QVariant> args = spy.takeFirst();
		if (args.at(0).toInt() == (int)SEND_TYPE::FILE_REC_CANCEL)
		{
			return args.at(1).toInt();
		}
	}
	else
	{
		emit errorState(ERROR_CLASSIFY_EM::K_RESPONSE_TIMEOUT, K_RESPONSE_TIMEOUT_EM::K_RESPONSE_CANCEL_FILE_TIMEOUT);
		return false;
	}
	return true;
}

void CFileTransSender::on_read_comm()
{
	QDataStream out(m_commSocket);
	out.setVersion(QDataStream::Qt_5_9);
	if (m_nextBlockSize == 0)
	{
		if (m_commSocket->bytesAvailable() >= sizeof(qint64))
		{
			out >> m_nextBlockSize;
		}
	}
	if (m_commSocket->bytesAvailable() >= m_nextBlockSize)
	{
		QByteArray array;
		out >> array;
		m_cacheData = QString(array);
		dealReadComm();
		m_cacheData.clear();
		m_nextBlockSize = 0;
	}
}

void CFileTransSender::dealReadComm()
{
	auto code = (SEND_TYPE)m_cacheData.section(SPLIT_TYPE_INFO_MAEK, 0, 0).toInt();
	int ret = m_cacheData.section(SPLIT_TYPE_INFO_MAEK, 1, 1).toInt();
	QString additionalInfo = m_cacheData.section(SPLIT_TYPE_INFO_MAEK, 2, 2);
	switch (code)
	{
	case SEND_TYPE::FILE_HEAD_REC_CODE: // 发送文件头响应处理
		if (ret)
		{
			quitFileTransferThread();
			m_fileTransferFuture = QtConcurrent::run(&m_pool, [this]() {send_file(); });
			m_isRunning = true;
			m_cancelFileTransMutex.lock();
			m_isCancel = false;
			m_cancelFileTransMutex.unlock();
		}
		else
		{
			qDebug() << ERROR_CODE_4;
		}
		break;
	case SEND_TYPE::FILE_REC_CODE: // 接收文件数据完成响应处理
		if (ret)
		{
			bool isOk;
			qint64 recv = additionalInfo.toLongLong(&isOk, 10);
			if (m_fileSize > 0)
			{
				int progressVal = (recv * 100) / m_fileSize;
				emit progressValue(m_fileName, progressVal);
			}
			qDebug() << QString::fromLocal8Bit("文件大小: ") << m_fileSize / 1024 << QString::fromLocal8Bit("KB,已接收数据大小: ") << recv / 1024 << "KB";
		}
		else
		{
			qDebug() << ERROR_CODE_5;
		}
		break;
	case SEND_TYPE::FILE_CANCEL:
		m_cancelFileTransMutex.lock();
		m_isCancel = true;
		m_cancelFileTransMutex.unlock();
		m_sendSize = 0;
		if (m_file.isOpen())
		{
			m_file.close();
		}
		emit progressValue(m_fileName, 0);
		emit cancelSendFileRspSig((int)code, QVariant(ret));
		break;
	case SEND_TYPE::FILE_TRANSFER_TIMEOUT_CODE:
		errorState(ERROR_CLASSIFY_EM::K_RESPONSE_TIMEOUT, K_RESPONSE_TIMEOUT_EM::K_RESPONSE_FILE_TRANSFER_TIMEOUT);
		break;
	case SEND_TYPE::FILE_TRANSFER_FINISH_CODE:
		emit fileTransferFinish();
		qDebug() << __FUNCTION__ << m_fileName << QString::fromLocal8Bit("文件传输完成 ");
		break;
	default:
		break;
	}
}

void CFileTransSender::quitFileTransferThread()
{
	while (m_isRunning)
	{
		m_cancelFileTransMutex.lock();
		m_isCancel = true;
		m_cancelFileTransMutex.unlock();
		QThread::msleep(10);
	}
}

void CFileTransSender::send_file()
{
	qint64 len = 0;
	QTcpSocket fileSocket;
	fileSocket.connectToHost(QHostAddress(m_ipAddress), PORT_FILE);
	bool ret = fileSocket.waitForConnected(m_timeoutMsec);
	if (ret)
	{
		connect(&fileSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onFileError(QAbstractSocket::SocketError)));
	}
	else
	{
		emit errorState(ERROR_CLASSIFY_EM::K_NETWORK_ERROR, K_NETWORK_ERROR_EM::K_NETWORK_DISCONNECTED_FILE_ERROR);
	}

	if (!m_file.isOpen())
	{
		bool isOk = m_file.open(QIODevice::ReadOnly);
		if (isOk)
		{
			qDebug() << QString::fromLocal8Bit("开始发送文件数据信息 ") << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
		}
		else
		{
			qDebug() << ERROR_CODE_2;
		}
	}
#if 1
	do 
	{
		QByteArray buf;
		buf = m_file.read(SEND_BLOCK_SIZE);
		len = fileSocket.write(buf);
		fileSocket.waitForBytesWritten();
		m_sendSize += len;		

		m_cancelFileTransMutex.lock();
		if (m_isCancel)
		{
			m_cancelFileTransMutex.unlock();
			break;
		}
		m_cancelFileTransMutex.unlock();

		qDebug() << QString::fromLocal8Bit("====>发送的数据数据大小:") << len / 1024 << "KB" << QString::fromLocal8Bit("\t已发送数据大小:")<< m_fileSize / 1024 << "KB";

	} while (len > 0);
#endif

#if 0
	QByteArray buf;
	buf = m_file.readAll();
	len = fileSocket.write(buf);
	fileSocket.waitForBytesWritten();
	m_sendSize = len;
#endif

	if (m_sendSize == m_fileSize)
	{
		m_sendSize = 0;
		m_file.close();
		fileSocket.disconnect();
		fileSocket.close();
		qDebug() << QString::fromLocal8Bit("文件 ") << m_file.fileName() << QString::fromLocal8Bit("发送完毕 ")
			<< QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
	}
	m_isRunning = false;
}

bool CFileTransSender::send_command(SEND_TYPE type, QVariant additionalInfo /*= ""*/)
{
	QString comm;
	switch (type)
	{
	case SEND_TYPE::FILE_HEAD_CODE:
		comm = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg((int)type).arg(m_fileName + SPLIT_ADDITION_INFO_MARK + QString::number(m_fileSize));
		break;
	case SEND_TYPE::FILE_CANCEL:
		comm = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg((int)type).arg("");
		break;
	case SEND_TYPE::FILE_CODE:
		comm = QString("%1" + QString(SPLIT_TYPE_INFO_MAEK) + "%2").arg((int)type).arg("");
		break;
	default:
		break;
	}
	qint64 len = m_commSocket->write(comm.toUtf8().data());
	m_commSocket->waitForBytesWritten();
	if (len <= 0)
	{
		qDebug() << QString::fromLocal8Bit("命令代号: ") << (int)type << ERROR_CODE_3;
		m_file.close();
		return false;
	}
	return true;
}

bool CFileTransSender::set_file(const QString& filePath)
{
	if (filePath.isEmpty())
	{
		qDebug() << ERROR_CODE_1;
	}
	QFileInfo info(filePath);
	m_fileName = info.absoluteFilePath();
	m_fileSize = info.size();
	m_sendSize = 0;
	if (m_file.isOpen())
	{
		m_file.close();
	}
	m_file.setFileName(filePath);
	bool isOk = m_file.open(QIODevice::ReadOnly);
	if (false == isOk)
	{
		qDebug() << ERROR_CODE_2;
		return false;
	}
	return true;
}

void CFileTransSender::onFileError(QAbstractSocket::SocketError socketError)
{
	qDebug() << __FUNCTION__ << socketError;
}

void CFileTransSender::onCommandError(QAbstractSocket::SocketError)
{
	qDebug() << __FUNCTION__ << m_commSocket->errorString();
}
