# da-cpp-lib

**da-cpp-lib** — это легковесная C++ библиотека для взаимодействия с [API DonationAlerts](https://www.donationalerts.com/apidoc). Она предоставляет удобный интерфейс для OAuth2-авторизации, получения профиля пользователя и списка донатов.

Проект написан с использованием современных стандартов (C++23) и ориентирован на чистую архитектуру и простоту использования.

## 🚀 Возможности

* **OAuth2 Авторизация:** Генерация ссылок для входа и автоматический обмен кода на токены доступа (`access_token`).
* **Профиль пользователя:** Получение базовой информации об авторизованном стримере.
* **Донаты:** Получение списка пожертвований с поддержкой пагинации (постраничного вывода).
* **Кастомные оповещения:** Генерация кастомных оповещений.

## 🛠 Технологии и зависимости

Для сборки и работы библиотеки требуются:
* **Компилятор:** Clang (с поддержкой C++23)
* **Система сборки:** CMake (>= 3.17) + Ninja
* **Зависимости:**
  * [nlohmann/json](https://github.com/nlohmann/json) — для парсинга ответов сервера.
  * [cpr](https://github.com/libcpr/cpr) — C++ Requests, для выполнения HTTP-запросов.

## 📦 Установка зависимостей

В зависимости от вашей операционной системы, выполните следующие команды для установки необходимых библиотек.

### Для Windows (через vcpkg)
```bash
vcpkg install nlohmann-json cpr crow --triplet=x64-windows
```

### Для Linux (через apt)
*Примечание: имена пакетов могут незначительно отличаться в зависимости от вашего дистрибутива (например, Ubuntu/Debian).*
```bash
sudo apt update
sudo apt install nlohmann-json3-dev libcpr-dev libcrow-dev 
```

## ⚙️ Настройка среды разработки (VSCode)

Для комфортной работы в Visual Studio Code с использованием `CMake`, `Clang` и `Ninja`, убедитесь, что у вас установлены расширения **CMake Tools** и **clangd**.

Добавьте следующие настройки в файл `.vscode/settings.json` вашего проекта:

```json
{
    "cmake.generator": "Ninja",
    "cmake.configureSettings": {
        "CMAKE_CXX_COMPILER": "clang++",
        "CMAKE_C_COMPILER": "clang"
    },
    "C_Cpp.intelliSenseEngine": "disabled",
    "clangd.arguments": [
        "--compile-commands-dir=${workspaceFolder}/build",
        "--background-index",
        "--clang-tidy",
        "--header-insertion=iwyu"
    ]
}
```
*Подсказка: Отключение `C_Cpp.intelliSenseEngine` необходимо, чтобы встроенный IntelliSense от Microsoft не конфликтовал с более быстрым и точным анализатором `clangd`.*

## 📖 Пример использования

Ниже приведен базовый пример того, как инициализировать клиент и получить ссылку для авторизации.

```cpp
#include <iostream>
#include "daApi.hpp"

using namespace donation_alerts;

int main() {
    // 1. Инициализация клиента
    // Данные (Client ID, Secret, Redirect URI) выдаются в панели разработчика DonationAlerts
    Client daClient(
        "YOUR_CLIENT_ID", 
        "YOUR_CLIENT_SECRET", 
        "http://localhost:8080/callback" // URL, куда DA вернет пользователя после логина
    );

    // 2. Формируем список прав (scopes), которые мы просим у пользователя
    std::vector<DA_SCOPE> scopes = {
        DA_SCOPE::UserShow,      // Право видеть профиль
        DA_SCOPE::DonationIndex  // Право читать список донатов
    };

    // 3. Генерируем ссылку и просим пользователя по ней перейти
    std::string authUrl = daClient.generateAuthUrl(scopes);
    std::cout << "Пожалуйста, перейдите по ссылке для авторизации:\n" << authUrl << "\n\n";

    // 4. В реальном приложении здесь веб-сервер (например, Crow) поймает callback.
    // Для примера просим ввести код из URL вручную:
    std::cout << "Введите код (auth_code) из адресной строки браузера: ";
    std::string authCode;
    std::cin >> authCode;

    // 5. Обмениваем код на токен
    try {
        if (daClient.authorize(authCode)) {
            std::cout << "Авторизация прошла успешно!\n";

            // std::optional означает, что результат может быть пустым (например, если нет связи с сервером).
            // Метод has_value() проверяет, получили ли мы реальные данные.
            auto profile = daClient.getProfile();
            if (profile.has_value()) {
                std::cout << "Привет, " << profile->name << "!\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}
```

## 🏗 Интеграция в ваш проект

Так как библиотека собирается как статическая (`STATIC`), вам достаточно подключить её в вашем корневом `CMakeLists.txt`:

```cmake
add_subdirectory(path/to/da-cpp-lib)
target_link_libraries(your_executable PRIVATE da-cpp-lib)
```