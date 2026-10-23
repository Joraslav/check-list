#!/bin/bash

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

LIST_ONLY=false
APPLY_FIXES=false
BUILD_DIR="build"
BUILD_TYPE=""
JOBS=""
TIDY_BUILD_DIR=""

print_usage() {
	echo "Использование: $0 [--list] [--fix] [--build-dir <path>] [--build-type <Debug|Release|...>] [--jobs <N>]"
}

cleanup() {
	if [ -n "$TIDY_BUILD_DIR" ] && [ -d "$TIDY_BUILD_DIR" ]; then
		rm -rf "$TIDY_BUILD_DIR"
	fi
}

prepare_tidy_compilation_database() {
	local source_db="$BUILD_DIR/compile_commands.json"

	TIDY_BUILD_DIR="$(mktemp -d)"
	trap cleanup EXIT

	sed -E \
		-e 's/(^| )-fmodules-ts($| )/ /g' \
		-e 's@(^| )-fmodule-mapper=[^ ]+($| )@ @g' \
		-e 's@(^| )-fdeps-format=[^ ]+($| )@ @g' \
		"$source_db" > "$TIDY_BUILD_DIR/compile_commands.json"
}

while [ $# -gt 0 ]; do
	case "$1" in
		--list)
			LIST_ONLY=true
			shift
			;;
		--fix)
			APPLY_FIXES=true
			shift
			;;
		--build-dir)
			if [ -z "$2" ]; then
				echo -e "${RED}✗ Не указан путь для --build-dir.${NC}"
				print_usage
				exit 1
			fi
			BUILD_DIR="$2"
			shift 2
			;;
		--build-type)
			if [ -z "$2" ]; then
				echo -e "${RED}✗ Не указан тип сборки для --build-type.${NC}"
				print_usage
				exit 1
			fi
			BUILD_TYPE="$2"
			shift 2
			;;
		--jobs)
			if [ -z "$2" ]; then
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

if ! command -v clang-tidy &> /dev/null; then
	echo -e "${RED}✗ clang-tidy не найден. Установите его и повторите попытку.${NC}"
	exit 1
fi

if ! command -v cmake &> /dev/null; then
	echo -e "${RED}✗ cmake не найден. Установите его и повторите попытку.${NC}"
	exit 1
fi

echo -e "${YELLOW}Поиск C++ файлов для проверки...${NC}"

cpp_files=()
while IFS= read -r -d '' source_file; do
	cpp_files+=("${source_file#./}")
done < <(
	find . \
		\( -type d \( -name .git -o -name .cache -o -name build -o -name CMakeFiles \
			-o -name cmake-build-\* -o -name _build -o -name node_modules -o -name vendor \) -prune \) -o \
		\( -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.c" \) -print0 \)
)

if [ ${#cpp_files[@]} -gt 0 ]; then
	mapfile -t cpp_files < <(printf '%s\n' "${cpp_files[@]}" | sort -u)
fi

if [ ${#cpp_files[@]} -eq 0 ]; then
	echo -e "${YELLOW}C++ исходники не найдены.${NC}"
	exit 0
fi

if [ "$LIST_ONLY" = true ]; then
	echo -e "${YELLOW}Найдено ${#cpp_files[@]} файлов:${NC}"
	printf '%s\n' "${cpp_files[@]}"
	exit 0
fi

if [ ! -f "$BUILD_DIR/compile_commands.json" ] && [ -z "$BUILD_TYPE" ]; then
	for candidate in Debug Release; do
		if [ -f "$BUILD_DIR/$candidate/compile_commands.json" ]; then
			BUILD_DIR="$BUILD_DIR/$candidate"
			break
		fi
	done
fi

if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
	if [ -z "$BUILD_TYPE" ]; then
		echo -e "${RED}✗ compile_commands.json не найден в '$BUILD_DIR'.${NC}"
		echo -e "${RED}  Укажите --build-type для автоконфигурации или передайте уже сконфигурированный --build-dir.${NC}"
		exit 1
	fi

	echo -e "${YELLOW}compile_commands.json не найден в '$BUILD_DIR'. Запускаю конфигурацию CMake (CMAKE_BUILD_TYPE=$BUILD_TYPE)...${NC}"
	cmake -S . -B "$BUILD_DIR" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
fi

if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
	echo -e "${RED}✗ Не удалось найти или создать $BUILD_DIR/compile_commands.json${NC}"
	exit 1
fi

if [ -z "$JOBS" ]; then
	total_cores=""
	if command -v nproc &> /dev/null; then
		total_cores="$(nproc)"
	elif command -v getconf &> /dev/null; then
		total_cores="$(getconf _NPROCESSORS_ONLN 2> /dev/null || true)"
	fi

	if [[ "$total_cores" =~ ^[1-9][0-9]*$ ]]; then
		JOBS="$(((total_cores + 1) / 2))"
	fi
fi

if ! [[ "$JOBS" =~ ^[1-9][0-9]*$ ]]; then
	JOBS=1
fi

prepare_tidy_compilation_database

echo -e "${YELLOW}Запускаю clang-tidy для ${#cpp_files[@]} файлов...${NC}"

clang_tidy_args=("-p" "$TIDY_BUILD_DIR" "-quiet")

if [ "$APPLY_FIXES" = true ]; then
	clang_tidy_args+=("--fix" "--fix-errors" "--format-style=file")
	echo -e "${YELLOW}Включены автоисправления (--fix).${NC}"
fi

set +e
if [ "$JOBS" -gt 1 ] && command -v xargs &> /dev/null; then
	echo -e "${YELLOW}Параллельный запуск clang-tidy: ${JOBS} поток(ов).${NC}"
	printf '%s\0' "${cpp_files[@]}" | xargs -0 -n1 -P "$JOBS" bash -c 'file="$1"; shift; clang-tidy "$file" "$@"' _ "${clang_tidy_args[@]}"
	clang_tidy_exit_code=$?
else
	if [ "$JOBS" -gt 1 ]; then
		echo -e "${YELLOW}xargs не найден, выполняю clang-tidy последовательно.${NC}"
	fi
	clang-tidy "${cpp_files[@]}" "${clang_tidy_args[@]}"
	clang_tidy_exit_code=$?
fi
set -e

if [ $clang_tidy_exit_code -eq 0 ]; then
	echo -e "${GREEN}✓ Проверка clang-tidy завершена успешно!${NC}"
else
	echo -e "${RED}✗ clang-tidy обнаружил замечания. Код возврата: $clang_tidy_exit_code${NC}"
	exit $clang_tidy_exit_code
fi