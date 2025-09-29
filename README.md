# Task Scheduler

## Описание

**Task Scheduler** — это библиотека на C++, предназначенная для выполнения связанных между собой задач.
Она позволяет описывать вычисления в виде ориентированного графа:

* **узлы** — это задачи (функции, лямбды, методы классов),
* **рёбра** — это зависимости между результатом одной задачи и входными параметрами другой.

Благодаря этому можно эффективно описывать и исполнять вычислительные цепочки, избегая повторных вычислений и обеспечивая гибкость работы с асинхронными результатами.

## Возможности

* Добавление задач в виде функций, лямбд или методов классов.
* Автоматическое управление зависимостями между задачами.
* Ленивые вычисления: результат считается только тогда, когда он реально нужен.
* Возможность получать «будущий результат» (аналог `future`) и передавать его в другие задачи.
* Полное выполнение всех заданий одним вызовом `executeAll()`.

## Пример использования

Пример ниже демонстрирует вычисление корней квадратного уравнения и последующую обработку результата с помощью `Task Scheduler`:

```cpp
struct AddNumber {
  float add(float a) const {
    return a + number;
  }

  float number;
};

float a = 1;
float b = -2;
float c = 0;
AddNumber add{ .number = 3 };

TTaskScheduler scheduler;

auto id1 = scheduler.add([](float a, float c) { return -4 * a * c; }, a, c);
auto id2 = scheduler.add([](float b, float v) { return b * b + v; }, b, scheduler.getFutureResult<float>(id1));
auto id3 = scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
auto id4 = scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
auto id5 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));
auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));
auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

scheduler.executeAll();

std::cout << "x1 = " << scheduler.getResult<float>(id5) << std::endl;
std::cout << "x2 = " << scheduler.getResult<float>(id6) << std::endl;
std::cout << "x3 = " << scheduler.getResult<float>(id7) << std::endl;
```

## Интерфейс `TTaskScheduler`

* `add` — добавляет задачу и возвращает идентификатор.
* `getFutureResult<T>` — возвращает объект-заглушку для результата, который можно использовать в других задачах.
* `getResult<T>` — возвращает итоговый результат задачи (при необходимости вычисляет её).
* `executeAll` — выполняет все зарегистрированные задачи.

## Применение

* Оптимизация сложных вычислений.
* Построение систем зависимостей.
* Разработка библиотек для ленивых и асинхронных вычислений.

## Цель проекта

Цель разработки — предоставить удобный и расширяемый инструмент для описания и выполнения зависимых вычислений на C++ в виде графа задач.
