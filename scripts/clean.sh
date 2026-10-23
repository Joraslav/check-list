#!/bin/bash

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# Функция для обработки ошибок
error_exit() {
    echo -e "${RED}Ошибка: $1${NC}"
    exit 1
}

echo -e "${YELLOW}Очистка рабочей области...${NC}"

if [ -d "build" ]; then
    rm -rf build || error_exit "Не удалось удалить папку build"
fi

if [ -f "CMakeUserPresets.json" ]; then
    rm CMakeUserPresets.json || error_exit "Не удалось удалить файл CMakeUserPresets.json"
fi

if [ -f "CMakeCache.txt" ]; then
    rm CMakeCache.txt || error_exit "Не удалось удалить файл CMakeCache.txt"
fi

if [ -L "compile_commands.json" ]; then
    rm compile_commands.json || error_exit "Не удалось удалить симлинк compile_commands.json"
fi

if [ -d "out" ]; then
    find out -mindepth 1 -delete || error_exit "Не удалось очистить папку out"
fi

if [ -d ".venv" ]; then
    rm -rf .venv || error_exit "Не удалось удалить папку .venv"
fi

echo -e "${YELLOW}Очистка кэшей инструментов...${NC}"

# Удаление Python кэшей
find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null
find . -type d -name ".pytest_cache" -exec rm -rf {} + 2>/dev/null
find . -type d -name "*.egg-info" -exec rm -rf {} + 2>/dev/null

# Удаление VS Code кэшей (если они в репозитории)
if [ -d ".vscode" ]; then
    find .vscode -type f -name "*.code-settings" -delete 2>/dev/null
fi

# Удаление временных файлов компилятора
find . -type f \( -name "*.o" -o -name "*.a" -o -name "*.so" \) -delete 2>/dev/null

echo -e "${GREEN}Очистка завершена!${NC}"