
#include "base/bt_pch.h"
#include "alert_service.h"

namespace bt {
    id_t alert_service::subscribe(alert_subscriber& subscriber) {
        const id_t id = id_utils::generate_id();
        m_subscriber_map[id] = &subscriber;
        return id;
    }

    void alert_service::unsubscribe(const id_t subscription_id) {
        if (m_subscriber_map.contains(subscription_id)) {
            m_subscriber_map.erase(subscription_id);
        }
    }

    void alert_service::notify_info(const std::string& msg) {
        for (auto& [id, sub] : m_subscriber_map) sub->info(msg);
    }

    void alert_service::notify_error(const error& err) {
        for (auto& [id, sub] : m_subscriber_map) sub->error(err);
    }
}