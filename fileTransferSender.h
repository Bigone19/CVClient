#pragma once
#ifndef FILETRANSFERSENDER_H
#define FILETRANSFERSENDER_H

#include "config.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>
#include <QMutex>
#include <QFuture>
#include <vector>

using std::vector;

class CFileTransSender : public QObject
{
	Q_OBJECT
public:
	CFileTransSender(QObject* parent = nullptr);
	~CFileTransSender();

	/**
	* @brief: ���ӷ����
	* @param: 
	*/
	bool connectToServer(const QString& ipAddr, int port);
	/**
	* @brief: �����ļ�
	* @param: 
	*/
	void sendFile(const QString& filePath);
	void sendFiles(QStringList& fileDirPath);
	/**
	* @brief: ȡ���ļ�
	* @param: 
	*/
	bool cancelSendFile();
signals:
	/**
	* @brief: ����״̬
	* @param: 
	*/
	void errorState(int code, int subCode);
	/**
	* @brief: �ļ���������ź�
	* @param: 
	*/
	void fileTransferFinish();
	/**
	* @brief: ������ѽ��ܵ����ݽ���
	* @param: 
	*/
	void progressValue(const QString& fileName, int progressVal);
	/**
	* @brief: ȡ���ļ�������Ӧ�ź�
	* @param: 
	*/
	void cancelSendFileRspSig(int type, QVariant retValue);
private slots:
	/**
	* @brief: ��Ӧ�������Ϣ
	* @param: 
	*/
	void on_read_comm();
	/**
	* @brief: �ļ�socket����
	* @param: 
	*/
	void onFileError(QAbstractSocket::SocketError);
	/**
	* @brief: ����socket����
	* @param:
	*/
	void onCommandError(QAbstractSocket::SocketError);

private:
	void dealReadComm();
	/**
	* @brief: �˳��ļ������߳�
	* @param: 
	*/
	void quitFileTransferThread();
	/**
	* @brief: �����ļ���������
	* @param: 
	*/
	void send_file();
	/**
	* @brief: �����ļ���Ϣ����
	* @param: 
	*/
	bool send_command(SEND_TYPE type, QVariant additionalInfo = "");
	/**
	* @brief: �����ļ�
	* @param: 
	*/
	bool set_file(const QString& filePath);
private:
	QTcpSocket* m_commSocket;
	QFile m_file;
	QString m_fileName;
	qint64 m_fileSize;

	qint64 m_sendSize;
	QThreadPool m_pool;
	QFuture<void> m_fileTransferFuture;
	QMutex m_cancelFileTransMutex;
	bool m_isCancel;
	bool m_isRunning;

	qint64 m_nextBlockSize;	// ��һ�ļ����С
	qint64 m_recvDataSize;	// �ѽ������ݴ�С
	QString m_cacheData;	// ������������

	int m_timeoutMsec;		// ��ʱʱ��
	QString m_ipAddress;

};

#endif // !FILETRANSFERSENDER_H
