# Image Processor
Программа для обработки изображений с несколькими функциями.

# Доступные операции:
## 1. RGB-фильтр (--rgbfilter)
Изменяет значение одной из цветовых компонент для всего изображения:
* --component_name - выбирает компоненту: red, green или blue
* --component_value - устанавливает значение (0-255)

## 2. Рисование квадрата (--square)
Рисует квадрат с заданными параметрами:

* --left_up - координаты левого верхнего угла (формат: x.y)
* --side_size - размер стороны (>0)
* --thickness - толщина линий (>0)
* --color - цвет линий (формат: rrr.ggg.bbb, пример: 255.0.0)
* --fill - заливка (есть флаг = залить, нет флага = не заливать)
* --fill_color - цвет заливки (работает с --fill)

## 3. Обмен частями (--exchange)
Делит прямоугольную область на 4 части и меняет их местами:

* --left_up - левый верхний угол области (формат: x.y)
* --right_down - правый нижний угол области (формат: x.y)
* --exchange_type - тип обмена:
  * clockwise - по часовой стрелке
  * counterclockwise - против часовой стрелки
  * diagonals - по диагонали

## 4. Замена частого цвета (--freq_color)
Находит самый часто встречаемый цвет в изображении и заменяет его на другой:

* --color - новый цвет (формат: rrr.ggg.bbb)
# Использование
```
# Пример 1: Изменить красную компоненту
./cw --input input.bmp --output output.bmp --rgbfilter --component_name red --component_value 200

# Пример 2: Нарисовать квадрат
./cw --input input.bmp --output output.bmp --square --left_up 50.50 --side_size 100 --thickness 2 --color 255.0.0 --fill --fill_color 0.255.0

# Пример 3: Обмен частями
./cw --input input.bmp --output output.bmp --exchange --left_up 0.0 --right_down 200.200 --exchange_type clockwise

# Пример 4: Заменить частый цвет
./cw --input input.bmp --output output.bmp --freq_color --color 128.128.128
```
