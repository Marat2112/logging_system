#pragma once  // Гарантирует однократное включение файла
#include <fstream>   // Для работы с файлами
#include <string>    // Для строковых операций
#include <mutex>     // Для синхронизации потоков
#include <ctime>     // Для работы со временем
#include <iomanip>   // Для форматирования вывода
#include <stdexcept> // Для исключений

// Уровни логирования (перечисление)
enum class LogLevel {
    Debug,   // Отладочные сообщения (самый низкий приоритет)
    Info,    // Информационные сообщения
    Warning, // Предупреждения
    Error    // Ошибки (высший приоритет)
};

class Logger {
public:
    // Конструктор: принимает имя файла и уровень логирования по умолчанию
    Logger(const std::string& filename, LogLevel defaultLevel);
    
    // Деструктор: закрывает файл
    ~Logger();
    
    // Установка нового уровня логирования
    void setLogLevel(LogLevel level);
    
    // Основной метод записи в лог
    void log(LogLevel level, const std::string& message);

private:
    std::ofstream logFile;   // Файловый поток
    LogLevel currentLevel;   // Текущий уровень фильтрации
    std::mutex logMutex;     // Мьютекс для потокобезопасности
    
    // Вспомогательные методы
    std::string levelToString(LogLevel level); // Конвертация enum в строку
    std::string getCurrentTime();              // Получение форматированного времени
};
