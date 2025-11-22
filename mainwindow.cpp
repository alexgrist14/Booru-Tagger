#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QScrollArea>
#include <QFileInfo>
#include <QTextEdit>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Основной layout: горизонтальный (теги слева, медиа справа)
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Левая часть: список тегов
    QVBoxLayout *leftLayout = new QVBoxLayout();
    tagList = new QListWidget();
    leftLayout->addWidget(new QLabel("Теги:"));
    leftLayout->addWidget(tagList);

    // Поле и кнопка для добавления тега
    QHBoxLayout *tagAddLayout = new QHBoxLayout();
    tagInput = new QLineEdit();
    addTagButton = new QPushButton("Добавить тег");
    connect(addTagButton, &QPushButton::clicked, this, &MainWindow::addTag);
    tagAddLayout->addWidget(tagInput);
    tagAddLayout->addWidget(addTagButton);
    leftLayout->addLayout(tagAddLayout);

    mainLayout->addLayout(leftLayout);

    // Правая часть: просмотр медиа и список файлов
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // Кнопка выбора папки
    QPushButton *selectFolderButton = new QPushButton("Выбрать папку");
    connect(selectFolderButton, &QPushButton::clicked, this, &MainWindow::selectFolder);
    rightLayout->addWidget(selectFolderButton);

    // Поле поиска
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Поиск по тегам...");
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::searchTags);
    rightLayout->addWidget(searchInput);

    // Список файлов
    mediaList = new QListWidget();
    connect(mediaList, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        loadMedia(currentDir.absoluteFilePath(item->text()));
    });
    rightLayout->addWidget(mediaList);

    // Контейнер для медиа (изображения и видео)
    QWidget *mediaContainer = new QWidget();
    QVBoxLayout *mediaContainerLayout = new QVBoxLayout(mediaContainer);
    mediaContainerLayout->setContentsMargins(0, 0, 0, 0);
    
    // Виджет для медиа
    mediaLabel = new QLabel();
    mediaLabel->setAlignment(Qt::AlignCenter);
    mediaLabel->setScaledContents(true);
    mediaLabel->setMinimumSize(800, 600);  // Увеличенный размер

    videoWidget = new QVideoWidget();
    videoWidget->setMinimumSize(800, 600);  // Увеличенный размер
    mediaPlayer = new QMediaPlayer();
    mediaPlayer->setVideoOutput(videoWidget);
    videoWidget->hide();  // Скрываем по умолчанию
    
    mediaContainerLayout->addWidget(mediaLabel);
    mediaContainerLayout->addWidget(videoWidget);

    QScrollArea *mediaScroll = new QScrollArea();
    mediaScroll->setWidget(mediaContainer);  // Контейнер с обоими виджетами
    mediaScroll->setWidgetResizable(true);
    rightLayout->addWidget(mediaScroll, 2);  // Больше места для медиа

    // Виджет для отображения информации о файле
    QGroupBox *infoGroup = new QGroupBox("Информация о файле");
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLabel = new QLabel();
    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setWordWrap(true);
    infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    infoLabel->setText("Выберите файл для просмотра информации");
    infoLayout->addWidget(infoLabel);
    infoGroup->setLayout(infoLayout);
    rightLayout->addWidget(infoGroup, 1);  // Меньше места для информации

    mainLayout->addLayout(rightLayout);

    // Инициализация
    completer = new QCompleter();
    searchInput->setCompleter(completer);
    QStringListModel *model = new QStringListModel();
    completer->setModel(model);

    setWindowTitle("Local Booru Viewer");
    resize(1200, 800);  // Увеличенный размер окна
}

MainWindow::~MainWindow() {}

void MainWindow::selectFolder() {
    QString dirPath = QFileDialog::getExistingDirectory(this, "Выберите папку с изображениями");
    if (dirPath.isEmpty()) return;

    currentDir = QDir(dirPath);
    tagsFilePath = currentDir.filePath("tags.json");
    loadTags();  // Загружаем теги

    mediaList->clear();
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.gif" << "*.mp4" << "*.avi";  // Добавьте другие форматы
    currentDir.setNameFilters(filters);
    QStringList files = currentDir.entryList();
    mediaList->addItems(files);
}

