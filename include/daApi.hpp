#include "daType.hpp"
#include <cpr/cpr.h>

namespace donation_alerts {

    class Client {
    public:
        // Конструктор принимает данные вашего приложения (их нужно получить в панели разработчика DA)
        Client(const std::string& clientId, const std::string& clientSecret, const std::string& redirectUri);

        // Генерация ссылки для авторизации в браузере
        [[nodiscard]] std::string generateAuthUrl(const std::vector<DA_SCOPE>& scopes) const;

        // Обмен полученного кода на рабочие токены
        // Возвращает true, если авторизация прошла успешно
        bool authorize(const std::string& authCode);

        // Пример запроса к API: получение профиля
        // std::optional означает, что профиль может быть не получен (например, если нет интернета или токен устарел)
        [[nodiscard]] std::optional<UserProfile> getProfile() const;
        // Получение списка донатов. page — номер страницы (начинается с 1)
        [[nodiscard]] std::optional<DonationsResponse> getDonations(int page = 1) const;
        // Отпрака кастомного оповещения
        [[nodiscard]] std::optional<CustomAlertResponse> setCustomAlert(CustomAlert customAlert) const;

        // Проверка, авторизован ли клиент (есть ли у нас токен)
        [[nodiscard]] bool isAuthorized() const;

    private:
        // Учетные данные приложения
        std::string _clientId;
        std::string _clientSecret;
        std::string _redirectUri;

        // Здесь мы будем хранить текущие токены после успешной авторизации
        std::optional<TokenInfo> _tokenInfo; 

        // Базовые URL сервиса
        const std::string API_BASE_URL = "https://www.donationalerts.com/api/v1";
        const std::string OAUTH_BASE_URL = "https://www.donationalerts.com/oauth";

        // Вспомогательный приватный метод для создания заголовка авторизации,
        // чтобы не писать одно и то же в каждом запросе.
        [[nodiscard]] cpr::Header getAuthHeader() const;
    };
}  // namespace donation_alerts
