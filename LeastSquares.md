# Описание
Алгоритм основанный на методе наименьшиx квадратов, который по обучающей выборке находит многочлен стпени от 1 до 11, который приближает неизвестную функцию с наименьшей квадратичной ошибкой.

X_train, Y_train - обучающая выборка, где f(X_train[i]) = Y_train[i], а f - неизвестная функция.

X_test, Y_test - тестовая выборка.

Так же алгоритм находит среднеквадратическую ошибку на обучающих и тестовых данных.