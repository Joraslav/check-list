---
description: "Use when writing, reviewing or refactoring C++ code. Covers naming conventions, include order, RAII, error handling, NOLINT policy and documentation style."
applyTo: "**/*.{cpp,hpp}"
---

# Соглашения об именовании

| Сущность | Стиль | Пример |
|---|---|---|
| Классы, структуры | `PascalCase` | `TaskDB`, `ScopedStatement` |
| Методы, функции | `PascalCase` | `AddUser()`, `GetAllTasks()` |
| Переменные, параметры | `snake_case` | `user_id`, `db_path` |
| Приватные поля класса | `snake_case_` (с `_` в конце) | `db_`, `statement_manager_` |
| Параметры конструктора (при совпадении с полем) | `p_` префикс | `p_user_id`, `p_text` |
| `constexpr`/`static` константы | `kPascalCase` | `kUserId`, `kMinLevel` |

Публичные поля структур (`struct`) — `snake_case` **без** завершающего `_`. Суффикс `_` только у приватных полей классов (`class`).
| `static constexpr` строки (в классах) | `UPPER_SNAKE_CASE` | `INSERT_USER`, `SELECT_USER` |
| Пространства имён | `snake_case` | `database`, `bot`, `logging` |
| Файлы заголовков | `PascalCase.hpp` | `TaskDB.hpp`, `ScopedStatement.hpp` |
| Файлы реализации | `PascalCase.cpp` | `TaskDB.cpp` |

**Примечание:** Публичные поля структур (`struct`) — `snake_case` **без** завершающего `_`. Суффикс `_` только у приватных полей классов.

# Целевой стандарт

- **Использовать C++26** (или новее) как базовый стандарт проекта.
- По возможности **предпочитать новые возможности C++26** вместо старых идиом, если это улучшает читаемость, безопасность или производительность.

# Рекомендуемые возможности C++26

| Категория | Фича | Когда использовать |
|---------|------|-------------------|
| **Язык** | `std::constexpr` в большем количестве контекстов (включая аллокации, `std::vector`, `std::string`) | Везде, где возможно |
| **Язык** | `consteval` + `constexpr` функции | Для сложных вычислений на этапе компиляции |
| **Язык** | `std::meta` (reflection) | Для библиотек, ORM, сериализации, dependency injection |
| **Язык** | Contracts (`pre`, `post`, `assert`) | На всех публичных и критических функциях |
| **Библиотека** | `std::print` / `std::println` | Вместо `std::cout` |
| **Библиотека** | `std::format` с новыми возможностями | Везде вместо ручной конкатенации |
| **Библиотека** | `std::expected<T, E>` | Вместо пары `(T, error_code)` или исключений в performance-critical коде |
| **Библиотека** | `std::span` + `std::mdspan` | Вместо сырых указателей и C-массивов |
| **Библиотека** | `std::generator<T>` | Для ленивых последовательностей |
| **Библиотека** | `std::stacktrace` | В обработке ошибок и логировании |
| **Библиотека** | `std::hazard_pointer`, `std::rcu` | При написании lock-free структур |
| **Библиотека** | `std::linalg` | При работе с линейной алгеброй |
| **Язык** | Deducing `this` + explicit object parameter | Для CRTP-альтернатив и chainable API |

**Правило:** Если в C++26 есть более современный и безопасный способ сделать то же самое — используй его. Старые идиомы оставляем только для совместимости или когда новая фича ещё не поддерживается компилятором.

# Стиль кода

- Заголовочная защита: `#pragma once`
- Порядок `#include`:
  1. В `.cpp` файле после комментариев первой директивой `#include` подключается парный заголовок этого же модуля
     (только имя файла, без подпапок)
  2. Стандартная библиотека (группа)
  3. Сторонние библиотеки (группа)
  4. Проектные заголовки (группа)
- Примеры для парного заголовка в `.cpp`:

```cpp
// Корректно
#include "Aggregator.hpp"

// Некорректно
#include "analysis/Aggregator.hpp"
```
- В `.cpp` файлах разрешено использовать короткие `using domain::Type;` для улучшения читаемости.
- `constexpr` / `consteval` / `const` — максимально широко
- `[[nodiscard]]`, `noexcept`, `override`, `final` — обязательно где применимо
- `std::move` только при передаче владения
- Конструкторы: `explicit` (включая multi-argument)
- Копирование и присваивание: явно `= delete`, если не нужны
- Анонимные пространства имён для внутренних символов

# Управление ресурсами (RAII)

- **Запрещены** владеющие сырые указатели.
- Использовать `std::unique_ptr`, `std::shared_ptr`, или собственные RAII-объекты.
- Сырые указатели/ссылки — только как non-owning observers.

# Обработка ошибок

- Предпочтение: `std::expected<T, E>` (C++23/26) → `std::optional` → исключения.
- Исключения (если используются) должны наследоваться от `std::exception`.
- Для вызовов внешних библиотек — отдельные `catch` блоки.
- Документировать:
  - `@throw`
  - `@exception_safety` (basic/strong/nothrow)
  - `@returns` для `std::expected`

# NOLINT-комментарии

Перед каждым `// NOLINT(...)` обязателен комментарий с обоснованием:
```cpp
// std::getenv is safe here: single-threaded startup, before any threads are spawned
const char* value = std::getenv(variable_name);  // NOLINT(concurrency-mt-unsafe)
```

# Документирование кода

Doxygen-стиль для публичного API:
```cpp
/**
 * @brief Краткое описание
 * @param user_id ID пользователя
 * @return `std::optional<int64_t>` — ID задачи или `std::nullopt`
 * @pre `user_id > 0`
 * @throw `std::runtime_error` если ...
 * @warning Операция необратима!
 */
```
