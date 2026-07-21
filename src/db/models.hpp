#pragma once

#include <optional>
#include <string>

struct OrganizationSettings {
    std::string org_name;
    std::string director_fio;
    std::string director_basis;
    std::string inn;
    std::string kpp;
    std::string legal_address;
    std::string bank_name;
    std::string bank_account;
    std::string bank_corr_account;
    std::string bank_bik;
    std::string mall_name;
    std::string mall_city;
    std::string mall_address;
    std::string mall_floor;
};

struct Tenant {
    int64_t id = 0;
    std::string fio;
    std::string activity_basis;
    std::string address;
    std::string inn;
    std::string ogrn;
    std::string passport_data;
    std::string phone;
    std::string email;
    std::string room_number;
    double area_total_sqm = 0.0;
    double area_trade_sqm = 0.0;
    std::string notes;
};

struct Contract {
    int64_t id = 0;
    int64_t tenant_id = 0;
    int64_t rent_kopecks = 0;
    std::string start_date;  // 'YYYY-MM-DD'
    std::string end_date;    // 'YYYY-MM-DD'
    std::optional<int64_t> batch_id;
    std::optional<int64_t> batch_seq;
    bool is_signed = false;
    std::optional<std::string> signed_at;
};
