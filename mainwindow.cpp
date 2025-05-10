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
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput); // 设置音频输出对象

    musicCurIndex = 0;
    basePath = ":/res/Audio";

    musicCurrentMode = orderMode;

    isShowListWidget = false;
    loadMusicList(basePath, true); // 加载音乐列表
    loadMusicListWidget(basePath); // 加载音乐列表到列表控件

    setWindowTitle("Music Player");
    initButton();

    // 固定窗口大小并设置背景
    setFixedSize(1080, 720);
    setBackGround(":/res/Background/bkgd.png");

    audioOutput->setVolume(defaultVolume);

    connect(ui->PlayAndPauseBtn, &QPushButton::clicked, this, &MainWindow::playAndPause);

    //connect(ui->ListBtn, &QPushButton::clicked, this, &MainWindow::selectFile);
    connect(ui->ListBtn, &QPushButton::clicked, this, &MainWindow::handleShowListWidget);

    connect(ui->PrevBtn, &QPushButton::clicked, this, &MainWindow::playPreview);

    connect(ui->NextBtn, &QPushButton::clicked, this, &MainWindow::playNext);

    connect(ui->ModeBtn, &QPushButton::clicked, this, &MainWindow::playModeSelect);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::playStatusChange);
    
    // 连接列表项点击信号与槽
    connect(ui->musicListWidget, &QListWidget::currentRowChanged, this, &MainWindow::clickListWidgetItem);

    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::musicPositonChange);

    connect(ui->progressBar, &QSlider::valueChanged, this, &MainWindow::progressBarMoved);

    // 初始化进度条的范围
    ui->progressBar->setRange(0, 100);
    ui->volumeBar->setRange(0, 100);

    ui->volumeBar->setValue(defaultVolume * 100);
    ui->volumeBar->hide();
    isShowVolumeBar = false;
    qApp->installEventFilter(this); // 为整个应用安装事件过滤器

    connect(ui->volumeBtn, &QPushButton::clicked, this, &MainWindow::volumeBtnClicked);

    connect(ui->volumeBar, &QSlider::sliderMoved, this, &MainWindow::volumeBarMoved);

    initDisc();
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
    setButtonStyle(ui->volumeBtn, ":/res/Icon/VolumeOn.png");

    ui->musicListWidget->hide();
    ui->musicListWidget->setStyleSheet("QListWidget {"
                                       "border: none;"
                                       //"color: white;"
                                       "font-size: 16px;"
                                       "border-radius: 20px;"
                                       "background-color: rgba(255, 255, 255, 0.1);"
                                       "}"

                                       "QListWidget::item {"
                                       "border: none;"
                                       "border-radius: 20px;"
                                       "padding: 10px;"
                                       "background-color: rgba(255, 255, 255, 0.2);"
                                       "}"

                                       "QListWidget::item:selected {"
                                       "border: none;"
                                       "background-color: rgba(255, 255, 255, 0.5);"
                                       "boarder-radius: 20px;"
                                       "}");
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


QUrl convertToQrcUrl(const QUrl &fileUrl) {
    if (fileUrl.scheme() == "file" && fileUrl.toString().startsWith("file::")) {
        // 获取资源路径部分（去掉 file:: 前缀）
        QString path = fileUrl.toString().mid(6); // "file::".length() = 6

        return QUrl("qrc:" + path);
    }
    return fileUrl;
}

void MainWindow::playCurrentMusic() {
    qDebug() << "\nplayCurrentMusic called!";

    if(musicCurIndex >= 0 && musicCurIndex < musicList.size()) {
        qDebug() << "Playing music:" << musicList[musicCurIndex];

        qDebug() << "Player state(last):" << player->playbackState();

        player->setSource(convertToQrcUrl(musicList[musicCurIndex]));
        player->play();
        ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Pause.png"));

        discAnimation->stop();
        setCurRotation(0);
        discAnimation->start();
    } else {
        qDebug() << "Invalid music index:" << musicCurIndex;
    }
}

