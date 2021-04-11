## Реализация интерпретатора командной строки (shell)

Модули:
- io.h - Ввод/вывод
- list.h - Построение списка слов - Лексический этап
- tree.h - Построение дерева команд - Синтаксический этап
- exec.h - Выполнение дерева команд

Дерево строится по L-графу из файла L-graph.pdf

Цели make:
- all - сборка
- run - запуск
- leak - запуск валгринда для проверок утечек памяти
- clean - удаление объектных файлов
- distclean - clean + удаление бинарных файлов

Реализованы:
- Конвейеры **|** - cat | sort | wc
- Перенаправления **> >> <** - cat < inp > out или cat >> append
- Запуск в фоновом режиме **&** - ls & 
- Последовательное выполенение команд **;** - ls ; pwd
- Выполнен команд по успешности завершения предыдущей: 
  - Или **||** - cat nosuchname || pwd
  - И  **&&** - cat nosuchname && pwd
- Сабшелл **()** - (ls; pwd; who) | cat -n
- Команда cd для перемещения между директориями
- Подстановка перемнных окружения $HOME, $SHELL, $USER, $EUID

Режим отладки переключается с помощью константы DEBUG в main
В этом режиме при работе выводится структура списка и дерева

Некоторые тесты для проверки работы:
- echo a b c > f; cat f & ls
- echo a b c > f; echo >> f; cat<f
- (ls; pwd; who) | cat -n
- ((((ls) | cat -n ) | cat -n) | sort -r)
- ((((ls) | cat -n ) | cat -n) | sort -r) &
- (pwd|(cat -n|(cat |(cat) | sort -r ) | cat -n) |cat)
- (pwd|(cat -n|(cat |(cat))))
- cat f; date; pwd > zz
- date; (ls -l |cat -n)>f & pwd
- ls && who && pwd
- ls || who || pwd
- ( ls || who || pwd) |cat | sort -r
- ( ls && who || pwd) |cat -n | sort
- pwd && sleep 5 &
- ls || who | cat
- who |cat || ls
- cat nosuchname || pwd
- cat nosuchname && pwd
- (ls& ) | sleep 10; pwd


Автор: Железов Максим, 209 группа
