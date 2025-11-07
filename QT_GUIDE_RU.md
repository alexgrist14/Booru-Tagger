# Подробное руководство по Qt и C++ для JavaScript-разработчика

## Что делает это приложение?

Это **Local Booru Viewer** — приложение для просмотра изображений и видео с системой тегирования. Аналогично сайтам вроде Danbooru или Gelbooru, но работает локально на вашем компьютере.

**Основной функционал:**

1. Выбор папки с медиа-файлами (изображения и видео)
2. Просмотр изображений и воспроизведение видео
3. Добавление тегов к файлам
4. Поиск файлов по тегам с автодополнением
5. Сохранение тегов в JSON-файл (`tags.json`)

---

## Архитектура приложения

### Структура файлов (аналог в JavaScript)

```
main.cpp          → index.js (точка входа)
mainwindow.h      → MainWindow.tsx (типы и интерфейс)
mainwindow.cpp    → MainWindow.tsx (реализация компонента)
mainwindow.ui     → MainWindow.ui (XML-описание UI, как JSX)
Localbooru1.pro   → package.json (зависимости и настройки сборки)
```

---

## Основные концепции C++ для JavaScript-разработчика

### 1. Указатели (Pointers) — `*`

**В JavaScript:** объекты передаются по ссылке автоматически

```javascript
const button = document.createElement("button");
// button - это ссылка на объект
```

**В C++:** нужно явно работать с указателями для динамических объектов

```cpp
QPushButton *button = new QPushButton();
// button - это указатель (адрес в памяти)
// *button - разыменование (получить сам объект)
// &variable - получить адрес переменной
```

**Зачем нужны указатели в Qt:**

- Qt использует **parent-child** систему для управления памятью
- Когда родитель удаляется, все дети автоматически удаляются
- Это аналог React-компонентов, где дочерние элементы удаляются при удалении родителя

```cpp
// Правильно: создаем с родителем
QPushButton *button = new QPushButton(this);  // this = MainWindow
// Когда MainWindow удалится, button тоже удалится автоматически

// Неправильно: без родителя (утечка памяти!)
QPushButton *button = new QPushButton();  // Нужно вручную delete button
```

**Аналогия с React:**

```jsx
// React
function Component() {
  return <button>Click</button>;  // React управляет жизнью компонента
}

// Qt
MainWindow::MainWindow() {
  QPushButton *button = new QPushButton(this);  // Qt управляет жизнью виджета
}
```

---

### 2. Классы и наследование

**В JavaScript (ES6+):**

```javascript
class MainWindow extends React.Component {
  constructor() {
    super();
    this.state = { ... };
  }
}
```

**В C++:**

```cpp
class MainWindow : public QMainWindow {
  // : public QMainWindow = extends QMainWindow
  Q_OBJECT  // Макрос Qt для системы сигналов/слотов

public:
  MainWindow(QWidget *parent = nullptr);  // Конструктор
  ~MainWindow();  // Деструктор (аналог componentWillUnmount)

private:
  // Приватные поля (аналог this.state или useRef)
  QLabel *mediaLabel;
};
```

**Ключевые отличия:**

- `public:` / `private:` / `protected:` — модификаторы доступа (в JS нет, всё публичное)
- `Q_OBJECT` — макрос Qt для мета-объектной системы (нужен для сигналов/слотов)
- Деструктор `~MainWindow()` вызывается при удалении объекта

---

### 3. Сигналы и слоты (Signals & Slots) — аналог событий

**В JavaScript:**

```javascript
button.addEventListener("click", () => {
  console.log("Clicked!");
});

// React
<button onClick={handleClick}>Click</button>;
```

**В Qt:**

```cpp
connect(addTagButton, &QPushButton::clicked, this, &MainWindow::addTag);
//     источник      сигнал              получатель  слот (метод)
```

**Что происходит:**

- `QPushButton::clicked` — сигнал (событие)
- `MainWindow::addTag` — слот (обработчик)
- `connect()` связывает их вместе

**Аналогия:**

