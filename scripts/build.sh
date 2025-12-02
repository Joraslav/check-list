#!/bin/bash

set -e  # Завершить выполнение при ошибке

echo "Создание папки build"
mkdir -p build

echo "Начало сборки проекта..."

echo "Конфигурация проекта через CMake..."
cmake --preset conan-release -DBUILD_TEST=ON -DDEV_MODE=OFF

# Сборка проекта
echo "Сборка проекта..."
cmake --build --preset conan-release

echo "Сборка завершена успешно!"