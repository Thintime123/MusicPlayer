#include "lyricparser.h"

LyricParser::LyricParser(QObject *parent) : QObject(parent)
{
}

bool LyricParser::loadLrcFile(const QString &filePath)
{
    // 清空现有歌词
    clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开歌词文件:" << filePath;
        return false;
    }

    QTextStream in(&file);
    //in.setCodec("UTF-8");  // 设置编码为UTF-8
    QTextStream stream(&file);
    //指定编码
    stream.setEncoding(QStringConverter::Utf8);


    // 正则表达式用于提取时间标签和歌词内容
    QRegularExpression timeRegex("\\[(\\d+):(\\d+)\\.(\\d+)\\]");

    while (!in.atEnd()) {
        QString line = in.readLine();

        // 处理元信息行
        if (line.startsWith("[ti:")) {
            title = line.mid(4, line.length() - 5);
            continue;
        } else if (line.startsWith("[ar:")) {
            artist = line.mid(4, line.length() - 5);
            continue;
        } else if (line.startsWith("[al:")) {
            album = line.mid(4, line.length() - 5);
            continue;
        }

        // 提取所有时间标签
        QRegularExpressionMatchIterator i = timeRegex.globalMatch(line);

        // 提取歌词内容（时间标签后的文本）
        QString lyricText = line;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            // 找到时间标签
            if (match.hasMatch()) {
                int minutes = match.captured(1).toInt();
                int seconds = match.captured(2).toInt();
                int milliseconds = match.captured(3).toInt();

                // 计算总毫秒数
                qint64 totalTime = minutes * 60 * 1000 + seconds * 1000 + milliseconds * 10;

                // 删除时间标签部分
                lyricText.remove(match.captured(0));

                // 存储时间和对应歌词
                if (!lyricText.isEmpty()) {
                    lyricMap.insert(totalTime, lyricText.trimmed());
                }
            }
        }
    }

    file.close();
    return !lyricMap.isEmpty();
}

QString LyricParser::getLyricByTime(qint64 time)
{
    if (lyricMap.isEmpty()) {
        return QString();
    }

    // 找到第一个时间大于当前时间的项
    auto it = lyricMap.upperBound(time);

    // 如果是第一项，则返回第一句歌词
    if (it == lyricMap.begin()) {
        return lyricMap.first();
    }

    // 否则返回前一项
    --it;
    return it.value();
}

qint64 LyricParser::getNextLyricTime(qint64 currentTime)
{
    if (lyricMap.isEmpty()) {
        return -1;
    }

    // 找到下一句歌词的时间
    auto it = lyricMap.upperBound(currentTime);
    if (it != lyricMap.end()) {
        return it.key();
    }

    return -1; // 没有下一句歌词
}

void LyricParser::clear()
{
    lyricMap.clear();
    title.clear();
    artist.clear();
    album.clear();
}

bool LyricParser::hasLyric() const
{
    return !lyricMap.isEmpty();
}