```javascript
// JavaScript
button.on('click', this.handleClick.bind(this));

// Qt
connect(button, &QPushButton::clicked, this, &MainWindow::handleClick);
```

---

### 4. Строки: QString vs std::string vs const char\*

**В JavaScript:** один тип `string`

```javascript
const text = "Hello";
```

**В C++:** несколько вариантов

```cpp
QString text = "Hello";           // Qt-строка (рекомендуется в Qt)
std::string text = "Hello";       // Стандартная C++ строка
const char* text = "Hello";       // C-строка (массив char)
```

**Почему QString:**

- Поддержка Unicode (кириллица, эмодзи)
- Много удобных методов: `trimmed()`, `isEmpty()`, `contains()`
- Интеграция с Qt-классами

**Методы QString (аналоги JS):**

```cpp
QString s = "  Hello  ";
s.trimmed()      // "Hello" (как .trim())
s.isEmpty()      // false (как .length === 0)
s.contains("el") // true (как .includes())
s.toLower()      // "hello" (как .toLowerCase())
```

---

### 5. Контейнеры (Collections) — аналог массивов/объектов

**В JavaScript:**

```javascript
const tags = ["tag1", "tag2"]; // Массив
const fileTags = { "file.jpg": ["tag1"] }; // Объект
```

**В Qt:**

```cpp
QStringList tags = {"tag1", "tag2"};                    // Список строк
QMap<QString, QStringList> fileTags;                     // Словарь
fileTags["file.jpg"] = QStringList{"tag1", "tag2"};     // Добавление
```

**Основные контейнеры Qt:**

- `QStringList` — список строк (аналог `string[]`)
- `QList<T>` — динамический массив (аналог `T[]`)
- `QMap<K, V>` — словарь (аналог `Map<K, V>` или `{K: V}`)
- `QVector<T>` — вектор (аналог `T[]`)

**Итерация (аналог forEach):**

```cpp
// C++11 range-based for (как for...of)
for (const QString &file : files) {
  // обработка
}

// Qt-стиль (старый способ)
QStringListIterator it(files);
while (it.hasNext()) {
  QString file = it.next();
}
```

---

### 6. Layouts (Макеты) — аналог CSS Flexbox/Grid

**В JavaScript/React:**

```jsx
<div style={{ display: "flex", flexDirection: "row" }}>
  <div>Left</div>
  <div>Right</div>
</div>
```

**В Qt:**

```cpp
QHBoxLayout *mainLayout = new QHBoxLayout();  // Горизонтальный (row)
mainLayout->addWidget(leftWidget);
mainLayout->addWidget(rightWidget);

QVBoxLayout *leftLayout = new QVBoxLayout();  // Вертикальный (column)
leftLayout->addWidget(widget1);
leftLayout->addWidget(widget2);
```

**Типы Layouts:**

- `QHBoxLayout` — горизонтальный (flex-direction: row)
- `QVBoxLayout` — вертикальный (flex-direction: column)
- `QGridLayout` — сетка (display: grid)
- `QFormLayout` — форма (label + input)

---

## Детальный разбор кода

### main.cpp — точка входа

```cpp
int main(int argc, char *argv[])
```

- `argc` — количество аргументов командной строки
- `argv` — массив строк (аргументы)
- В JS: `process.argv` в Node.js

```cpp
QApplication a(argc, argv);
```

- Создает объект приложения Qt
- Аналог: инициализация React-приложения
- Должен быть создан до любых виджетов

```cpp
MainWindow w;
w.show();
```

- Создает главное окно
- `show()` делает окно видимым
- В JS: `ReactDOM.render(<App />, root)`

```cpp
return a.exec();
```

- Запускает главный цикл событий (event loop)
- Аналог: `ReactDOM.render()` блокирует до закрытия приложения
- Обрабатывает клики, нажатия клавиш и т.д.

---

