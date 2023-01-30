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
	* @brief: ��������
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
	* @brief: ���÷��ؽ��Ŀ¼
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
	QString m_absFilePathInHost;	// �ļ��������е�·���������ļ���
	QString m_fileName;
	qint64 m_fileSize;
	qint64 m_recvSize;				// �������ݴ�С
	QString m_resultDir;			// �ļ�����·��

	bool m_isCancel;
	QDateTime m_startTime;			// ���俪ʼʱ���¼����
	bool m_startTimeFlag;			// ���俪ʼʱ���¼��ʶ
	QTimer* m_timerProgress;		// ��ʱ��ȡ�ѽ������ݴ�С��ʱ��
	bool m_isRunning;
	QByteArrayList m_cacheList;		// �������ݻ������
	QReadWriteLock m_cacheListLock;	// �������ݻ�����ж�д��

	QTimer* m_timeOutProgress;		// ���䳬ʱ��ⶨʱ��
	qint64 m_recvSizeBefore;		// ��ʱ������ݣ���¼��һ���ѽ������ݴ�С
};

#endif // !__RESTRANSFERRECEVICE_H__
