#!/bin/bash

set -e  # Завершить выполнение при ошибке

echo "Начало сборки проекта..."

# Создание директории сборки
BUILD_DIR="build"
echo "Создание директории сборки: $BUILD_DIR"
mkdir -p $BUILD_DIR

# Переход в директорию сборки
cd $BUILD_DIR

# Конфигурация проекта через CMake с нужными флагами
echo "Конфигурация проекта через CMake..."
cmake .. -DBUILD_TEST=ON -DDEV_MODE=OFF

# Сборка проекта
echo "Сборка проекта..."
cmake --build .

echo "Сборка завершена успешно!"