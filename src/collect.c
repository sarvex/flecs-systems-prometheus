#include <flecs_systems_prometheus.h>
#include "collect.h"

static
void PromSystemCountersAdd(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, PromSystemCounters, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], PromSystemCounters, {0});
    }
}

static
void PromSystemCountersCollect(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);
    ECS_COLUMN(rows, PromSystemCounters, counters, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        counters->time_spent += stats->time_spent;
        if (stats->enabled && stats->active) {
            counters->ticks ++;
        }
    }
}

void PrometheusCollectImport(
    ecs_world_t *world,
    int flags)
{
    ECS_MODULE(world, PrometheusCollect);

    ECS_COMPONENT(world, PromSystemCounters);
    ECS_TAG(world, PromSkipCollect);

    ECS_SYSTEM(world, PromSystemCountersAdd,     EcsOnStore,  
        [in] EcsSystemStats, !PromSystemCounters,
        SYSTEM.PromSkipCollect, !PromSkipCollect);

    ECS_SYSTEM(world, PromSystemCountersCollect, EcsPostLoad, 
        [in] EcsSystemStats, PromSystemCounters);

    ECS_EXPORT_COMPONENT(PromSystemCounters);
}
