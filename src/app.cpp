// Подключаем заголовочный файл нашей библиотеки логирования
#include "logger.h"

// Стандартные заголовки C++
#include <iostream>       // Ввод/вывод в консоль
#include <queue>          // Очередь для хранения сообщений
#include <thread>         // Работа с потоками
#include <mutex>          // Синхронизация потоков
#include <condition_variable> // Условные переменные
#include <atomic>         // Атомарные операции
#include <cctype>         // Функции для работы с символами
#include <algorithm>      // Алгоритмы STL
#include <memory>         // Умные указатели
#include <optional>       // Опциональные значения (C++17)

// Потокобезопасная очередь сообщений
class ThreadSafeQueue {
public:
    // Добавление сообщения в очередь
    void push(const std::string& message, LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex); // Блокировка мьютекса
        queue.push({message, level});            // Добавление пары (сообщение, уровень)
        cond.notify_one();                       // Уведомление одного ожидающего потока
    }

    // Извлечение сообщения из очереди
    std::optional<std::pair<std::string, LogLevel>> pop() {
        std::unique_lock<std::mutex> lock(mutex); // Уникальная блокировка
        
        // Ожидание сообщения или сигнала завершения
        cond.wait(lock, [this]{ 
            return !queue.empty() || shutdownFlag; 
        });
        
        // Проверка флага завершения
        if (shutdownFlag && queue.empty()) {
            return std::nullopt; // Возвращаем пустое значение
        }
        
        // Извлекаем сообщение из очереди
        auto item = queue.front();
        queue.pop();
        return item;
    }

    // Сигнал завершения работы очереди
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex);
        shutdownFlag = true;         // Устанавливаем флаг завершения
        cond.notify_all();           // Уведомляем все ожидающие потоки
    }

private:
    std::queue<std::pair<std::string, LogLevel>> queue; // Очередь сообщений
    std::mutex mutex;                // Мьютекс для синхронизации
    std::condition_variable cond;    // Условная переменная
    bool shutdownFlag = false;       // Флаг завершения работы
};

// Преобразование строки в значение перечисления LogLevel
LogLevel parseLogLevel(const std::string& levelStr) {
    std::string upper; // Строка в верхнем регистре
    
    // Преобразование в верхний регистр
    for (char c : levelStr) {
        upper += std::toupper(c);
    }
    
    // Определение уровня логирования
    if (upper == "DEBUG") return LogLevel::Debug;
    if (upper == "INFO") return LogLevel::Info;
    if (upper == "WARNING") return LogLevel::Warning;
    if (upper == "ERROR") return LogLevel::Error;
    
    // Если уровень не распознан - исключение
    throw std::invalid_argument("Unknown log level: " + levelStr);
}

// Функция рабочего потока для обработки сообщений
void logWorker(ThreadSafeQueue& queue, Logger& logger) {
    // Бесконечный цикл обработки
    while (auto item = queue.pop()) {
        try {
            // Запись сообщения в лог
            logger.log(item->second, item->first);
        } catch (const std::exception& e) {
            // Обработка ошибок записи
            std::cerr << "Logging failed: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // Проверка количества аргументов командной строки
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <logfile> <default_log_level>" << std::endl;
        return 1;
    }

    // Получение имени файла журнала из аргументов
    std::string logfile = argv[1];
    LogLevel defaultLevel;
    
    try {
        // Парсинг уровня логирования из аргументов
        defaultLevel = parseLogLevel(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    try {
        // Инициализация логгера (может бросить исключение)
        Logger logger(logfile, defaultLevel);
        
        // Создание потокобезопасной очереди
        ThreadSafeQueue logQueue;
        
        // Запуск рабочего потока
        std::thread workerThread([&]{
            logWorker(logQueue, logger);
        });

        // Информационное сообщение для пользователя
        std::cout << "Logger started. Type 'exit' to quit." << std::endl;
        
        // Основной цикл ввода сообщений
        while (true) {
            std::string input;
            std::cout << "Enter message: ";
            
            // Чтение ввода пользователя
            if (!std::getline(std::cin, input) || input == "exit") {
                break; // Выход по команде "exit" или ошибке ввода
            }

            std::string levelInput;
            std::cout << "Enter log level (optional): ";
            std::getline(std::cin, levelInput);

            // Уровень логирования (по умолчанию или пользовательский)
            LogLevel level = defaultLevel;
            if (!levelInput.empty()) {
                try {
                    level = parseLogLevel(levelInput);
                } catch (...) {
                    std::cout << "Invalid level. Using default: " 
                              << argv[2] << std::endl;
                }
            }

            // Добавление сообщения в очередь
            logQueue.push(input, level);
        }

        // Завершение работы
        logQueue.shutdown(); // Сигнал остановки
        
        // Ожидание завершения рабочего потока
        if (workerThread.joinable()) {
            workerThread.join();
        }

    } catch (const std::exception& e) {
        // Обработка исключений инициализации
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}