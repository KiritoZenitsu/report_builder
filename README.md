# report_builder
# Система построителя отчетов на C++

## Описание
Система для автоматической генерации различных типов отчетов из структурированных данных. 
Реализована с использованием паттернов проектирования и современных возможностей C++17.

## Реализованные паттерны
- **Абстрактная фабрика** (`ReportFactory`, `FinanceReportFactory`, `SalesReportFactory`)
- **Строитель** (`ReportBuilder`)
- **Стратегия** (`ExportStrategy` и его реализации)
- **Фабричный метод** (виртуальные методы создания)

## Структура проекта
```
report-builder/
├── data_types.h // Базовые типы данных (DataValue, DataTable)
├── interfaces.h // Интерфейсы всех компонентов
├── data_providers.h // Источники данных (CSV, JSON, In-memory)
├── data_processors.h // Обработчики (фильтр, сортировка, агрегация)
├── formatters.h // Форматировщики (HTML, Plain Text, Markdown)
├── export_strategies.h // Стратегии экспорта (файл, консоль, email)
├── report_builder.h // Строитель отчетов (паттерн Builder)
├── report_factories.h // Фабрики отчетов (паттерн Abstract Factory)
├── main.cpp // Основная программа с примерами
├── test_simple.cpp // Базовые тесты
├── final_test.cpp // Полные тесты системы
└── CMakeLists.txt // Файл для сборки через CMake
```

## Быстрый старт

### Сборка с g++:
# Основная программа
```
g++ -std=c++17 -o report_builder main.cpp
```

# Тесты
```
g++ -std=c++17 -o test_simple test_simple.cpp
g++ -std=c++17 -o final_test final_test.cpp
```

### Сборка с CMake:
```
mkdir build
cd build
cmake ..
cmake --build .
```

### Запуск:
```
./report_builder     # Linux
report_builder.exe   # Windows
```

## Тестирование
Система включает два тестовых файла:
test_simple.cpp - проверка основных компонентов
final_test.cpp - полное тестирование конвейера

Запуск тестов:
```
g++ -std=c++17 -o test_simple test_simple.cpp && ./test_simple
```
