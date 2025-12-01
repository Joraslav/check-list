#!/bin/bash

set -e  # Завершить выполнение при ошибке

echo "Создание папки build"
mkdir -p build

echo "Начало сборки проекта..."

# Установка зависимостей Conan
echo "Установка зависимостей Conan..."
conan profile detect --force
conan install . --build=missing -pr default -s compiler.cppstd=23

# Конфигурация проекта через CMake с нужными флагами и Conan presets
echo "Конфигурация проекта через CMake..."
cmake --preset conan-release -DBUILD_TEST=ON -DDEV_MODE=OFF

# Сборка проекта
echo "Сборка проекта..."
cmake --build --preset conan-release

echo "Сборка завершена успешно!"