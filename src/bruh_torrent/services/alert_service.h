#pragma once

#include "utils/id_utils.h"
#include "utils/result.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace bt {
    class alert_subscriber {
    public:
        virtual void info(const std::string& msg) = 0;
        virtual void error(const error& err) = 0;
    };

    using subscriber_map = std::unordered_map<id_t, alert_subscriber*>;

    class alert_service {
    public:
        alert_service() = default;

        virtual ~alert_service() = default;

        id_t subscribe(alert_subscriber& subscriber);

        void unsubscribe(id_t subscription_id);

        void notify_info(const std::string& msg);

        void notify_error(const error& err);

    private:
        subscriber_map m_subscriber_map;
    };
}