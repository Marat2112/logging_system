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
    
# Команды для демонстрации:

bash
# Показать сборку
make clean && make

# Показать работу
./logger_app demo.log DEBUG

# Показать содержимое лога
tail -f demo.log
