#pragma once

#include "utils/id_utils.h"
#include "base/result.h"

namespace bt {
    class BT_API alert_subscriber BT_ABS_CLASS {
    public:
        virtual ~alert_subscriber() = default;
        virtual void info(const std::string& msg) = 0;
        virtual void error(const error& err) = 0;
    };

    using subscriber_map = std::unordered_map<id_t, alert_subscriber*>;

    class BT_API alert_service {
    public:
        alert_service() = default;

        virtual ~alert_service() = default;

        alert_service(const alert_service&) = default;
        alert_service(alert_service&&) = default;
        alert_service& operator=(const alert_service&) = default;
        alert_service& operator=(alert_service&&) = default;

        id_t subscribe(alert_subscriber& subscriber);

        void unsubscribe(id_t subscription_id);

        void notify_info(const std::string& msg);

        void notify_error(const error& err);

    private:
        subscriber_map m_subscriber_map;
    };
}