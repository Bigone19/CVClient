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
	* @brief: 连接服务端
	* @param: 
	*/
	bool connectToServer(const QString& ipAddr, int port);
	/**
	* @brief: 发送文件
	* @param: 
	*/
	void sendFile(const QString& filePath);
	void sendFiles(QStringList& fileDirPath);
	/**
	* @brief: 取消文件
	* @param: 
	*/
	bool cancelSendFile();
signals:
	/**
	* @brief: 错误状态
	* @param: 
	*/
	void errorState(int code, int subCode);
	/**
	* @brief: 文件传输完成信号
	* @param: 
	*/
	void fileTransferFinish();
	/**
	* @brief: 服务端已接受的数据进度
	* @param: 
	*/
	void progressValue(const QString& fileName, int progressVal);
	/**
	* @brief: 取消文件传输响应信号
	* @param: 
	*/
	void cancelSendFileRspSig(int type, QVariant retValue);
private slots:
	/**
	* @brief: 响应服务端信息
	* @param: 
	*/
	void on_read_comm();
	/**
	* @brief: 文件socket错误
	* @param: 
	*/
	void onFileError(QAbstractSocket::SocketError);
	/**
	* @brief: 命令socket错误
	* @param:
	*/
	void onCommandError(QAbstractSocket::SocketError);

private:
	void dealReadComm();
	/**
	* @brief: 退出文件传输线程
	* @param: 
	*/
	void quitFileTransferThread();
	/**
	* @brief: 发送文件内容数据
	* @param: 
	*/
	void send_file();
	/**
	* @brief: 发送文件信息命令
	* @param: 
	*/
	bool send_command(SEND_TYPE type, QVariant additionalInfo = "");
	/**
	* @brief: 设置文件
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

	qint64 m_nextBlockSize;	// 下一文件块大小
	qint64 m_recvDataSize;	// 已接受数据大小
	QString m_cacheData;	// 缓存数据容器

	int m_timeoutMsec;		// 超时时间
	QString m_ipAddress;

};

#endif // !FILETRANSFERSENDER_H
