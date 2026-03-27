#include "daApi.hpp"
#include <cpr/api.h>
#include <cpr/body.h>
#include <exception>
#include <stdexcept>
#include <string>

namespace donation_alerts {

    Client::Client(const std::string& clientId, const std::string& clientSecret, const std::string& redirectUri)
        : _clientId(std::move(clientId)),
          _clientSecret(std::move(clientSecret)),
          _redirectUri(std::move(redirectUri)) {}

    std::string Client::generateAuthUrl(const std::vector<DA_SCOPE>& scopes) const {
        std::string scopesStr;
        // Собираем все запрошенные права в одну строку через пробел
        for (const auto& scope : scopes) {
            scopesStr += nlohmann::json(scope).get<std::string>() + " ";
        }

        // Удаляем последний лишний пробел, если он есть
        if (!scopesStr.empty()) scopesStr.pop_back();

        // Формируем URL. cpr::util::urlEncode защищает спецсимволы (пробелы превращаются в %20 и т.д.)
        return OAUTH_BASE_URL + "/authorize?" + "client_id=" + _clientId +
               "&redirect_uri=" + cpr::util::urlEncode(_redirectUri) + "&response_type=code" +
               "&scope=" + cpr::util::urlEncode(scopesStr);
    }

    bool Client::authorize(const std::string& auth_code) {
        // Отправляем POST запрос (cpr::Post) для обмена кода на токен
        cpr::Response r = cpr::Post(cpr::Url{OAUTH_BASE_URL + "/token"},
                                    // Payload - это тело POST запроса в формате ключ-значение (как в HTML формах)
                                    cpr::Payload{{"grant_type", "authorization_code"},
                                                 {"client_id", _clientId},
                                                 {"client_secret", _clientSecret},
                                                 {"redirect_uri", _redirectUri},
                                                 {"code", auth_code}});

        if (r.status_code == 200) {
            // Если сервер ответил 200 OK, парсим JSON и сохраняем токен
            nlohmann::json j = nlohmann::json::parse(r.text);
            _tokenInfo = j.get<TokenInfo>();
            return true;
        } else {
            // Выводим ошибку в консоль для отладки
            throw std::runtime_error("Auth Failed! Status: " + std::to_string(r.status_code) +
                                     " | Response: " + r.text);
        }
    }

    cpr::Header Client::getAuthHeader() const {
        if (_tokenInfo.has_value()) {
            return cpr::Header{{"Authorization", "Bearer " + _tokenInfo->access_token}};
        }
        return cpr::Header{};
    }

    bool Client::isAuthorized() const { return _tokenInfo.has_value() && !_tokenInfo->access_token.empty(); }

    std::optional<UserProfile> Client::getProfile() const {
        if (!isAuthorized()) {
            throw std::runtime_error("Client is not autorized");
        }
        // Выполняем GET запрос, передавая наш сгенерированный заголовок с токеном
        cpr::Response r = cpr::Get(cpr::Url{API_BASE_URL + "/user"}, getAuthHeader());

        if (r.status_code == 200) {
            nlohmann::json j = nlohmann::json::parse(r.text);
            // DonationAlerts оборачивает данные в объект "data"
            return j.at("data").get<UserProfile>();
        }

        throw std::runtime_error("Failed to get profile. Status: " + std::to_string(r.status_code) + " " + r.reason);
    }

    std::optional<DonationsResponse> Client::getDonations(int page) const {
        if (!isAuthorized()) {
            throw std::runtime_error("Client is not autorized");
        }

        cpr::Response r = cpr::Get(cpr::Url{API_BASE_URL + "/alerts/donations"}, getAuthHeader(),
                                   cpr::Parameters{{"page", std::to_string(page)}});

        if (r.status_code == 200) {
            try {
                auto j = nlohmann::json::parse(r.text);
                return j.get<DonationsResponse>();
            } catch (...) {
                throw;
            }
        }
        throw std::runtime_error("Failed to get donation page. Status: " + std::to_string(r.status_code) + " " +
                                 r.reason);
    }

    std::optional<CustomAlertResponse> Client::setCustomAlert(CustomAlert customAlert) const {
        if (!isAuthorized()) {
            throw std::runtime_error("Client is not autorized");
        }

        cpr::Payload payload{
            {"external_id", customAlert.external_id}, {"header", customAlert.header},
            {"message", customAlert.message},         {"is_shown", std::to_string(customAlert.is_shown)},
            {"image_url", customAlert.image_url},     {"sound_url", customAlert.sound_url}};

        cpr::Response r = cpr::Post(cpr::Url{API_BASE_URL + "/custom_alert"}, getAuthHeader(), payload);

        if (r.status_code == 200) {
            try {
                auto j = nlohmann::json::parse(r.text);
                return j.get<CustomAlertResponse>();
            } catch (...) {
                return std::nullopt;
            }
        }
        throw std::runtime_error("Failed to set custom alert. Status: " + std::to_string(r.status_code) + " " +
                                 r.reason);
    }

}  // namespace donation_alerts