void MainWindow::playAndPause()
{
    qDebug() << "playAndPause called!";
    // 如果播放器正在播放，则暂停
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
        ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Play.png"));
        discAnimation->pause();
    } else {
        // 否则，开始播放
        if(player->playbackState() == QMediaPlayer::StoppedState) {
            playCurrentMusic();
        } else if(player->playbackState() == QMediaPlayer::PausedState){
            player->play();
            ui->PlayAndPauseBtn->setIcon(QIcon(":/res/Icon/Pause.png"));
            discAnimation->resume();
        }
    }
    qDebug() << "Player state:" << player->playbackState();
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

    if(filePath_1.isEmpty()) qDebug() << "The filePath is invalid!";
    else {
        filePath = filePath_1;
        qDebug() << "Selected file:" << filePath;

        QString baseDir = basePath; // 基础目录（相对路径）
        QDir dir("../../");

        QString baseDirAbs = dir.absolutePath(); // 基础目录（绝对路径）
        QDir dirAbs(baseDirAbs);

        qDebug() << "基础目录相对路径baseDir: " << baseDir;
        qDebug() << "基础目录绝对路径baseDirAbs: " << baseDirAbs;

        QString relativePath = dirAbs.relativeFilePath(filePath);

        qDebug() << "relativePath:" << relativePath;

        int foundIndex = -1;

        for(int i = 0; i < musicList.size(); i++) {
            QString temp = musicList[i].path();
            //qDebug() << "temp_:" << temp;
            if(temp.startsWith(":/")) {
                temp = temp.mid(2); // 去掉前缀
            }
            qDebug() << "temp:" << temp;
            if(temp == relativePath) {
                foundIndex = i;
                break;
            }
        }
        qDebug() << "找到相对路径:" << relativePath << "在musicList中的索引为:" << foundIndex;

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



    for (const QFileInfo &fileInfo : fileList) {
        if (fileInfo.isFile()) {
            QString filePath = fileInfo.absoluteFilePath();
            qDebug() << "Found audio file:" << filePath;

            // 将音乐文件添加到播放列表中
            QUrl url = QUrl::fromLocalFile(filePath);
            musicList.append(url);
            qDebug() << "Found audio fileUrl: " << url;
        }
    }

    qDebug() << "The size of musicList(loadMusicList) : " << musicList.size();

    if (recursive) {
        // 获取当前目录中的所有子目录（排除.和..）
        QStringList subdirs = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);

        for (const QString &subdir : subdirs) {
            QString subdirPath = basePath + "/" + subdir;
            loadMusicList(subdirPath, recursive);
        }

    }
}

void MainWindow::playPreview()
{
    qDebug() << "playPreview called! List size:" << musicList.size() <<
        "Current index(last):" << musicCurIndex;
    if(musicList.isEmpty()) return;

    if(musicCurIndex > 0) {
        musicCurIndex--;
    } else {
        musicCurIndex = musicList.size() - 1;
    }

    qDebug() << "musicCurIndex(now) = " << musicCurIndex;

    playCurrentMusic();
}

void MainWindow::playNext()
{
    qDebug() << "playNext called! List size:" << musicList.size() <<
        "Current index(last):" << musicCurIndex;

    if(musicList.isEmpty()) {
        qDebug() << "musicList is empty!";
        return;
    }

    if(musicCurIndex < musicList.size() - 1) {
        musicCurIndex++;
    } else {
        musicCurIndex = 0;
    }

    qDebug() << "musicCurIndex(now) = " << musicCurIndex;

    playCurrentMusic();
}

void MainWindow::playModeSelect()
{
    qDebug() << "\nplayModeSelect called!";

    if(musicCurrentMode == orderMode) {
        musicCurrentMode = randomMode;
        ui->ModeBtn->setIcon(QIcon(":/res/Icon/Random.png"));

        qDebug() << "musicCurrntMode = randomMode";
    } else if(musicCurrentMode == randomMode) {
        musicCurrentMode = loopMode;
        ui->ModeBtn->setIcon(QIcon(":/res/Icon/Loop.png"));

        qDebug() << "musicCurrntMode = loopMode";
    } else if(musicCurrentMode == loopMode) {
        musicCurrentMode = singleMode;
        ui->ModeBtn->setIcon(QIcon(":/res/Icon/Single.png"));

        qDebug() << "musicCurrntMode = singleMode";
    } else if(musicCurrentMode == singleMode) {
        musicCurrentMode = orderMode;
        ui->ModeBtn->setIcon(QIcon(":/res/Icon/Order.png"));

        qDebug() << "musicCurrntMode = orderMode";
    }
}

void MainWindow::playStatusChange(QMediaPlayer::MediaStatus status) {
    if(status == QMediaPlayer::EndOfMedia) {
        if(musicCurrentMode == orderMode) {
            if(musicCurIndex < musicList.size() - 1) {
                playNext();
            }
        } else if(musicCurrentMode == randomMode) {
            int oldIndex = musicCurIndex;
            do {
                musicCurIndex = QRandomGenerator::global()->bounded(musicList.size());
            } while (musicList.size() > 1 && musicCurIndex == oldIndex);
            playCurrentMusic();
        } else if(musicCurrentMode == loopMode) {
            playNext();
        } else if(musicCurrentMode == singleMode) {
            playCurrentMusic();
        }
    }
    ui->musicListWidget->setCurrentRow(musicCurIndex);
}

