#ifndef LYRICPARSER_H
#define LYRICPARSER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

class LyricParser : public QObject
{
    Q_OBJECT
public:
    explicit LyricParser(QObject *parent = nullptr);

    // 加载LRC文件
    bool loadLrcFile(const QString &filePath);

    // 根据时间获取对应歌词
    QString getLyricByTime(qint64 time);

    // 获取下一句歌词的时间
    qint64 getNextLyricTime(qint64 currentTime);

    // 清空歌词
    void clear();

    // 当前是否有加载歌词
    bool hasLyric() const;

    QMap<qint64, QString> getAllLyrics() const { return lyricMap; }
private:
    // 存储时间戳(毫秒)和对应的歌词
    QMap<qint64, QString> lyricMap;

    // 存储LRC文件的元信息
    QString title;
    QString artist;
    QString album;

};

#endif // LYRICPARSER_H
