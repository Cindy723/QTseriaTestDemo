﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QRegion>
#include <QPainter>
#include <QSystemTrayIcon>
#include "SerialCommunication.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QThread>
#include <qDebug>
#include "MySocketClient.h"
#include "Worker.h"
#include "setting.h"

#include "drag.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define APPVERSION " Version 0.2 "

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 串口通信对象
    SerialCommunication m_SerialCom;
    MySocketClient m_SocketClient;
    void setupConnections();
    void updateOperationResult(QString content);

    // 重写关闭事件处理函数
    void closeEvent(QCloseEvent *event) override;
    void ConnectRecev();

    void setHistoryTextInfo(QString head, QString info);

    /* 界面拖动与圆角实现 */
protected:
        void mousePressEvent(QMouseEvent *event) override {
            if (event->button() == Qt::LeftButton) {
                // 保存当前鼠标位置和窗口位置的差值
                dragStartPosition = event->globalPos() - frameGeometry().topLeft();
                event->accept();
            }
        }

        void mouseMoveEvent(QMouseEvent *event) override {
            if (event->buttons() & Qt::LeftButton) {
                // 移动窗口到新位置
                move(event->globalPos() - dragStartPosition);
                event->accept();
            }
        }

        void paintEvent(QPaintEvent *event) override {
            QPainter painter(this);

            // 设置抗锯齿
            painter.setRenderHint(QPainter::Antialiasing);

            // 背景颜色
            QColor fillColor("#f9f9f9");
            painter.setBrush(fillColor);
            painter.setPen(Qt::transparent);

            // 绘制圆角矩形
            QRect rect = this->rect();
            rect.setWidth(rect.width() - 1);
            rect.setHeight(rect.height() - 1);
            painter.drawRoundedRect(rect, 15, 15);

            // 绘制边界线
            QColor borderColor("#E94560"); // 边界线颜色
            painter.setPen(QPen(borderColor, 1)); // 设置边界线颜色和宽度
            painter.drawRoundedRect(rect, 15, 15);

            // 添加标题横线
            QColor penColor("#E94560");
            painter.setPen(QPen(penColor, 1));
            painter.drawLine(0, 45, width()-2, 43);

            QWidget::paintEvent(event);
        }

        // 快捷指令排序相关
        void dragEnterEvent(QDragEnterEvent* event) override {
            // 确保接收到的是我们期望的数据类型
            if (event->mimeData()->hasText()) {
                event->acceptProposedAction();
            }
        }

        void dragMoveEvent(QDragMoveEvent* event) override {
            event->acceptProposedAction();
        }

        void dropEvent(QDropEvent* event) override {
            // 查找拖动按钮的位置
            QString buttonText = event->mimeData()->text();
            QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());

            if (!layout) return;

            // 将全局鼠标位置转换为当前窗口的本地坐标
            QPoint localPos = event->pos();
            QWidget* targetWidget = nullptr;
            int targetIndex = layout->count();  // 默认插入到最后

            for (int i = 0; i < layout->count(); ++i) {
                QWidget* widget = layout->itemAt(i)->widget();
                if (widget && widget->geometry().contains(localPos)) {
                    targetWidget = widget;
                    targetIndex = i;  // 找到鼠标指向的控件，并记录目标位置
                    break;
                }
            }

            QWidget* sourceButton = nullptr;
            for (int i = 0; i < layout->count(); ++i) {
                DraggableButton* button = qobject_cast<DraggableButton*>(layout->itemAt(i)->widget());
                if (button && button->text() == buttonText) {
                    sourceButton = button;  // 找到拖动的按钮
                    break;
                }
            }

            if (sourceButton && targetWidget) {
                // 移除原按钮并在目标位置重新添加
                layout->removeWidget(sourceButton);
                layout->insertWidget(targetIndex, sourceButton);
                event->acceptProposedAction();
            }
        }

private:
        QRegion createRoundedMask(const QSize &size, int radius) {
            QRegion maskRegion;
            QRect outerRect(QPoint(0, 0), size);
            maskRegion += outerRect;

            // 创建一个椭圆形状，作为窗口的圆角
            QRect innerRect(QPoint(radius, radius), size - QSize(radius * 2, radius * 2));
            maskRegion -= innerRect;
            return maskRegion;
        }

        QPoint dragStartPosition;

        // 托盘
        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;
        QAction *restoreAction;
        QAction *quitAction;

        // 恢复窗口
        void restoreWindow();
        // 处理托盘图标点击事件
        void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
private:
    Ui::MainWindow *ui = nullptr;
    QStringList m_Portlist;
    QString m_CurretProt;
    Setting *m_setWidget;
    int m_totalFastContrl;

    // 串口循环发送线程
    Worker *worker = nullptr;
    QThread *thread = nullptr;

    QStatusBar *statusBar = nullptr;
    QLabel *Lable_currentOperation = nullptr; // Label for current operation
    QLabel *Lable_OperationrResult = nullptr; // Label for operation result
    QLabel *Lable_softwareVersion = nullptr; // Label for software version
    void checkMethodAndSendFun(QByteArray byteArray);
    void loadSettings();
    void saveSettings();
//signals:
public slots:

};


#endif
