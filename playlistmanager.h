#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QList>
#include <QRandomGenerator>
#include <QDebug>

enum PlayMode {
    orderMode,
    randomMode,
    loopMode,
    singleMode
};

/**
 * @brief 播放列表管理类模板
 * 
 * 一个通用的播放列表管理类模板，可以处理任何类型的音频资源
 * T: 音频资源类型，如QUrl、QString等
 */
template <typename T>
class PlaylistManager {
private:
    QList<T> items; // 播放列表项目
    int currentIndex;
    PlayMode currentMode;
    
public:
    PlaylistManager() : currentIndex(0), currentMode(orderMode) {
        qDebug() << "创建播放列表管理器";
    }

    void addItem(const T& item) {
        items.append(item);
        qDebug() << "添加项目到播放列表，当前大小: " << items.size();
    }

    void addItems(const QList<T>& newItems) {
        items.append(newItems);
        qDebug() << "添加多个项目到播放列表，当前大小: " << items.size();
    }
    
    bool removeItem(int index) {
        if (index >= 0 && index < items.size()) {
            items.removeAt(index);
            
            // 如果移除的是当前项目之前的项目，更新当前索引
            if (index < currentIndex) {
                currentIndex--;
            }
            // 如果移除的是当前项目，需要确保当前索引合法
            else if (index == currentIndex) {
                if (currentIndex >= items.size()) {
                    currentIndex = items.isEmpty() ? 0 : items.size() - 1;
                }
            }
            
            qDebug() << "从播放列表移除项目，当前大小: " << items.size() << "，当前索引: " << currentIndex;
            return true;
        }
        qDebug() << "无法移除项目，索引无效: " << index;
        return false;
    }
    
    // brief 清空播放列表
    void clear() {
        items.clear();
        currentIndex = 0;
        qDebug() << "清空播放列表";
    }
    
    T getCurrentItem() const {
        if (!items.isEmpty() && currentIndex >= 0 && currentIndex < items.size()) {
            return items[currentIndex];
        }
        qDebug() << "播放列表为空或索引无效，无法获取当前项目";
        return T(); // 返回默认构造的T类型对象
    }
    
    T getNextItem() {
        if (items.isEmpty()) {
            qDebug() << "播放列表为空，无法获取下一个项目";
            return T();
        }
        
        // 根据播放模式决定下一个项目
        switch (currentMode) {
            case orderMode:
                // 顺序播放：如果是最后一个，则停留在最后一个
                if (currentIndex < items.size() - 1) {
                    currentIndex++;
                }
                break;
                
            case randomMode:
                // 随机播放：随机选择一个不同的索引
                if (items.size() > 1) {
                    int oldIndex = currentIndex;
                    do {
                        currentIndex = QRandomGenerator::global()->bounded(items.size());
                    } while (currentIndex == oldIndex);
                }
                break;
                
            case loopMode:
                // 循环播放：如果是最后一个，则回到第一个
                currentIndex = (currentIndex + 1) % items.size();
                break;
                
            case singleMode:
                // 单曲循环：索引不变
                break;
        }
        
        qDebug() << "获取下一个项目，当前索引: " << currentIndex;
        return getCurrentItem();
    }
    
    // 获取上一个项目并更新当前索引
    T getPreviousItem() {
        if (items.isEmpty()) {
            qDebug() << "播放列表为空，无法获取上一个项目";
            return T();
        }
        
        switch (currentMode) {
            case orderMode:
            case loopMode:
                // 顺序 or 循环：如果是第一个，则循环到最后一个
                currentIndex = (currentIndex > 0) ? (currentIndex - 1) : (items.size() - 1);
                break;
                
            case randomMode:
                // 随机播放：随机选择一个不同的索引
                if (items.size() > 1) {
                    int oldIndex = currentIndex;
                    do {
                        currentIndex = QRandomGenerator::global()->bounded(items.size());
                    } while (currentIndex == oldIndex);
                }
                break;
                
            case singleMode:
                // 单曲循环：索引不变
                break;
        }
        
        qDebug() << "获取上一个项目，当前索引: " << currentIndex;
        return getCurrentItem();
    }
    
    bool setCurrentIndex(int index) {
        if (index >= 0 && index < items.size()) {
            currentIndex = index;
            qDebug() << "设置当前索引: " << currentIndex;
            return true;
        }
        qDebug() << "无法设置当前索引，索引无效: " << index;
        return false;
    }
    
    int getCurrentIndex() const {
        return currentIndex;
    }
    
    int size() const {
        return items.size();
    }
    
    bool isEmpty() const {
        return items.isEmpty();
    }
    
    QList<T> getAllItems() const {
        return items;
    }
    
    // 获取指定索引的项目
    T getItemAt(int index) const {
        if (index >= 0 && index < items.size()) {
            return items[index];
        }
        qDebug() << "无法获取项目，索引无效: " << index;
        return T();
    }
    
    void setPlayMode(PlayMode mode) {
        currentMode = mode;
        qDebug() << "设置播放模式: " << mode;
    }
    
    PlayMode getPlayMode() const {
        return currentMode;
    }
    
    // 切换到下一个播放模式
    PlayMode switchToNextPlayMode() {
        switch (currentMode) {
            case orderMode:
                currentMode = randomMode;
                qDebug() << "切换播放模式: 随机播放";
                break;
            case randomMode:
                currentMode = loopMode;
                qDebug() << "切换播放模式: 循环播放";
                break;
            case loopMode:
                currentMode = singleMode;
                qDebug() << "切换播放模式: 单曲循环";
                break;
            case singleMode:
                currentMode = orderMode;
                qDebug() << "切换播放模式: 顺序播放";
                break;
        }
        return currentMode;
    }
    
    int getNextIndex() const {
        if (items.isEmpty()) {
            return -1;
        }
        
        int nextIndex = currentIndex;
        
        switch (currentMode) {
            case orderMode:
                nextIndex = (currentIndex < items.size() - 1) ? (currentIndex + 1) : currentIndex;
                break;
            case randomMode:
                if (items.size() > 1) {
                    do {
                        nextIndex = QRandomGenerator::global()->bounded(items.size());
                    } while (nextIndex == currentIndex);
                }
                break;
            case loopMode:
                nextIndex = (currentIndex + 1) % items.size();
                break;
            case singleMode:
                nextIndex = currentIndex;
                break;
        }
        
        return nextIndex;
    }
    
    /**
     * @brief 处理播放结束事件
     * @return 根据播放模式确定的下一个项目
     */
    T handleEndOfMedia() {
        if (items.isEmpty()) {
            return T();
        }
        
        switch (currentMode) {
            case orderMode:
                // 顺序播放：如果不是最后一个，则播放下一个
                if (currentIndex < items.size() - 1) {
                    return getNextItem();
                }
                break;
                
            case randomMode:
            case loopMode:
                // 随机/循环播放：播放下一个
                return getNextItem();
                
            case singleMode:
                // 单曲循环：重新播放当前歌曲
                return getCurrentItem();
        }
        
        return getCurrentItem();
    }
};

#endif // PLAYLISTMANAGER_H
