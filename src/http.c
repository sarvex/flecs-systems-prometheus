#include "http.h"
#include "collect.h"

/* -- Allocation statistics -- */

static
void PromHttpReplyAllocCount(ecs_rows_t *rows) 
{
    ECS_COLUMN(rows, EcsAllocStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    ecs_strbuf_appendstr(reply, "#HELP flecs_alloc_count Number of allocations since application start\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_alloc_count counter\n");

    /* Combine malloc and calloc into a single metric */
    ecs_strbuf_append(reply, "flecs_alloc_count{kind=\"malloc\"} %u\n", 
        stats->malloc_count);
    ecs_strbuf_append(reply, "flecs_alloc_count{kind=\"calloc\"} %u\n", 
        stats->calloc_count); 
    ecs_strbuf_appendstr(reply, "\n");

    /* Separate metric for reallocs, since it is not an allocation that can be 
     * easily summed up with malloc and calloc */
    ecs_strbuf_appendstr(reply, "#HELP flecs_realloc_count Number of reallocations since application start\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_realloc_count counter\n");

    ecs_strbuf_append(reply, "flecs_realloc_count %u\n", 
        stats->realloc_count);
     ecs_strbuf_appendstr(reply, "\n");

    /* Separate metric for frees, since it is not something that can be summed
     * with allocations */
    ecs_strbuf_appendstr(reply, "#HELP flecs_free_count Number of frees since application start\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_free_count counter\n");

    ecs_strbuf_append(reply, "flecs_free_count %u\n", 
        stats->free_count);
}

/* -- World -- */

static
void world_metric_header(
    ecs_strbuf_t *reply,
    const char *metric_name)
{
    ecs_strbuf_append(reply, "%s ", metric_name);
}

static
void PromHttpReplyWorldTableCount(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_world_table_count Number of tables in world\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_world_table_count gauge\n");
    }

    world_metric_header(reply, "flecs_world_table_count");
    ecs_strbuf_append(reply, "%u\n", stats->table_count);
}

static
void PromHttpReplyWorldComponentCount(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_world_component_count Number of components in world\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_world_component_count gauge\n");
    }

    world_metric_header(reply, "flecs_world_component_count");
    ecs_strbuf_append(reply, "%u\n", stats->component_count);
}

static
void PromHttpReplyWorldEntityCount(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_world_entity_count Number of entities in world\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_world_entity_count gauge\n");
    }

    world_metric_header(reply, "flecs_world_entity_count");
    ecs_strbuf_append(reply, "%u\n", stats->entity_count);
}

static
void PromHttpReplyWorldThreadCount(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_world_thread_count Number of threads in world\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_world_thread_count gauge\n");
    }

    world_metric_header(reply, "flecs_world_thread_count");
    ecs_strbuf_append(reply, "%u\n", stats->thread_count);
}

static
void PromHttpReplyWorldTickCount(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_world_tick_count Number of ticks in world\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_world_tick_count counter\n");
    }

    world_metric_header(reply, "flecs_world_tick_count");
    ecs_strbuf_append(reply, "%u\n", stats->tick_count);
}

static
void PromHttpReplyWorldTimeTotal(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, PromWorldCounters, counters, 2);

    ecs_strbuf_t *reply = rows->param;

    ecs_strbuf_appendstr(reply, "#HELP flecs_world_time_total Time spent\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_world_time_total counter\n");

    ecs_strbuf_append(reply, "flecs_world_time_total{kind=\"frame\"} %f\n", 
        counters->frame_time_total);

    ecs_strbuf_append(reply, "flecs_world_time_total{kind=\"system\"} %f\n", 
        counters->system_time_total);

    ecs_strbuf_append(reply, "flecs_world_time_total{kind=\"merge\"} %f\n", 
        counters->merge_time_total);                
}

static
void PromHttpReplyWorldTime(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    ecs_strbuf_appendstr(reply, "#HELP flecs_world_time Time expired since world creation\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_world_time counter\n");

    world_metric_header(reply, "flecs_world_time");
    ecs_strbuf_append(reply, "%f\n", stats->world_time);
}

