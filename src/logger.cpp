#include "logger.h"

// Конструктор
Logger::Logger(const std::string& filename, LogLevel defaultLevel) 
    : currentLevel(defaultLevel) { // Инициализация уровня
    // Открытие файла в режиме добавления (append)
    logFile.open(filename, std::ios::app);
    
    // Проверка успешности открытия файла
    if (!logFile.is_open()) {
        throw std::runtime_error("Cannot open log file: " + filename);
    }
}

// Деструктор
Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();  // Закрытие файла при уничтожении объекта
    }
}

// Установка нового уровня логирования
void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex); // Блокировка мьютекса
    currentLevel = level; // Обновление уровня
}

// Запись сообщения в лог
void Logger::log(LogLevel level, const std::string& message) {
    // Фильтрация по уровню важности
    if (level < currentLevel) return;

    // Блокировка для потокобезопасности
    std::lock_guard<std::mutex> lock(logMutex);
    
    // Проверка состояния файла
    if (!logFile) {
        throw std::runtime_error("Log file is in bad state");
    }
    
    // Форматированная запись: время + уровень + сообщение
    logFile << getCurrentTime() 
            << " [" << levelToString(level) << "] " 
            << message << std::endl;
}

// Преобразование уровня в строку
std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        default:                return "UNKNOWN"; // На случай расширения
    }
}

// Получение текущего времени
std::string Logger::getCurrentTime() {
    auto now = std::time(nullptr); // Текущее время
    auto tm = *std::localtime(&now); // Конвертация в локальное время
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S"); // Форматирование
    return oss.str();
}
