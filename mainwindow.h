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
#include <QPropertyAnimation>
#include <QEvent>
#include <QMouseEvent>
#include "lyricparser.h"

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

    // 碟片旋转角度属性访问器
    int getCurRotation() const { return curRotation; }
    void setCurRotation(int rotation) { curRotation = rotation; }

    // 申明为 Q_PROPERTY，使得QPropertyAnimation可以访问
    Q_PROPERTY(int curRotation READ getCurRotation WRITE setCurRotation)

protected:
    bool eventFilter(QObject *watched, QEvent *event) override; // 事件过滤器

private:
    void initButton();
    void setBackGround(const QString &filename);

    void playCurrentMusic();

    void loadMusicList(const QString basePath, bool recursive);
    void loadMusicListWidget(const QString& basePath);



private slots:
    void playAndPause();
    void selectFile();


    void playPreview();
    void playNext();

    void playModeSelect();
    void playStatusChange(QMediaPlayer::MediaStatus status);

    void handleShowListWidget();

    void showAnimation(QWidget* window);
    void hideAnimation(QWidget* window);
    
    void clickListWidgetItem(int row); // 添加处理点击列表项的函数

    void musicPositonChange();
    void progressBarMoved(int positon);

    void volumeBtnClicked();
    void volumeBarMoved(int value);


private:
    Ui::MainWindow *ui;
    QMediaPlayer *player; // 媒体播放器对象
    QAudioOutput *audioOutput; // 音频输出对象
    float defaultVolume = 0.6;

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
    bool isShowVolumeBar;

    QPropertyAnimation *discAnimation;
    int curRotation;
    void initDisc();

private:
    LyricParser *lyricParser; // 歌词解析器
    QString currentLyricFile; // 当前歌词文件路径

    void loadLyric(const QString &musicFileName); // 加载歌词文件
    void updateLyric(qint64 position); // 更新当前显示的歌词

    void initLyricLabel();
};
#endif // MAINWINDOW_H
