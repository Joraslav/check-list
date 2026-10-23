#!/bin/bash

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

LIST_ONLY=false
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

if [ $# -gt 1 ]; then
	echo -e "${RED}✗ Неверное количество аргументов. Использование: $0 [--list]${NC}"
	exit 1
fi

if [ $# -eq 1 ]; then
	if [ "$1" = "--list" ]; then
		LIST_ONLY=true
	else
		echo -e "${RED}✗ Неизвестный аргумент: $1. Использование: $0 [--list]${NC}"
		exit 1
	fi
fi

if ! command -v clang-format &> /dev/null; then
	echo -e "${RED}✗ clang-format не найден. Установите его и повторите попытку.${NC}"
	exit 1
fi

echo -e "${YELLOW}Поиск C++ файлов для форматирования...${NC}"

cpp_files=()
while IFS= read -r -d '' cpp_file; do
	cpp_files+=("$cpp_file")
done < <(
	cd "$PROJECT_ROOT"
	find . \
		\( -type d \( -name .git -o -name .cache -o -name build \
			-o -name CMakeFiles -o -name cmake-build-\* -o -name _build \
			-o -name out -o -name vendor -o -name node_modules \) -prune \) -o \
		\( -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" \
			-o -name "*.cxx" -o -name "*.h" -o -name "*.hh" \
			-o -name "*.hpp" -o -name "*.hxx" \) -print0 \)
)

if [ ${#cpp_files[@]} -eq 0 ]; then
	echo -e "${YELLOW}C++ файлы не найдены.${NC}"
	exit 0
fi

if [ "$LIST_ONLY" = true ]; then
	echo -e "${YELLOW}Найдено ${#cpp_files[@]} файлов:${NC}"
	printf '%s\n' "${cpp_files[@]#./}"
	exit 0
fi

echo -e "${YELLOW}Найдено ${#cpp_files[@]} файлов. Запускаю clang-format...${NC}"

cd "$PROJECT_ROOT"
clang-format -i "${cpp_files[@]}"

echo -e "${GREEN}✓ Форматирование завершено успешно!${NC}"
