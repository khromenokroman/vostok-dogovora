#pragma once

#include <httplib.h>

#include "config.hpp"
#include "db/contract_repository.hpp"
#include "db/database.hpp"
#include "db/settings_repository.hpp"
#include "db/tenant_repository.hpp"

class App {
public:
    explicit App(Config config);

    void run();
    void stop();

private:
    void register_routes();

    // dashboard
    void handle_dashboard(const httplib::Request &req, httplib::Response &res);

    // tenants
    void handle_tenants_list(const httplib::Request &req, httplib::Response &res);
    void handle_tenants_add_form(const httplib::Request &req, httplib::Response &res);
    void handle_tenants_add(const httplib::Request &req, httplib::Response &res);
    void handle_tenants_edit_form(const httplib::Request &req, httplib::Response &res);
    void handle_tenants_update(const httplib::Request &req, httplib::Response &res);
    void handle_tenants_delete(const httplib::Request &req, httplib::Response &res);
    Tenant tenant_from_form(const httplib::Request &req, int64_t id) const;

    // contracts
    void handle_contracts_list(const httplib::Request &req, httplib::Response &res);
    void handle_contracts_add_form(const httplib::Request &req, httplib::Response &res);
    void handle_contracts_add(const httplib::Request &req, httplib::Response &res);
    void handle_contracts_edit_form(const httplib::Request &req, httplib::Response &res);
    void handle_contracts_update(const httplib::Request &req, httplib::Response &res);
    void handle_contracts_delete(const httplib::Request &req, httplib::Response &res);
    void handle_contracts_toggle_signed(const httplib::Request &req, httplib::Response &res);

    // batch creation
    void handle_batch_new_form(const httplib::Request &req, httplib::Response &res);
    void handle_batch_create(const httplib::Request &req, httplib::Response &res);

    // print
    void handle_print_contract(const httplib::Request &req, httplib::Response &res);
    void handle_print_batch(const httplib::Request &req, httplib::Response &res);
    void handle_print_selected(const httplib::Request &req, httplib::Response &res);

    // settings
    void handle_settings_form(const httplib::Request &req, httplib::Response &res);
    void handle_settings_update(const httplib::Request &req, httplib::Response &res);

    static void not_found(httplib::Response &res, const std::string &message);

    Config m_config;
    Database m_db;
    TenantRepository m_tenants;
    ContractRepository m_contracts;
    SettingsRepository m_settings;
    httplib::Server m_server;
};
