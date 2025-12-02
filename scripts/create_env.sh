#!/bin/bash

set -e 

if [ $# -eq 0 ]; then
    echo "Ошибка: Не указан тип сборки"
    echo "Использование: $0 [Debug|Release]"
    exit 1
fi

BUILD_TYPE=$1

if [ "$BUILD_TYPE" != "Debug" ] && [ "$BUILD_TYPE" != "Release" ]; then
    echo "Ошибка: Неверный тип сборки '$BUILD_TYPE'. Допустимые значения: Debug, Release"
    exit 1
fi

echo "Настройка окружения проекта для сборки типа $BUILD_TYPE..."

if [ ! -f "~/.conan2/profiles/default" ]; then
    echo "Создание дефолтного профиля Conan..."
    conan profile detect --force
fi

echo "Установка зависимостей Conan..."
conan install . --build=missing -pr profiles/$BUILD_TYPE

echo "Окружение успешно настроено!"