static
void PromHttpReplyWorldTargetFps(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    ecs_strbuf_appendstr(reply, "#HELP flecs_world_target_fps Target frames per second\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_world_target_fps counter\n");

    world_metric_header(reply, "flecs_world_target_fps");
    ecs_strbuf_append(reply, "%f\n", stats->target_fps);
}


/* -- Systems -- */

static
const char* system_kind_str(
    EcsSystemKind kind)
{
    switch(kind) {
    case EcsOnLoad: return "EcsOnLoad";
    case EcsPostLoad: return "EcsPostLoad";
    case EcsPreUpdate: return "EcsPreUpdate";
    case EcsOnUpdate: return "EcsOnUpdate";
    case EcsOnValidate: return "EcsOnValidate";
    case EcsPostUpdate: return "EcsPostUpdate";
    case EcsPreStore: return "EcsPreStore";
    case EcsOnStore: return "EcsOnStore";
    case EcsManual: return "EcsManual";
    case EcsOnAdd: return "EcsOnAdd";
    case EcsOnRemove: return "EcsOnRemove";
    case EcsOnSet: return "EcsOnSet";
    }

    return "UNKNOWN_SYSTEM_KIND";
}

static
void system_metric_header(
    ecs_strbuf_t *reply,
    const char *metric_name,
    EcsSystemStats *stats)
{
    ecs_strbuf_append(reply, "%s {system=\"%s\",phase=\"%s\",enabled=\"%s\",active=\"%s\"} ",
        metric_name, stats->id, system_kind_str(stats->kind),
        stats->enabled ? "true" : "false",
        stats->active ? "true" : "false");
}

static
void PromHttpReplySystemTimeSpent(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);
    ECS_COLUMN(rows, PromSystemCounters, counters, 2);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_time_total Time spent in system\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_time_total counter\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_metric_header(reply, "flecs_system_time_total", &stats[i]);
        ecs_strbuf_append(reply, "%f\n", counters[i].time_spent);
    }
}

static
void PromHttpReplySystemTicks(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);
    ECS_COLUMN(rows, PromSystemCounters, counters, 2);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_ticks_total Number of times the system has ticked\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_ticks_total counter\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_metric_header(reply, "flecs_system_ticks_total", &stats[i]);
        ecs_strbuf_append(reply, "%f\n", counters[i].ticks);
    }
}

static
void PromHttpReplySystemEntitiesMatched(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_entities_matched Number of entities matched with system\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_entities_matched gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_metric_header(reply, "flecs_system_entities_matched", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].entities_matched);
    }
}

static
void PromHttpReplySystemTablesMatched(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_tables_matched Number of tables matched with system\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_tables_matched gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_metric_header(reply, "flecs_system_tables_matched", &stats[i]);        
        ecs_strbuf_append(reply, "%u\n", stats[i].tables_matched);
    }
}

/* -- System memory -- */

/* TODO: add memory stats for row systems */

static
void system_mem_metric_header(
    ecs_strbuf_t *reply,
    const char *metric_name,
    const char *kind,
    EcsSystemStats *stats)
{
    ecs_strbuf_append(reply, "%s {system=\"%s\",phase=\"%s\",kind=\"%s\",enabled=\"%s\",active=\"%s\"} ",
        metric_name, stats->id, system_kind_str(stats->kind), kind,
        stats->enabled ? "true" : "false",
        stats->active ? "true" : "false");
}

static
void PromHttpReplySystemMemoryUsed(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);
    ECS_COLUMN(rows, EcsColSystemMemoryStats, memstats, 2);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_memory_used Amount of memory used by system\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_memory_used gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_mem_metric_header(reply, "flecs_system_memory_used", "base", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].base);

        system_mem_metric_header(reply, "flecs_system_memory_used", "other", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].other);

        system_mem_metric_header(reply, "flecs_system_memory_used", "columns", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].columns.used);

        system_mem_metric_header(reply, "flecs_system_memory_used", "active_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].active_tables.used);

        system_mem_metric_header(reply, "flecs_system_memory_used", "inactive_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].inactive_tables.used);

        system_mem_metric_header(reply, "flecs_system_memory_used", "jobs", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].jobs.used);
    }
}

