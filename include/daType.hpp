#include "nlohmann/json.hpp"

namespace donation_alerts {
    enum class DA_SCOPE {
        UserShow,           // Получение данных профиля пользователя
        DonationSubscribe,  // Подписка на новые оповещения о донатах
        DonationIndex,      // Просмотр списка донатов
        CustomAlertStore,   // Создание пользовательских оповещений
        GoalSubscribe,      // Подписка на обновления целей сбора средств
        PollSubscribe       // Подписка на обновления опросов
    };
    NLOHMANN_JSON_SERIALIZE_ENUM(DA_SCOPE, {{DA_SCOPE::UserShow, "oauth-user-show"},
                                            {DA_SCOPE::DonationSubscribe, "oauth-donation-subscribe"},
                                            {DA_SCOPE::DonationIndex, "oauth-donation-index"},
                                            {DA_SCOPE::CustomAlertStore, "oauth-custom_alert-store"},
                                            {DA_SCOPE::GoalSubscribe, "oauth-goal-subscribe"},
                                            {DA_SCOPE::PollSubscribe, "oauth-poll-subscribe"}})

    struct TokenInfo {
        std::string token_type;
        int expires_in = 0;
        std::string access_token;
        std::string refresh_token;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(TokenInfo, token_type, expires_in, access_token, refresh_token)
    };

    struct UserProfile {
        int id = 0;
        std::string code;
        std::string name;
        std::string avatar;
        std::string email;
        std::string socket_connection_token;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(UserProfile, id, name, code, avatar, email, socket_connection_token)
    };

    struct Donation {
        int id = 0;
        std::string name;                     // Имя, которое ввел отправитель
        std::string username;  // Имя зарегистрированного аккаунта (может не быть)
        std::string message;   // Текст доната (может быть пустым)
        double amount = 0.0;                  // Сумма доната
        std::string currency;                 // Валюта (например, "RUB", "USD")
        std::string created_at;               // Дата в формате ISO 8601

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Donation, id, name, username, message, amount, currency, created_at)
    };

    struct DonationsResponse {
        std::vector<Donation> data;
        // Вспомогательная структура для метаданных пагинации
        struct Meta {
            int current_page;
            int last_page;
            int total;
            NLOHMANN_DEFINE_TYPE_INTRUSIVE(Meta, current_page, last_page, total)
        } meta;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(DonationsResponse, data, meta)
    };

    struct CustomAlert {
        std::string external_id;
        std::string header;
        std::string message= "";
        int is_shown = 0;
        std::string image_url= "";
        std::string sound_url= "";

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(CustomAlert, external_id, header, message, is_shown, image_url, sound_url)
    };

    struct CustomAlertResponse {
        int id = 0;
        std::string external_id;
        std::string header;
        std::string message;
        std::string image_url;
        std::string sound_url;
        int is_shown = 0;
        std::string created_at;
        std::string shown_at;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(CustomAlertResponse, id, external_id, header, message, image_url, sound_url, is_shown, created_at, shown_at)
    };

    struct Pagination {
        int current_page = 0;
        int last_page = 0;
        int per_page = 0;
        int total = 0;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Pagination, current_page, last_page, per_page, total)
    };

    template <typename T>
    struct PaginatedResponse {
        std::vector<T> data;
        Pagination pagination;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(PaginatedResponse, data, pagination)
    };

}  // namespace donation_alerts
