# 🪵 Advanced Logging System - C++

## Профессиональная система логирования с поддержкой многопоточности и динамической библиотекой


## 🚀 Особенности

- **Динамическая библиотека** логирования (`liblogger.so`)
- **4 уровня важности**: DEBUG, INFO, WARNING, ERROR
- **Многопоточная обработка** сообщений
- Потокобезопасная реализация
- Гибкая настройка уровня логирования
- Детальная информация в записях: время, уровень, сообщение
- Простая интеграция через Makefile

## 📦 Установка и сборка

```bash
# Клонирование репозитория
git clone https://github.com/yourusername/logging-system.git
cd logging-system

# Сборка проекта
make

# Запуск приложения
LD_LIBRARY_PATH=. ./logger_app log.txt INFO
```

## 🖥 Использование

```text
Logger started. Type 'exit' to quit.
Enter message: Application initialized
Enter log level (optional): 
Enter message: Low memory warning
Enter log level (optional): WARNING
Enter message: Database connection failed
Enter log level (optional): ERROR
Enter message: exit
```

Пример файла журнала (`log.txt`):
```text
2023-10-15 14:30:22 [INFO] Application initialized
2023-10-15 14:30:25 [WARNING] Low memory warning
2023-10-15 14:30:29 [ERROR] Database connection failed
```

## 🧩 Структура проекта

| Файл                | Описание                          |
|---------------------|-----------------------------------|
| `include/logger.h`  | Заголовочный файл библиотеки      |
| `src/logger.cpp`    | Реализация библиотеки логирования |
| `src/app.cpp`       | Многопоточное консольное приложение |
| `Makefile`          | Система сборки проекта            |

## 🔧 Технические детали

- **Стандарт языка**: C++17
- **Компилятор**: GCC (g++)
- **ОС**: Linux (Ubuntu/Debian)
- **Зависимости**: Только STL

## 📄 Лицензия

Проект распространяется под лицензией MIT. Подробнее см. в [LICENSE](LICENSE).

# Архитектура:

```mermaid
graph TD
    A[Консольное приложение] -->|использует| B[liblogger.so]
    B --> C[(Файл журнала)]
    A --> D[Потокобезопасная очередь]
    subgraph Потоки
        D --> E[Рабочий поток]
    end
    E --> B
```

# Ключевые моменты:

Динамическая библиотека: .so файл, загружаемый в runtime

Потокобезопасность: мьютексы + условные переменные

RAII: автоматическое управление ресурсами (файлами, потоками)

Обработка ошибок: исключения + проверки состояния

C++17: std::optional, лямбды, structured bindings

# Структура библиотеки

```mermaid
classDiagram
    class Logger {
        -logFile: ofstream
        -currentLevel: LogLevel
        -logMutex: mutex
        +Logger(filename: string, level: LogLevel)
        +~Logger()
        +setLogLevel(level: LogLevel)
        +log(level: LogLevel, message: string)
        -levelToString(level: LogLevel) string
        -getCurrentTime() string
    }
    
    class ThreadSafeQueue {
        -queue: queue<pair<string, LogLevel>>
        -mutex: mutex
        -cond: condition_variable
        -shutdownFlag: bool
        +push(message: string, level: LogLevel)
        +pop() optional<pair<string, LogLevel>>
        +shutdown()
    }
    
    LogLevel <|-- Logger
    ThreadSafeQueue "1" *-- "1" App
    Logger "1" *-- "1" App
```

# Процесс работы:

```mermaid
sequenceDiagram
    participant User as Пользователь
    participant App as Приложение
    participant Queue as Очередь сообщений
    participant Worker as Рабочий поток
    participant Logger as Библиотека логирования
    participant File as Файл журнала
    
    User->>App: Ввод сообщения и уровня
    activate App
    App->>Queue: Добавление в очередь
    deactivate App
    
    loop Рабочий цикл
        Worker->>Queue: Запрос сообщения
        activate Queue
        Queue-->>Worker: Возвращает сообщение
        deactivate Queue
        
        Worker->>Logger: Вызов log()
        activate Logger
        Logger->>File: Запись в файл
        File-->>Logger: Подтверждение
        Logger-->>Worker: Успешная запись
        deactivate Logger
    end
    
    User->>App: Команда "exit"
    App->>Worker: Сигнал завершения
    Worker-->>App: Подтверждение остановки
```
    
