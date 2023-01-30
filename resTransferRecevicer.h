#pragma once
#ifndef __RESTRANSFERRECEVICE_H__
#define __RESTRANSFERRECEVICE_H__

#include "config.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

class CResTransRecevicer : public QObject
{
	Q_OBJECT
public:
	/**
	* @brief: 发送命令
	* @param:
	*/
	void send_command(SEND_TYPE code, int ret, QString additional = "");

	static CResTransRecevicer* GetInstance()
	{
		if (m_recevicer == nullptr)
		{
			m_recevicer = new CResTransRecevicer;
		}
		return m_recevicer;
	}

public slots:
	void progress();
	void timeOutCheck();
	void on_connect_c();
	void on_connect_f();

	void on_read_command();
	void on_read_file();
	void onFileError(QAbstractSocket::SocketError);
	void onCommandError(QAbstractSocket::SocketError);

private:
	/**
	* @brief: 设置返回结果目录
	* @param:
	*/
	void setResultDir(const QString& dirPath);
	void beginSave();
	void quitFileTransferThread();
	CResTransRecevicer(QObject* parent = nullptr);
	~CResTransRecevicer();
private:
	static CResTransRecevicer* m_recevicer;

	QTcpServer* m_tcpServerComm;
	QTcpServer* m_tcpServerFile;
	QTcpSocket* m_commSocket;
	QTcpSocket* m_fileSocket;

	QFile m_file;
	QString m_absFilePathInHost;	// 文件在主机中的路径，包含文件名
	QString m_fileName;
	qint64 m_fileSize;
	qint64 m_recvSize;				// 接收数据大小
	QString m_resultDir;			// 文件缓存路径

	bool m_isCancel;
	QDateTime m_startTime;			// 传输开始时间记录对象
	bool m_startTimeFlag;			// 传输开始时间记录标识
	QTimer* m_timerProgress;		// 定时获取已接收数据大小定时器
	bool m_isRunning;
	QByteArrayList m_cacheList;		// 接收数据缓存队列
	QReadWriteLock m_cacheListLock;	// 接收数据缓存队列读写锁

	QTimer* m_timeOutProgress;		// 传输超时监测定时器
	qint64 m_recvSizeBefore;		// 定时检测数据，记录上一次已接收数据大小
};

#endif // !__RESTRANSFERRECEVICE_H__