### mainwindow.h — определение класса

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
```

- **Header guards** — защита от повторного включения
- В JS: модули автоматически кешируются, здесь нужно вручную

```cpp
class MainWindow : public QMainWindow {
  Q_OBJECT
```

- `Q_OBJECT` — макрос для MOC (Meta-Object Compiler)
- Позволяет использовать сигналы/слоты
- Qt автоматически генерирует `moc_mainwindow.cpp`

```cpp
private slots:
  void addTag();
```

- `slots:` — методы, которые можно подключить к сигналам
- В JS: обычные методы класса

```cpp
private:
  QLabel *mediaLabel;
```

- Приватные поля класса
- В JS: `this.mediaLabel` или `useRef()`

---

### mainwindow.cpp — реализация

#### Конструктор

```cpp
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
```

- `::` — оператор области видимости (scope resolution)
- `MainWindow::MainWindow` = конструктор класса MainWindow
- `: QMainWindow(parent)` — вызов конструктора родителя (super())

```cpp
QWidget *centralWidget = new QWidget(this);
setCentralWidget(centralWidget);
```

- Создаем центральный виджет
- `this` передается как родитель (автоматическое управление памятью)

```cpp
QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
```

- Создаем горизонтальный layout
- `centralWidget` — родитель (layout привязывается к виджету)

#### Lambda-функции (C++11)

```cpp
connect(mediaList, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
  loadMedia(currentDir.absoluteFilePath(item->text()));
});
```

**Разбор:**

- `[this]` — capture list (захватывает `this` из внешней области)
- `(QListWidgetItem *item)` — параметры
- `{ ... }` — тело функции

**Аналогия с JS:**

```javascript
// JavaScript
mediaList.addEventListener('click', (item) => {
  this.loadMedia(item.text);
});

// C++
connect(mediaList, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
  loadMedia(...);
});
```

#### Работа с файлами

```cpp
QFile file(tagsFilePath);
if (!file.open(QIODevice::ReadOnly)) return;
```

**Аналогия с JS:**

```javascript
// JavaScript (Node.js)
const fs = require('fs');
const data = fs.readFileSync('tags.json', 'utf8');

// Qt
QFile file("tags.json");
file.open(QIODevice::ReadOnly);
QByteArray data = file.readAll();
```

#### JSON в Qt

```cpp
QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
QJsonObject obj = doc.object();
```

**Аналогия:**

```javascript
// JavaScript
const obj = JSON.parse(data);

// Qt
QJsonDocument doc = QJsonDocument::fromJson(data);
QJsonObject obj = doc.object();
```

---

## Управление памятью

### В JavaScript (автоматическая сборка мусора)

```javascript
const button = document.createElement("button");
// Когда button больше не используется, GC удалит его автоматически
```

### В C++ (ручное управление или умные указатели)

**Qt-подход (parent-child):**

```cpp
QPushButton *button = new QPushButton(this);  // this = родитель
// Когда this удалится, button удалится автоматически
```

**Проблема без родителя:**

```cpp
QPushButton *button = new QPushButton();  // Утечка памяти!
// Нужно: delete button; (но лучше использовать parent)
```

**Современный C++ (умные указатели):**

```cpp
std::unique_ptr<QPushButton> button = std::make_unique<QPushButton>();
// Автоматически удалится при выходе из области видимости
```

**Рекомендация:** В Qt используйте parent-child систему, это проще и безопаснее.

---

## Как добавлять новые модули

### Шаг 1: Создать новый класс

**Создайте файлы:**

- `tagmanager.h` — заголовочный файл
- `tagmanager.cpp` — реализация

**tagmanager.h:**

```cpp
#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <QObject>
#include <QMap>
#include <QStringList>

class TagManager : public QObject {
    Q_OBJECT

public:
    explicit TagManager(QObject *parent = nullptr);

    void addTag(const QString &file, const QString &tag);
    QStringList getTags(const QString &file) const;
    QStringList getAllTags() const;

signals:
    void tagAdded(const QString &file, const QString &tag);

private:
    QMap<QString, QStringList> fileTags;
    QStringList allTags;
};

#endif // TAGMANAGER_H
```

**tagmanager.cpp:**

```cpp
#include "tagmanager.h"