void MainWindow::loadMusicListWidget(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        QMessageBox::warning(this, "Error", "Directory does not exist.");
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

    for(auto element : fileList)
    {
        ui->musicListWidget->addItem(element.baseName());
    }

    ui->musicListWidget->setCurrentRow(0);
}

void MainWindow::handleShowListWidget()
{
    if(isShowListWidget) {
        // 使用侧边滑出动画隐藏列表
        hideAnimation(ui->musicListWidget);
        isShowListWidget = false;
    } else {
        // 使用侧边滑入动画显示列表
        showAnimation(ui->musicListWidget);
        isShowListWidget = true;
    }
}

void MainWindow::showAnimation(QWidget* widget)
{
    // 设置初始位置（在窗口右侧不可见区域）
    int endX = this->width() - widget->width(); // 最终位置
    int startX = this->width(); // 开始位置（窗口右边界外）
    
    widget->move(this->width(), 0);
    widget->show();
    
    // 创建位置动画
    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(300); // 动画持续时间为300毫秒
    
    // 设置起始值（窗口右边缘）和结束值（目标位置）
    animation->setStartValue(QRect(startX, 0, widget->width(), widget->height()));
    animation->setEndValue(QRect(endX, 0, widget->width(), widget->height()));
    
    // 设置缓动曲线，使动画更自然
    animation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 启动动画
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    
    connect(animation, &QPropertyAnimation::finished, [=]() {
        qDebug() << "滑入动画完成";
    });
}

void MainWindow::hideAnimation(QWidget* widget)
{
    // 创建位置动画
    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry");
    animation->setDuration(300); // 动画持续时间为300毫秒
    
    int startX = this->width() - widget->width(); // 当前位置
    int endX = this->width(); // 目标位置（窗口右边界外）
    
    // 设置起始值（当前位置）和结束值（窗口右边缘）
    animation->setStartValue(QRect(startX, 0, widget->width(), widget->height()));
    animation->setEndValue(QRect(endX, 0, widget->width(), widget->height()));
    
    // 设置缓动曲线，使动画更自然
    animation->setEasingCurve(QEasingCurve::InCubic);
    
    // 启动动画
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    
    // 当动画完成时隐藏控件
    connect(animation, &QPropertyAnimation::finished, [=]() {
        widget->hide();
        qDebug() << "滑出动画完成";
    });
}

void MainWindow::clickListWidgetItem(int row)
{
    qDebug() << "列表项被点击，行号:" << row;
    
    // 检查点击的行号是否合法
    if (row >= 0 && row < musicList.size()) {
        musicCurIndex = row;
        playCurrentMusic();
    } else {
        qDebug() << "无效的行号:" << row;
    }
}

void MainWindow::musicPositonChange()
{
    // 如果播放器没有媒体或正在寻找，则返回
    if (player->mediaStatus() == QMediaPlayer::NoMedia || 
        player->mediaStatus() == QMediaPlayer::LoadingMedia) {
        return;
    }
    
    // 获取当前播放位置（毫秒）
    qint64 position = player->position();
    
    // 获取音频总时长（毫秒）
    qint64 duration = player->duration();
    
    // 避免除零错误，确保总时长大于0
    if (duration <= 0) {
        return;
    }
    
    // 更新进度条，阻断信号以避免循环触发
    ui->progressBar->blockSignals(true);
    ui->progressBar->setRange(0, duration);
    ui->progressBar->setValue(position);
    ui->progressBar->blockSignals(false);
    
    // 更新时间标签
    // 将毫秒转换为时:分:秒格式
    int positionSeconds = position / 1000;
    int hours = positionSeconds / 3600;
    int minutes = (positionSeconds % 3600) / 60;
    int seconds = positionSeconds % 60;
    
    int durationSeconds = duration / 1000;
    int durationHours = durationSeconds / 3600;
    int durationMinutes = (durationSeconds % 3600) / 60;
    int durationSeconds2 = durationSeconds % 60;
    
    // 更新当前时间和总时长标签
    QString currentTimeStr = QString("%1:%2:%3")
                              .arg(hours, 2, 10, QChar('0'))
                              .arg(minutes, 2, 10, QChar('0'))
                              .arg(seconds, 2, 10, QChar('0'));
    
    QString totalTimeStr = QString("%1:%2:%3")
                            .arg(durationHours, 2, 10, QChar('0'))
                            .arg(durationMinutes, 2, 10, QChar('0'))
                            .arg(durationSeconds2, 2, 10, QChar('0'));
    
    ui->currentTime->setText(currentTimeStr);
    ui->endTime->setText(totalTimeStr);
    
    qDebug() << "Position:" << currentTimeStr << "/" << totalTimeStr;
}

void MainWindow::progressBarMoved(int position)
{
    if (player->mediaStatus() != QMediaPlayer::NoMedia && 
        player->mediaStatus() != QMediaPlayer::LoadingMedia) {
        
        qDebug() << "进度条移动到:" << position << "毫秒";
        player->setPosition(position);
    }
}

