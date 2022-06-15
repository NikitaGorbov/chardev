# Char device driver

## Пояснение
Результат вычисления a + b записывается в буфер kernel_buf. Параметры a и b имеют тип int. В стандарте C не указывается максимально возможный размер int (стандарт требует, чтобы int покрывал диапазон -32767 - 32767). Таким образом, размер буфера, способного хранить a + b зависит от архитектуры. Для выделения памяти под буфер используется константа INT_MAX из limits.h, что позволяет гарантировать выделение достаточного количество памяти для данной архитектуры. Размер буфера считается как число цифр в INT_MAX + 1 байт на символ '-' для отрицательных чисел + 1 байт на '\0'.