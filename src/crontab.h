#ifndef __CRONTAB_H
#define __CRONTAB_H

#include <functional>
#include <variant>

extern "C" {
    #include "ccroneexpr/ccronexpr.h"
}


#define CRONTAB_MAX_ENTRY_COUNT     12
typedef int CronID; 
typedef enum {
    CRON_ERROR_PARSE,
    CRON_ERROR_NO_MEM
} CronError;

class CronTab;

class CronEntry {
public:
    CronEntry();
    CronEntry(CronTab&, cron_expr expr, std::function<void()> callback);

    bool                        isEnabled();
    void                        check(time_t now);
private:
    bool                        enabled;
    cron_expr                   expr;
    time_t                      next;
    std::function<void()>       callback;
};


class CronTab {
public:
    CronTab();
    ~CronTab();

    std::variant<CronID,CronError>      add(const char* entry, std::function<void()> callback);
    void                                remove(CronID id);
    void                                removeAll();

    bool                                isAllocated(CronID id);
    int                                 count();

    void                                serve();

    void                                setTimeFunction(std::function<time_t(time_t*)> fn);
    time_t                              time(time_t*);
private:
    std::function<time_t(time_t*)>      getTimeFunc;

    CronEntry                           entries[CRONTAB_MAX_ENTRY_COUNT];
};


#endif