static
void PromHttpReplySystemMemoryAllocd(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);
    ECS_COLUMN(rows, EcsColSystemMemoryStats, memstats, 2);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_memory_allocated Amount of memory allocated by system\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_memory_allocated gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_mem_metric_header(reply, "flecs_system_memory_allocated", "base", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].base);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "other", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].other);    

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "columns", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].columns.allocd);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "active_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].active_tables.allocd);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "inactive_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].inactive_tables.allocd);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "jobs", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].jobs.allocd);
    }
}

/* -- Components -- */

static
void component_metric_header(
    ecs_strbuf_t *reply,
    const char *metric_name,
    EcsComponentStats *stats)
{
    ecs_strbuf_append(reply, "%s {component=\"%s\"} ",
        metric_name, stats->id);
}

static
void PromHttpReplyCompTableCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsComponentStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_component_table_count Number of tables component appears in\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_component_table_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        component_metric_header(reply, "flecs_component_table_count", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].tables);
    }
}

static
void PromHttpReplyCompEntityCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsComponentStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_component_entity_count Number of entities with component\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_component_entity_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        component_metric_header(reply, "flecs_component_entity_count", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].entities);
    }
}

static
void PromHttpReplyCompMemoryUsed(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsComponentStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_component_memory_used Component memory in use\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_component_memory_used gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        component_metric_header(reply, "flecs_component_memory_used", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].memory_used);
    }
}

static
void PromHttpReplyCompMemoryAllocd(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsComponentStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_component_memory_allocd Component memory allocated\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_component_memory_allocd gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        component_metric_header(reply, "flecs_component_memory_allocd", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].memory_allocd);
    }
}

