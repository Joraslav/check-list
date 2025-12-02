#!/bin/bash

# Функция для обработки ошибок
error_exit() {
    echo "Ошибка: $1"
    exit 1
}

echo "Очистка рабочей области..."

if [ -d "build" ]; then
    rm -rf build || error_exit "Не удалось удалить папку build"
fi

if [ -f "CMakeUserPresets.json" ]; then
    rm CMakeUserPresets.json || error_exit "Не удалось удалить файл CMakeUserPresets.json"
fi

echo "Очистка завершена!"