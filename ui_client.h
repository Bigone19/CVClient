/********************************************************************************
** Form generated from reading UI file 'client.ui'
**
** Created by: Qt User Interface Compiler version 6.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CClient
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btnConnectServer;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *filePostion;
    QPushButton *btnSelectFile;
    QPushButton *btnTransfer;
    QHBoxLayout *horizontalLayout_2;
    QProgressBar *barProgress;
    QPushButton *btnCancel;
    QLabel *labelImgDisplay;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *CClient)
    {
        if (CClient->objectName().isEmpty())
            CClient->setObjectName("CClient");
        CClient->resize(685, 758);
        centralwidget = new QWidget(CClient);
        centralwidget->setObjectName("centralwidget");
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(30, 10, 642, 722));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        btnConnectServer = new QPushButton(layoutWidget);
        btnConnectServer->setObjectName("btnConnectServer");

        horizontalLayout_3->addWidget(btnConnectServer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        filePostion = new QLineEdit(layoutWidget);
        filePostion->setObjectName("filePostion");

        horizontalLayout->addWidget(filePostion);

        btnSelectFile = new QPushButton(layoutWidget);
        btnSelectFile->setObjectName("btnSelectFile");

        horizontalLayout->addWidget(btnSelectFile);

        btnTransfer = new QPushButton(layoutWidget);
        btnTransfer->setObjectName("btnTransfer");

        horizontalLayout->addWidget(btnTransfer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        barProgress = new QProgressBar(layoutWidget);
        barProgress->setObjectName("barProgress");
        barProgress->setValue(0);

        horizontalLayout_2->addWidget(barProgress);

        btnCancel = new QPushButton(layoutWidget);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout_2->addWidget(btnCancel);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout_3);

        labelImgDisplay = new QLabel(layoutWidget);
        labelImgDisplay->setObjectName("labelImgDisplay");
        labelImgDisplay->setMinimumSize(QSize(640, 640));

        verticalLayout_2->addWidget(labelImgDisplay);

        CClient->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(CClient);
        statusbar->setObjectName("statusbar");
        CClient->setStatusBar(statusbar);

        retranslateUi(CClient);

        QMetaObject::connectSlotsByName(CClient);
    } // setupUi

    void retranslateUi(QMainWindow *CClient)
    {
        CClient->setWindowTitle(QCoreApplication::translate("CClient", "CClient", nullptr));
        btnConnectServer->setText(QCoreApplication::translate("CClient", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        btnSelectFile->setText(QCoreApplication::translate("CClient", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        btnTransfer->setText(QCoreApplication::translate("CClient", "\344\270\212\344\274\240\346\226\207\344\273\266", nullptr));
        btnCancel->setText(QCoreApplication::translate("CClient", "\345\217\226\346\266\210\344\270\212\344\274\240", nullptr));
        labelImgDisplay->setText(QCoreApplication::translate("CClient", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CClient: public Ui_CClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENT_H