void MainWindow::loadMedia(const QString &filePath) {
    currentFile = QFileInfo(filePath).fileName();
    QFileInfo fileInfo(filePath);

    // Определяем тип: изображение или видео
    QString suffix = fileInfo.suffix().toLower();
    QString infoText = "<b>Расширение:</b> " + suffix.toUpper() + "<br>";

    if (suffix == "mp4" || suffix == "avi" || suffix == "mov" || suffix == "mkv" || suffix == "webm") {  // Видео
        mediaLabel->hide();
        videoWidget->show();
        
        // Отключаем предыдущие соединения, чтобы избежать множественных вызовов
        disconnect(mediaPlayer, &QMediaPlayer::metaDataChanged, nullptr, nullptr);
        
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
        mediaPlayer->play();

        // Получаем информацию о видео
        // Разрешение и кодировка будут доступны после загрузки метаданных
        connect(mediaPlayer, &QMediaPlayer::metaDataChanged, this, [this, suffix]() {
            QSize resolution = mediaPlayer->metaData().value(QMediaMetaData::Resolution).toSize();
            QString codec = mediaPlayer->metaData().value(QMediaMetaData::VideoCodec).toString();
            
            QString videoInfo = "<b>Расширение:</b> " + suffix.toUpper() + "<br>";
            if (!resolution.isNull()) {
                videoInfo += "<b>Разрешение:</b> " + QString::number(resolution.width()) + 
                            " x " + QString::number(resolution.height()) + "<br>";
            } else {
                videoInfo += "<b>Разрешение:</b> Неизвестно<br>";
            }
            if (!codec.isEmpty()) {
                videoInfo += "<b>Кодировка:</b> " + codec + "<br>";
            } else {
                videoInfo += "<b>Кодировка:</b> Неизвестно<br>";
            }
            infoLabel->setText(videoInfo);
        });

        // Временная информация до загрузки метаданных
        infoText += "<b>Разрешение:</b> Загрузка...<br>";
        infoText += "<b>Кодировка:</b> Загрузка...<br>";
        infoLabel->setText(infoText);
    } else {  // Изображение
        videoWidget->hide();
        mediaLabel->show();
        QPixmap pixmap(filePath);
        if (pixmap.isNull()) {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение.");
            return;
        }
        
        // Получаем информацию об изображении
        QSize resolution = pixmap.size();
        infoText += "<b>Разрешение:</b> " + QString::number(resolution.width()) + 
                   " x " + QString::number(resolution.height()) + "<br>";
        
        // Для изображений кодировка = формат файла
        infoText += "<b>Кодировка:</b> " + suffix.toUpper() + "<br>";
        
        mediaLabel->setPixmap(pixmap.scaled(mediaLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        infoLabel->setText(infoText);
    }

    // Отображаем теги
    tagList->clear();
    if (fileTags.contains(currentFile)) {
        tagList->addItems(fileTags[currentFile]);
    }
}

void MainWindow::addTag() {
    if (currentFile.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите файл.");
        return;
    }

    QString tag = tagInput->text().trimmed();
    if (tag.isEmpty()) return;

    if (!fileTags[currentFile].contains(tag)) {
        fileTags[currentFile].append(tag);
        tagList->addItem(tag);

        // Обновляем все теги для автодополнения
        if (!allTags.contains(tag)) {
            allTags.append(tag);
            static_cast<QStringListModel*>(completer->model())->setStringList(allTags);
        }

        saveTags();
    }
    tagInput->clear();
}

void MainWindow::searchTags(const QString &text) {
    if (text.isEmpty()) {
        mediaList->clear();
        mediaList->addItems(currentDir.entryList());
        return;
    }

    mediaList->clear();
    QStringList files = currentDir.entryList();
    for (const QString &file : files) {
        if (fileTags.contains(file) && fileTags[file].contains(text, Qt::CaseInsensitive)) {
            mediaList->addItem(file);
        }
    }
}

void MainWindow::loadTags() {
    fileTags.clear();
    allTags.clear();

    QFile file(tagsFilePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    for (const QString &key : obj.keys()) {
        QJsonArray arr = obj[key].toArray();
        QStringList tags;
        for (const QJsonValue &val : arr) {
            tags.append(val.toString());
        }
        fileTags[key] = tags;
        allTags << tags;  // Добавляем в все теги
    }
    allTags.removeDuplicates();
    static_cast<QStringListModel*>(completer->model())->setStringList(allTags);
}

void MainWindow::saveTags() {
    QJsonObject obj;
    for (const QString &key : fileTags.keys()) {
        QJsonArray arr;
        for (const QString &tag : fileTags[key]) {
            arr.append(tag);
        }
        obj[key] = arr;
    }

    QFile file(tagsFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson());
    }
}