void MainWindow::volumeBtnClicked()
{
    // if(ui->volumeBar->isHidden()) {
    //     ui->volumeBar->show();
    //     ui->volumeBtn->setIcon(QIcon(":/res/Icon/VolumeOn.png"));
    //     if(ui->volumeBar->value() == 0) {
    //         ui->volumeBar->setValue(50);
    //     }
    //     audioOutput->setVolume(ui->volumeBar->value() / 100.0);
    // } else {
    //     // ui->volumeBar->hide();
    //     ui->volumeBtn->setIcon(QIcon(":/res/Icon/VolumeOff.png"));
    //     audioOutput->setVolume(0);
    //     ui->volumeBar->setValue(0);
    // }

    if(ui->volumeBar->value()) {
        if(ui->volumeBar->isHidden()) {
            ui->volumeBar->show();
            isShowVolumeBar = true;
        } else {
            ui->volumeBtn->setIcon(QIcon(":/res/Icon/VolumeOff.png"));
            defaultVolume = ui->volumeBar->value() / 100.0;
            ui->volumeBar->setValue(0);
            audioOutput->setVolume(0);
        }
    } else {
        ui->volumeBar->show();
        isShowVolumeBar = true;
        ui->volumeBar->setValue(defaultVolume * 100);
        audioOutput->setVolume(defaultVolume);
        ui->volumeBtn->setIcon(QIcon(":/res/Icon/VolumeOn.png"));
    }
}

void MainWindow::volumeBarMoved(int value)
{
    audioOutput->setVolume(value / 100.0);

    if(value == 0) {
        ui->volumeBtn->setIcon(QIcon(":/res/Icon/VolumeOff.png"));
    } else {
        ui->volumeBtn->setIcon(QIcon(":/res/Icon/VolumeOn.png"));
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        // 获取点击位置的控件
        QWidget *clickedWidget = QApplication::widgetAt(mouseEvent->globalPosition().toPoint());

        // 检查点击是否在音量控制区域外
        bool clickOutsideVolume = true;

        if (clickedWidget) {
            if (clickedWidget == ui->volumeBtn ||
                clickedWidget == ui->volumeBar ||
                ui->volumeBtn->isAncestorOf(clickedWidget) ||
                ui->volumeBar->isAncestorOf(clickedWidget)) {
                clickOutsideVolume = false;
            }
        }

        if (clickOutsideVolume && ui->volumeBar->isVisible()) {
            ui->volumeBar->hide();
            // QPropertyAnimation *animation = new QPropertyAnimation(ui->volumeBar, "geometry", this);
            // animation->setDuration(200); // 200毫秒
            // animation->setStartValue(ui->volumeBar->geometry());
            // animation->setEndValue(QRect(ui->volumeBar->x(),
            //                              ui->volumeBar->y(),
            //                              ui->volumeBar->width(),
            //                              0));
            // animation->start(QAbstractAnimation::DeleteWhenStopped);

            // connect(animation, &QPropertyAnimation::finished, [this]() {
            //     ui->volumeBar->hide();
            // });
            return true; // 事件已处理
        }
    }

    // 其他事件继续传递
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::initDisc()
{
    //
    QPixmap discImage(":/res/Icon/Disc.png");
    ui->disc->setPixmap(discImage);
    ui->disc->setScaledContents(true); // 确保图像缩放以填充整个标签

    // 设置旋转中心点
    ui->disc->setAlignment(Qt::AlignCenter);

    // 初始化旋转动画
    discAnimation = new QPropertyAnimation(this, "curRotation");
    discAnimation->setDuration(10000); // 10秒转一圈
    discAnimation->setStartValue(0);
    discAnimation->setEndValue(360);
    discAnimation->setLoopCount(-1); // 无限循环

    connect(discAnimation, &QPropertyAnimation::valueChanged, this, [this](const QVariant &value) {
        // 每当值变化时更新碟片旋转角度
        int rotation = value.toInt();

        // 创建变换矩阵
        QTransform transform;
        // 将原点移到标签中心
        transform.translate(ui->disc->width()/2, ui->disc->height()/2);

        transform.rotate(rotation);
        // 将原点移回左上角
        transform.translate(-ui->disc->width()/2, -ui->disc->height()/2);

        // 获取原始图像
        QPixmap originalPixmap(":/res/Icon/Disc.png");
        // 应用旋转变换
        QPixmap rotatedPixmap = originalPixmap.transformed(transform, Qt::SmoothTransformation);

        // 设置旋转后的图像
        ui->disc->setPixmap(rotatedPixmap);
    });

    curRotation = 0;
}
