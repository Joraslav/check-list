#!/bin/bash

set -euo pipefail

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

BUILD_DIR=""
BUILD_TYPE="Debug"
JOBS=""

print_usage() {
	echo "Использование: $0 [--build-type <Debug|Release>] [--build-dir <path>] [--jobs <N>]"
}

while [ $# -gt 0 ]; do
	case "$1" in
		--build-dir)
			if [ $# -lt 2 ] || [ -z "$2" ]; then
				echo -e "${RED}✗ Не указан путь для --build-dir.${NC}"
				print_usage
				exit 1
			fi
			BUILD_DIR="$2"
			shift 2
			;;
		--build-type)
			if [ $# -lt 2 ] || [ -z "$2" ]; then
				echo -e "${RED}✗ Не указан тип сборки для --build-type.${NC}"
				print_usage
				exit 1
			fi
			BUILD_TYPE="$2"
			shift 2
			;;
		--jobs)
			if [ $# -lt 2 ] || [ -z "$2" ]; then
				echo -e "${RED}✗ Не указано значение для --jobs.${NC}"
				print_usage
				exit 1
			fi
			if ! [[ "$2" =~ ^[1-9][0-9]*$ ]]; then
				echo -e "${RED}✗ --jobs должен быть положительным целым числом.${NC}"
				print_usage
				exit 1
			fi
			JOBS="$2"
			shift 2
			;;
		--help)
			print_usage
			exit 0
			;;
		*)
			echo -e "${RED}✗ Неизвестный аргумент: $1${NC}"
			print_usage
			exit 1
			;;
	esac
done

if [ "$BUILD_TYPE" != "Debug" ] && [ "$BUILD_TYPE" != "Release" ]; then
	echo -e "${RED}✗ Неверный тип сборки '$BUILD_TYPE'. Допустимые значения: Debug, Release.${NC}"
	print_usage
	exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
	echo -e "${RED}✗ cmake не найден. Установите его и повторите попытку.${NC}"
	exit 1
fi

if ! command -v ctest >/dev/null 2>&1; then
	echo -e "${RED}✗ ctest не найден. Установите его и повторите попытку.${NC}"
	exit 1
fi

if [ -z "$JOBS" ]; then
	if command -v nproc >/dev/null 2>&1; then
		JOBS="$(nproc)"
	else
		JOBS=1
	fi
fi

if [ -z "$BUILD_DIR" ]; then
	BUILD_DIR="build/$BUILD_TYPE"
fi

BUILD_TYPE_LOWER="$(printf '%s' "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"
PRESET_NAME="conan-$BUILD_TYPE_LOWER"

if [ ! -d "$BUILD_DIR" ] || [ ! -f "$BUILD_DIR/CTestTestfile.cmake" ]; then
	echo -e "${YELLOW}Сборочная директория '$BUILD_DIR' не готова. Запускаю конфигурацию через preset ${PRESET_NAME}.${NC}"
	cmake --preset "$PRESET_NAME"
fi

echo -e "${YELLOW}Собираю тестовые цели...${NC}"
cmake --build --preset "$PRESET_NAME" --parallel "$JOBS"

echo -e "${YELLOW}Запускаю GoogleTest, зарегистрированные в CTest...${NC}"
ctest --test-dir "$BUILD_DIR" --build-config "$BUILD_TYPE" --output-on-failure --parallel "$JOBS"

echo -e "${GREEN}✓ Все тесты завершены.${NC}"
