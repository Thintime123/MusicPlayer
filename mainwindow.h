#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QList>
#include <QVideoWidget>
#include <QRandomGenerator>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initButton();
    void setBackGround(const QString &filename);

    void playCurrentMusic();
    void playAndPause();
    void selectFile();

    void loadMusicList(const QString basePath, bool recursive);

    void playPreview();
    void playNext();

    void playModeSelect();
    void playStatusChange(QMediaPlayer::MediaStatus status);

    void loadMusicListWidget(const QString& basePath);

    void handleShowListWidget();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player; // 媒体播放器对象
    QAudioOutput *audioOutput; // 音频输出对象
    //QVideoWidget *videoWidget; // 视频输出对象

    QString filePath;   // 正要播放的音频文件路径

    QString basePath;
    QList<QUrl> musicList; // 音乐列表
    int musicCurIndex = 0; // 当前音乐索引

    enum musicMode {
        orderMode,
        randomMode,
        loopMode,
        singleMode
    };
    musicMode musicCurrentMode;

    bool isShowListWidget;
};
#endif // MAINWINDOW_H
