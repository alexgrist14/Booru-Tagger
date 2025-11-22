#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCompleter>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaMetaData>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QMap>
#include <QStringListModel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectFolder();  // Выбор папки
    void loadMedia(const QString &filePath);  // Загрузка изображения/видео
    void addTag();  // Добавление тега
    void searchTags(const QString &text);  // Поиск с автодополнением

private:
    void loadTags();  // Загрузка тегов из JSON
    void saveTags();  // Сохранение тегов в JSON

    QDir currentDir;  // Текущая папка
    QString currentFile;  // Текущий файл
    QLabel *mediaLabel;  // Для изображений
    QVideoWidget *videoWidget;  // Для видео
    QMediaPlayer *mediaPlayer;  // Плеер для видео
    QLabel *infoLabel;  // Для отображения информации о файле
    QListWidget *tagList;  // Список тегов слева
    QLineEdit *tagInput;  // Поле для добавления тега
    QPushButton *addTagButton;  // Кнопка добавления тега
    QLineEdit *searchInput;  // Поле поиска
    QCompleter *completer;  // Автодополнение
    QListWidget *mediaList;  // Список файлов в папке

    QMap<QString, QStringList> fileTags;  // Хранение тегов: файл -> список тегов
    QStringList allTags;  // Все уникальные теги для автодополнения
    QString tagsFilePath;  // Путь к tags.json
};

#endif // MAINWINDOW_H
