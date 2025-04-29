#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPixmap>
#include <QPalette>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Music Player");
    InitButton();

    // 固定窗口大小并设置背景
    setFixedSize(1080, 720);
    SetBackGround(":/res/Background/Sky.png");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void SetButtonStyle(QPushButton *button, const QString Iconpath)
{
    // 设置按钮的大小、图标和样式（透明化）
    button->setFixedSize(50, 50);
    button->setIcon(QIcon(Iconpath));
    button->setIconSize(QSize(button->width(), button->height()));
    button->setStyleSheet("background-color: transparent; border: none;");
}

void MainWindow::InitButton()
{
    SetButtonStyle(ui->PrevBtn, ":/res/Icon/Prev.png");
    SetButtonStyle(ui->PlayAndPauseBtn, ":/res/Icon/Play.png");
    SetButtonStyle(ui->NextBtn, ":/res/Icon/Next.png");
    SetButtonStyle(ui->ModeBtn, ":/res/Icon/Order.png");
    SetButtonStyle(ui->ListBtn, ":/res/Icon/Music.png");
}

void MainWindow::SetBackGround(const QString &filename)
{
    if (!QFile::exists(filename)) {
        qDebug() << "Background file not found:" << filename;
    }
    // 创建照片对象
    QPixmap pixmap(filename);

    // 获取窗口大小
    QSize windowSize = this->size();

    this->setAutoFillBackground(true);
    // 将图片缩放到当前窗口的大小
    QPixmap scalePixmap = pixmap.scaled(windowSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // 创建画布 palette 对象并设置照片
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(scalePixmap));

    // 将画布应用到窗口
    this->setPalette(palette);

}