static
void PromHttpReply(ecs_rows_t *rows) {
    ECS_COLUMN_ENTITY(rows, PromHttpReplyAllocCount, 1);

    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldTableCount, 2);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldComponentCount, 3);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldEntityCount, 4);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldThreadCount, 5);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldTickCount, 6);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldTimeTotal, 7);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldTime, 8);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyWorldTargetFps, 9);

    ECS_COLUMN_ENTITY(rows, PromHttpReplySystemTimeSpent, 10);
    ECS_COLUMN_ENTITY(rows, PromHttpReplySystemTicks, 11);
    ECS_COLUMN_ENTITY(rows, PromHttpReplySystemEntitiesMatched, 12);
    ECS_COLUMN_ENTITY(rows, PromHttpReplySystemTablesMatched, 13);

    ECS_COLUMN_ENTITY(rows, PromHttpReplySystemMemoryUsed, 14);
    ECS_COLUMN_ENTITY(rows, PromHttpReplySystemMemoryAllocd, 15);

    ECS_COLUMN_ENTITY(rows, PromHttpReplyCompTableCount, 16);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyCompEntityCount, 17);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyCompMemoryUsed, 18);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyCompMemoryAllocd, 19);

    ecs_strbuf_t *reply = rows->param;

    /* Add allocation statistics */
    ecs_run(rows->world, PromHttpReplyAllocCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add world statistics */
    ecs_run(rows->world, PromHttpReplyWorldTableCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyWorldComponentCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyWorldEntityCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyWorldThreadCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyWorldTickCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyWorldTimeTotal, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyWorldTime, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyWorldTargetFps, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add system statistics */
    ecs_run(rows->world, PromHttpReplySystemTimeSpent, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplySystemTicks, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplySystemEntitiesMatched, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplySystemTablesMatched, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add system memory statistics */
    ecs_run(rows->world, PromHttpReplySystemMemoryUsed, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplySystemMemoryAllocd, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add component statistics */
    ecs_run(rows->world, PromHttpReplyCompTableCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyCompEntityCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyCompMemoryUsed, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(rows->world, PromHttpReplyCompMemoryAllocd, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");        
}

void PrometheusHttpImport(
    ecs_world_t *world,
    int flags)
{
    ECS_MODULE(world, PrometheusHttp);

    /* -- Allocation metrics -- */

    ECS_SYSTEM(world, PromHttpReplyAllocCount,         EcsManual, [in] EcsAllocStats);

    /* -- World metrics -- */

    ECS_SYSTEM(world, PromHttpReplyWorldTableCount,     EcsManual, [in] EcsWorldStats);
    ECS_SYSTEM(world, PromHttpReplyWorldComponentCount, EcsManual, [in] EcsWorldStats);
    ECS_SYSTEM(world, PromHttpReplyWorldEntityCount,    EcsManual, [in] EcsWorldStats);
    ECS_SYSTEM(world, PromHttpReplyWorldThreadCount,    EcsManual, [in] EcsWorldStats);
    ECS_SYSTEM(world, PromHttpReplyWorldTickCount,      EcsManual, [in] EcsWorldStats);
    ECS_SYSTEM(world, PromHttpReplyWorldTimeTotal,      EcsManual, [in] EcsWorldStats, PromWorldCounters);
    ECS_SYSTEM(world, PromHttpReplyWorldTime,           EcsManual, [in] EcsWorldStats);
    ECS_SYSTEM(world, PromHttpReplyWorldTargetFps,      EcsManual, [in] EcsWorldStats);

    /* -- System metrics -- */

    ECS_SYSTEM(world, PromHttpReplySystemTimeSpent,       EcsManual, [in] EcsSystemStats, PromSystemCounters);
    ECS_SYSTEM(world, PromHttpReplySystemTicks,           EcsManual, [in] EcsSystemStats, PromSystemCounters);
    ECS_SYSTEM(world, PromHttpReplySystemEntitiesMatched, EcsManual, [in] EcsSystemStats);    
    ECS_SYSTEM(world, PromHttpReplySystemTablesMatched,   EcsManual, [in] EcsSystemStats);

    /* -- System memory metrics -- */

    ECS_SYSTEM(world, PromHttpReplySystemMemoryUsed,  EcsManual, [in] EcsSystemStats, [in] EcsColSystemMemoryStats);
    ECS_SYSTEM(world, PromHttpReplySystemMemoryAllocd, EcsManual, [in] EcsSystemStats, [in] EcsColSystemMemoryStats);
    
    /* -- Component metrics -- */

    ECS_SYSTEM(world, PromHttpReplyCompTableCount,   EcsManual, [in] EcsComponentStats);
    ECS_SYSTEM(world, PromHttpReplyCompEntityCount,  EcsManual, [in] EcsComponentStats);
    ECS_SYSTEM(world, PromHttpReplyCompMemoryUsed,   EcsManual, [in] EcsComponentStats);
    ECS_SYSTEM(world, PromHttpReplyCompMemoryAllocd, EcsManual, [in] EcsComponentStats);

    /* -- Construct HTTP reply for Prometheus scrape -- */

    ECS_SYSTEM(world, PromHttpReply, EcsManual,
        .PromHttpReplyAllocCount,

        .PromHttpReplyWorldTableCount,
        .PromHttpReplyWorldComponentCount,
        .PromHttpReplyWorldEntityCount,
        .PromHttpReplyWorldThreadCount,
        .PromHttpReplyWorldTickCount,
        .PromHttpReplyWorldTimeTotal,
        .PromHttpReplyWorldTime,
        .PromHttpReplyWorldTargetFps,

        .PromHttpReplySystemTimeSpent,
        .PromHttpReplySystemTicks,
        .PromHttpReplySystemEntitiesMatched,
        .PromHttpReplySystemTablesMatched,

        .PromHttpReplySystemMemoryUsed,
        .PromHttpReplySystemMemoryAllocd,
        
        .PromHttpReplyCompTableCount,
        .PromHttpReplyCompEntityCount,
        .PromHttpReplyCompMemoryUsed,
        .PromHttpReplyCompMemoryAllocd);

    ECS_EXPORT_ENTITY(PromHttpReply);
}
