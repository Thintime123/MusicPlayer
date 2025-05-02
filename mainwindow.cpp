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

    qDebug() << "\n\nstart*******************\n\n";
    player = new QMediaPlayer(this); // 初始化媒体播放器对象
    audioOutput = new QAudioOutput(this); // 初始化音频输出对象
    player->setAudioOutput(audioOutput); // 设置音频输出对象

    musicCurIndex = 0;
    loadMusicList("res/Audio/", true); // 加载音乐列表

    qDebug() << musicList.size();

    setWindowTitle("Music Player");
    initButton();

    // 固定窗口大小并设置背景
    setFixedSize(1080, 720);
    setBackGround(":/res/Background/bkgd.png");

    audioOutput->setVolume(0.5); // 设置音量为50%

    // 信号与槽连接--开始与暂停
    connect(ui->PlayAndPauseBtn, &QPushButton::clicked, this, &MainWindow::playAndPause);

    // 信号与槽连接--选择文件
    connect(ui->ListBtn, &QPushButton::clicked, this, &MainWindow::selectFile);

    // 信号与槽连接--上一首
    connect(ui->PrevBtn, &QPushButton::clicked, this, &MainWindow::playPreview);

    // 信号与槽连接--下一首
    connect(ui->NextBtn, &QPushButton::clicked, this, &MainWindow::playNext);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void setButtonStyle(QPushButton *button, const QString Iconpath)
{
    // 设置按钮的大小、图标和样式（透明化）
    button->setFixedSize(50, 50);
    button->setIcon(QIcon(Iconpath));
    button->setIconSize(QSize(button->width(), button->height()));
    button->setStyleSheet("background-color: transparent; border: none;");
}

void MainWindow::initButton()
{
    setButtonStyle(ui->PrevBtn, ":/res/Icon/Prev.png");
    setButtonStyle(ui->PlayAndPauseBtn, ":/res/Icon/Play.png");
    setButtonStyle(ui->NextBtn, ":/res/Icon/Next.png");
    setButtonStyle(ui->ModeBtn, ":/res/Icon/Order.png");
    setButtonStyle(ui->ListBtn, ":/res/Icon/Music.png");
}

void MainWindow::setBackGround(const QString &filename)
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

void MainWindow::playAndPause()
{
    // 如果播放器正在播放，则暂停
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
        ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Play.png"));
    } else {
        // 否则，开始播放
        //player->setSource(QUrl("qrc:/res/Audio/M800001ziKgJ3o5Ipp.mp3")); // 设置音乐文件路径
        if(filePath.isEmpty()) qDebug() << "The filePath is unvalid!";
        else {
            player->play();
            ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Pause.png"));
        }
    }
}

void MainWindow::selectFile()
{
    QString filePath_1;
    filePath_1 = QFileDialog::getOpenFileName(
        this,
        "Selec an audio file",
        "../../res/Audio",
        "Audio files(*.mp3 *wav *.flac *.ogg);;All files(*)"
    );

    if(filePath_1.isEmpty()) qDebug() << "The filePath is unvalid!";
    else {
        filePath = filePath_1;
        player->setSource(QUrl(filePath));
        player->play();
        ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Pause.png"));
    }
}

void MainWindow::loadMusicList(const QString basePath, bool recursive)
{
    // 检查目录是否存在
    QDir dir(basePath);
    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << basePath;
        return;
    }

    // 设置文件过滤器，只查找指定格式的音频文件
    QStringList filters; // 音频文件格式
    filters << "*.mp3" << "*.wav" << "*.flac" << "*.ogg";
    dir.setNameFilters(filters);

    // 设置过滤标志，只查找文件（不包括符号链接）
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    // 获取符合条件的文件列表
    QFileInfoList fileList = dir.entryInfoList();

    // for (const QFileInfo &fileInfo : fileList) {
    //     if (fileInfo.isFile()) {
    //         QString filePath = fileInfo.absoluteFilePath();
    //         qDebug() << "Found audio file:" << filePath;

    //         // 将音乐文件添加到播放列表中
    //         QUrl url = QUrl::fromLocalFile(filePath);
    //         musicList.append(url);
    //     }
    // }
    for (int i = 0; i < fileList.size(); i++) {
        QFileInfo fileInfo = fileList[i];
        if (fileInfo.isFile()) {
            QString filePath = fileInfo.absoluteFilePath();
            qDebug() << "Found audio file:" << filePath;

            // 将音乐文件添加到播放列表中
            QUrl url = QUrl::fromLocalFile(filePath);
            musicList.append(url);
        }
    }
    // qDebug() << "The size of musicList : " << musicList.size();

    if (recursive) {
        // 获取当前目录中的所有子目录（排除.和..）
        QStringList subdirs = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);

        // for (const QString &subdir : subdirs) {
        //     QString subdirPath = basePath + "/" + subdir;
        //     loadMusicList(subdirPath, recursive);
        // }
        for (int i = 0; i < subdirs.size(); i++) {
            QString subdirPath = basePath + "/" + subdirs[i];
            loadMusicList(subdirPath, recursive);
        }
    }
}

void MainWindow::playPreview()
{
    qDebug() << "playPreview called! List size:" << musicList.size() <<
        "Current index:" << musicCurIndex;
    if(musicList.isEmpty()) return;

    if(musicCurIndex > 0) {
        musicCurIndex--;
    } else {
        musicCurIndex = musicList.size() - 1;
    }
    player->pause();
    player->setSource(musicList.at(musicCurIndex));
    player->play();
    ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Pause.png"));
}

void MainWindow::playNext()
{
    qDebug() << "The size of musicList : " << musicList.size();
    if(musicList.isEmpty()) {
        qDebug() << "musicList is empty!";
        return;
    }
    if(musicCurIndex < musicList.size() - 1) {
        musicCurIndex++;
    } else {
        musicCurIndex = 0;
    }
    player->pause();
    player->setSource(musicList.at(musicCurIndex));
    player->play();
    ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Pause.png"));
}

