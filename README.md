# RB-дерево

**Лабораторная работа 1**

---

**Тема:** Реализация сбалансированного дерева поиска и профилирование производительности

**Условие:** Разработать библиотеку и программу-словарь для хранения пар «регистронезависимое слово (до 256 символов) — число от $0$ до $2^{64} - 1$» с бинарной сериализацией (`Save`/`Load`), провести анализ скорости и памяти.

**Вариант:** Красно-чёрное дерево (RB-Tree).

---

## Сборка и запуск

### Требования

- Компилятор C++23
- CMake 3.16+
- Python 3, `gprof`, `valgrind`, `time` (для бенчмарков и профилирования)
- `pdflatex` (TeX Live с кириллицей) и `make` (для сборки отчёта)

### 1. Debug (по умолчанию)

```bash
cmake -B build
cmake --build build
./build/dict
```

### 2. Release (для замеров)

```bash
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
./build-release/dict < input.txt
```

### 3. Сборка под `gprof`

```bash
cmake -B build-prof -DCMAKE_BUILD_TYPE=Release -DENABLE_GPROF=ON
cmake --build build-prof
```

---

## Тесты и профилирование

### Базовая проверка

```bash
./build/dict < tests/smoke.in
```

### Генерация нагрузочных данных

```bash
python3 benchmarks/gen_bench.py --out benchmarks/data --ops 500000
```

### Полный прогон профайлеров (`time`, `gprof`, `valgrind`)

```bash
./benchmarks/run_profile.sh v1
```

Логи замеров сохраняются в `report/raw/<версия>/`.

---

## Сборка отчёта

Двойной прогон `make pdf` нужен для корректного обновления ссылок и таблиц `longtable`:

```bash
cd report
make pdf && make pdf
```

Очистка временных файлов после сборки:

```bash
rm -f report.{aux,log,out}
```

---

## Структура проекта

```text
alg-lab-rbtree/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── rb_tree.hpp             # Интерфейс красно-чёрного дерева
│   └── rb_tree_io.hpp          # Бинарная сериализация
├── src/
│   ├── main.cpp                # Обработка команд словаря
│   ├── rb_tree.cpp             # Реализация RB-Tree
│   └── rb_tree_io.cpp          # Реализация Save/Load
├── tests/                      # Тесты корректности (smoke.in)
├── benchmarks/
│   ├── gen_bench.py            # Генератор тестовых данных
│   └── run_profile.sh          # Скрипт сборки и профилирования
└── report/
    ├── Makefile                # Сборка PDF
    ├── report.tex              # Основной файл отчёта
    ├── report.pdf              # Собранный отчёт
    ├── src/                    # Разделы отчёта (.tex)
    ├── styles/
    │   └── log-style.sty       # Стилевой файл
    └── raw/                    # Логи замеров по версиям (v1, v2)
```

---

Продвинутые алгоритмы и дискретный анализ

Семестр 3. Лабораторная работа 1
