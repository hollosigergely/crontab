#include "crontab.h"
#include <time.h>
using namespace std;

CronEntry::CronEntry() :
    enabled(false),
    callback(nullptr)
{

}

CronEntry::CronEntry(CronTab& crontab, cron_expr expr, std::function<void()> callback) :
    expr(expr),
    callback(callback)
{
    this->next = cron_next(&expr, crontab.time(nullptr));
    this->enabled = true;
}

bool CronEntry::isEnabled()
{
    return enabled;
}

void CronEntry::check(time_t now) {
    if(now >= this->next) {
        if(this->callback != nullptr)
            this->callback();

        this->next = cron_next(&expr, now);
    }
}



CronTab::CronTab() :
    getTimeFunc(::time)
{
    
}

CronTab::~CronTab() {

}

time_t CronTab::time(time_t* t)
{
    return getTimeFunc(t);
}

std::variant<CronID,CronError> CronTab::add(const char* entry, std::function<void()> callback)
{
    const char* err = NULL;
    cron_expr   expr;

    cron_parse_expr(entry, &expr, &err);
    if(err != NULL)
    {
        return CRON_ERROR_PARSE;
    }

    for(int i = 0; i < CRONTAB_MAX_ENTRY_COUNT; i++)
    {
        if(!entries[i].isEnabled())
        {
            entries[i] = CronEntry(*this, expr, callback);

            return i;
        }
    }

    return CRON_ERROR_NO_MEM;
}

void CronTab::remove(CronID id)
{
    if(id >= 0 && id < CRONTAB_MAX_ENTRY_COUNT)
    {
        this->entries[id] = CronEntry();
    }
}

void CronTab::removeAll()
{
    for(int i = 0; i < CRONTAB_MAX_ENTRY_COUNT; i++)
    {
        remove(i);
    }
}
    
bool CronTab::isAllocated(CronID id)
{
    return id >=0 && id < CRONTAB_MAX_ENTRY_COUNT && entries[id].isEnabled();
}

int CronTab::count()
{
    int c = 0;
    for(int i = 0; i < CRONTAB_MAX_ENTRY_COUNT; i++)
        c += (entries[i].isEnabled()?1:0);
        
    return c;
}


void CronTab::serve()
{
    time_t now = time(nullptr);
    for(int i = 0; i < CRONTAB_MAX_ENTRY_COUNT; i++)
    {
        if(entries[i].isEnabled())
        {
            entries[i].check(now);
        }
    }
}