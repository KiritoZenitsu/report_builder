# Report Builder System

Система для автоматической генерации отчетов на C++. Поддерживает гибкий конвейер обработки данных: загрузка из различных источников (CSV, JSON, In-memory), фильтрация, сортировка, агрегация, форматирование (HTML, Plain Text, Markdown) и экспорт (файл, консоль, email). Проект реализован с использованием паттернов проектирования (Abstract Factory, Builder, Strategy) и соответствует Yandex C++ Style Guide.

## Требования к системе

*   **Компилятор C++:** с поддержкой стандарта C++17 или выше (GCC 7+, Clang 5+, MSVC 2017+).
*   **Система сборки:** CMake 3.14 или выше.
*   **Для тестирования:** Google Test (gtest) — при сборке с опцией `-DBUILD_TESTS=ON`.

## Быстрый старт (сборка через CMake)

Следующие команды соберут проект в ОС Linux (Ubuntu/Debian), что является одним из требований.

### 1. Клонирование репозитория (если нужно)
```
git clone <your-repo-url>
cd report_builder
```

### 2. Создание директории для сборки
```
mkdir build && cd build
```

### 3. Конфигурация проекта с помощью CMake
```
cmake ..
```

### 4. Сборка проекта
```
make
```

### 5. Запуск основной программы
```
./src/report_builder
```

### Сборка с тестами: Для включения и сборки модульных тестов на gtest используйте конфигурацию:
```
cmake -DBUILD_TESTS=ON ..
make
```
# Тестирование
Проект включает модульные тесты, использующие фреймворк Google Test.
Запуск тестов после сборки:
```
# В директории `build/` после сборки с `-DBUILD_TESTS=ON`
ctest --output-on-failure

# Или можно запустить исполняемые файлы тестов напрямую
./test/unit_tests
./test/integration_tests
```

# Проверка и форматирование стиля кода
Проект следует Yandex C++ Style Guide. Для автоматической проверки используется clang-format.
Проверка стиля (без изменений файлов):
```
clang-format -n --style=file --dry-run -Werror include/report_builder/*.h src/*.cpp test/*.cpp
```
Автоматическое исправление стиля:
```
clang-format -i --style=file include/report_builder/*.h src/*.cpp test/*.cpp
```
