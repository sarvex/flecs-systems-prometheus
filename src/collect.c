#include <flecs_systems_prometheus.h>
#include "collect.h"

static
void PromWorldCountersAdd(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, PromWorldCounters, 2);

    ecs_set(rows->world, rows->entities[0], PromWorldCounters, {0});
}

static
void PromSystemCountersAdd(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, PromSystemCounters, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], PromSystemCounters, {0});
    }
}

static
void PromWorldCountersCollect(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);
    ECS_COLUMN(rows, PromWorldCounters, counters, 2);

    counters->frame_time_total += stats->frame_time;
    counters->system_time_total += stats->system_time;
    counters->merge_time_total += stats->merge_time;
}

static
void PromSystemCountersCollect(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);
    ECS_COLUMN(rows, PromSystemCounters, counters, 2);

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        counters[i].time_spent += stats[i].time_spent;

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

    ECS_COMPONENT(world, PromWorldCounters);
    ECS_COMPONENT(world, PromSystemCounters);
    ECS_TAG(world, PromSkipCollect);

    ECS_SYSTEM(world, PromWorldCountersAdd,     EcsOnStore,  
        [in] EcsWorldStats, !PromWorldCounters);

    ECS_SYSTEM(world, PromSystemCountersAdd,     EcsOnStore,  
        [in] EcsSystemStats, !PromSystemCounters,
        SYSTEM.PromSkipCollect, !PromSkipCollect);

    ECS_SYSTEM(world, PromWorldCountersCollect, EcsPostLoad,
        [in] EcsWorldStats, PromWorldCounters);

    ECS_SYSTEM(world, PromSystemCountersCollect, EcsPostLoad, 
        [in] EcsSystemStats, PromSystemCounters);

    ECS_EXPORT_COMPONENT(PromSystemCounters);
}