TagManager::TagManager(QObject *parent) : QObject(parent) {
}

void TagManager::addTag(const QString &file, const QString &tag) {
    if (!fileTags[file].contains(tag)) {
        fileTags[file].append(tag);
        if (!allTags.contains(tag)) {
            allTags.append(tag);
        }
        emit tagAdded(file, tag);  // Отправляем сигнал
    }
}

QStringList TagManager::getTags(const QString &file) const {
    return fileTags.value(file);
}

QStringList TagManager::getAllTags() const {
    return allTags;
}
```

### Шаг 2: Добавить в проект

**Localbooru1.pro:**

```pro
HEADERS += \
    mainwindow.h \
    tagmanager.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tagmanager.cpp
```

### Шаг 3: Использовать в MainWindow

**mainwindow.h:**

```cpp
#include "tagmanager.h"

private:
    TagManager *tagManager;  // Добавить поле
```

**mainwindow.cpp:**

```cpp
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    tagManager = new TagManager(this);  // Создать экземпляр

    // Подключить сигнал
    connect(tagManager, &TagManager::tagAdded,
            this, &MainWindow::onTagAdded);

    // Использовать
    tagManager->addTag("file.jpg", "nature");
}

void MainWindow::onTagAdded(const QString &file, const QString &tag) {
    // Обработка события
}
```

---

## Полезные советы

### 1. Отладка

```cpp
qDebug() << "Debug message:" << variable;
// Аналог: console.log()
```

### 2. Проверка на nullptr

```cpp
if (widget != nullptr) {
    widget->doSomething();
}
// Или короче:
if (widget) {
    widget->doSomething();
}
```

### 3. Строковые литералы

```cpp
QString text = QString("Hello %1").arg(name);
// Аналог: `Hello ${name}`
```

### 4. Итерация по QMap

```cpp
for (auto it = fileTags.begin(); it != fileTags.end(); ++it) {
    QString key = it.key();
    QStringList value = it.value();
}
// Или C++11:
for (auto it = fileTags.constBegin(); it != fileTags.constEnd(); ++it) {
    // ...
}
```

---

## Сравнительная таблица: JavaScript vs C++/Qt

| JavaScript           | C++/Qt                           | Примечание               |
| -------------------- | -------------------------------- | ------------------------ |
| `const x = {}`       | `QObject *x = new QObject(this)` | Создание объекта         |
| `x.method()`         | `x->method()`                    | Вызов метода (указатель) |
| `x.method()`         | `x.method()`                     | Вызов метода (объект)    |
| `x.property`         | `x->property`                    | Доступ к свойству        |
| `array.forEach()`    | `for (const auto &item : array)` | Итерация                 |
| `JSON.parse()`       | `QJsonDocument::fromJson()`      | Парсинг JSON             |
| `fs.readFileSync()`  | `QFile::readAll()`               | Чтение файла             |
| `addEventListener()` | `connect()`                      | Подписка на события      |
| `class X extends Y`  | `class X : public Y`             | Наследование             |
| `this`               | `this`                           | Текущий объект           |
| `super()`            | `: ParentClass(args)`            | Вызов родителя           |

---

## Следующие шаги

1. **Изучите Qt Documentation:** https://doc.qt.io/
2. **Практикуйтесь:** добавляйте новые функции (фильтры, сортировка, экспорт)
3. **Изучите Qt Designer:** визуальный редактор UI (аналог Figma для Qt)
4. **Изучите Model/View:** для больших списков данных
5. **Изучите QML:** декларативный язык для UI (аналог React)

---

## Частые ошибки новичков

1. **Забыли `Q_OBJECT`** — сигналы/слоты не работают
2. **Не передали `parent`** — утечка памяти
3. **Использовали `delete` для виджетов с parent** — двойное удаление
4. **Забыли `#include`** — ошибки компиляции
5. **Использовали `std::string` вместо `QString`** — проблемы с Unicode

---

Удачи в изучении Qt! 🚀
