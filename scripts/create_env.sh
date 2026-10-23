#!/bin/bash

set -euo pipefail

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

usage() {
    echo "Использование: $0 [Debug|Release] [BuildProfile]"
    echo "Пример: $0 Debug"
    echo "Пример: $0 Debug Release"
    echo "На Fedora автоматически используются профили Fedora-Debug/Fedora-Release, если они существуют."
}

if [ $# -lt 1 ] || [ $# -gt 2 ]; then
    echo -e "${RED}Ошибка: Неверное количество аргументов${NC}"
    usage
    exit 1
fi

if ! command -v conan >/dev/null 2>&1; then
    echo -e "${RED}Ошибка: Conan не найден в PATH${NC}"
    echo "Установите Conan и повторите запуск."
    exit 1
fi

BUILD_TYPE=$1
BUILD_PROFILE=${2:-Release}
PROFILE_PREFIX=""

if [ "$BUILD_TYPE" != "Debug" ] && [ "$BUILD_TYPE" != "Release" ]; then
    echo -e "${YELLOW}Ошибка: Неверный тип сборки '$BUILD_TYPE'. Допустимые значения: Debug, Release${NC}"
    usage
    exit 1
fi

if [ "$BUILD_PROFILE" != "Debug" ] && [ "$BUILD_PROFILE" != "Release" ]; then
    echo -e "${YELLOW}Ошибка: Неверный build profile '$BUILD_PROFILE'. Допустимые значения: Debug, Release${NC}"
    usage
    exit 1
fi

if [ -f /etc/os-release ]; then
    # На Fedora используем отдельные профили, чтобы не ломать CI/Ubuntu-профили.
    if grep -qi '^ID=fedora$' /etc/os-release; then
        PROFILE_PREFIX="Fedora-"
    fi
fi

HOST_PROFILE_PATH="$PROJECT_ROOT/profiles/${PROFILE_PREFIX}$BUILD_TYPE"
BUILD_PROFILE_PATH="$PROJECT_ROOT/profiles/${PROFILE_PREFIX}$BUILD_PROFILE"

if [ ! -f "$HOST_PROFILE_PATH" ] || [ ! -f "$BUILD_PROFILE_PATH" ]; then
    HOST_PROFILE_PATH="$PROJECT_ROOT/profiles/$BUILD_TYPE"
    BUILD_PROFILE_PATH="$PROJECT_ROOT/profiles/$BUILD_PROFILE"
fi

if [ ! -f "$HOST_PROFILE_PATH" ]; then
    echo -e "${RED}Ошибка: Не найден host profile '$HOST_PROFILE_PATH'${NC}"
    exit 1
fi

if [ ! -f "$BUILD_PROFILE_PATH" ]; then
    echo -e "${RED}Ошибка: Не найден build profile '$BUILD_PROFILE_PATH'${NC}"
    exit 1
fi

if ! command -v perl >/dev/null 2>&1; then
    echo -e "${RED}Ошибка: Perl не найден в PATH. Он нужен для сборки openssl через Conan.${NC}"
    if command -v dnf >/dev/null 2>&1; then
        echo "Установите Perl: sudo dnf install -y perl"
    elif command -v apt-get >/dev/null 2>&1; then
        echo "Установите Perl: sudo apt-get update && sudo apt-get install -y perl"
    fi
    exit 1
fi

REQUIRED_PERL_MODULES=(FindBin IPC::Cmd File::Compare File::Copy)
MISSING_PERL_MODULES=()
for module in "${REQUIRED_PERL_MODULES[@]}"; do
    if ! perl -M"$module" -e '1' >/dev/null 2>&1; then
        MISSING_PERL_MODULES+=("$module")
    fi
done

if [ ${#MISSING_PERL_MODULES[@]} -ne 0 ]; then
    echo -e "${RED}Ошибка: отсутствуют Perl-модули для сборки openssl: ${MISSING_PERL_MODULES[*]}${NC}"
    if command -v dnf >/dev/null 2>&1; then
        echo "Рекомендуется установить полный Perl: sudo dnf install -y perl"
        echo "Или минимально необходимые модули: sudo dnf install -y perl-FindBin perl-IPC-Cmd perl-File-Compare perl-File-Copy"
    elif command -v apt-get >/dev/null 2>&1; then
        echo "Установите Perl core-модули через пакеты вашей системы (например: perl-modules)."
    else
        echo "Установите Perl-модули FindBin, IPC::Cmd, File::Compare и File::Copy через менеджер пакетов вашей ОС."
    fi
    exit 1
fi

cd "$PROJECT_ROOT"

echo -e "${GREEN}Настройка окружения проекта для сборки типа $BUILD_TYPE...${NC}"

echo -e "${GREEN}Установка зависимостей Conan...${NC}"
echo -e "${GREEN}Host profile: $(basename "$HOST_PROFILE_PATH"), Build profile: $(basename "$BUILD_PROFILE_PATH")${NC}"
conan install . --build=missing -pr:h "$HOST_PROFILE_PATH" -pr:b "$BUILD_PROFILE_PATH"

echo -e "${GREEN}Окружение успешно настроено!${NC}"
