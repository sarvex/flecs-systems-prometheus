#include "http.h"

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
        stats->malloc_count_total);
    ecs_strbuf_append(reply, "flecs_alloc_count{kind=\"calloc\"} %u\n", 
        stats->calloc_count_total); 
    ecs_strbuf_appendstr(reply, "\n");

    /* Separate metric for reallocs, since it is not an allocation that can be 
     * easily summed up with malloc and calloc */
    ecs_strbuf_appendstr(reply, "#HELP flecs_realloc_count Number of reallocations since application start\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_realloc_count counter\n");

    ecs_strbuf_append(reply, "flecs_realloc_count %u\n", 
        stats->realloc_count_total);
     ecs_strbuf_appendstr(reply, "\n");

    /* Separate metric for frees, since it is not something that can be summed
     * with allocations */
    ecs_strbuf_appendstr(reply, "#HELP flecs_free_count Number of frees since application start\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_free_count counter\n");

    ecs_strbuf_append(reply, "flecs_free_count %u\n", 
        stats->free_count_total);
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
    ecs_strbuf_append(reply, "%u\n", stats->tables_count);
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
    ecs_strbuf_append(reply, "%u\n", stats->components_count);
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
    ecs_strbuf_append(reply, "%u\n", stats->entities_count);
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
    ecs_strbuf_append(reply, "%u\n", stats->threads_count ? stats->threads_count : 1);
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
    ecs_strbuf_append(reply, "%u\n", stats->frame_count_total);
}

static
void PromHttpReplyWorldTimeTotal(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    ecs_strbuf_appendstr(reply, "#HELP flecs_world_time_total Time spent\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_world_time_total counter\n");

    ecs_strbuf_append(reply, "flecs_world_time_total{kind=\"frame\"} %f\n", 
        stats->frame_seconds_total);

    ecs_strbuf_append(reply, "flecs_world_time_total{kind=\"system\"} %f\n", 
        stats->system_seconds_total);

    ecs_strbuf_append(reply, "flecs_world_time_total{kind=\"merge\"} %f\n", 
        stats->merge_seconds_total);                
}

static
void PromHttpReplyWorldTime(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    ecs_strbuf_appendstr(reply, "#HELP flecs_world_time Time expired since world creation\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_world_time counter\n");

    world_metric_header(reply, "flecs_world_time");
    ecs_strbuf_append(reply, "%f\n", stats->world_seconds_total);
}

static
void PromHttpReplyWorldTargetFps(ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsWorldStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    ecs_strbuf_appendstr(reply, "#HELP flecs_world_target_fps Target frames per second\n");
    ecs_strbuf_appendstr(reply, "#TYPE flecs_world_target_fps counter\n");

    world_metric_header(reply, "flecs_world_target_fps");
    ecs_strbuf_append(reply, "%f\n", stats->target_fps_hz);
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
    ecs_strbuf_append(reply, "%s {system=\"%s\",phase=\"%s\",enabled=\"%s\",active=\"%s\", hidden=\"%s\"} ",
        metric_name, stats->name, system_kind_str(stats->kind),
        stats->is_enabled ? "true" : "false",
        stats->is_active ? "true" : "false",
        stats->is_hidden ? "true" : "false");
}

static
void PromHttpReplySystemTimeSpent(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_time_total Time spent in system\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_time_total counter\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_metric_header(reply, "flecs_system_time_total", &stats[i]);
        ecs_strbuf_append(reply, "%f\n", stats[i].seconds_total);
    }
}

static
void PromHttpReplySystemTicks(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsSystemStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_system_ticks_total Number of times the system has ticked\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_system_ticks_total counter\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        system_metric_header(reply, "flecs_system_ticks_total", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].invoke_count_total);
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
        ecs_strbuf_append(reply, "%u\n", stats[i].entities_matched_count);
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
        ecs_strbuf_append(reply, "%u\n", stats[i].tables_matched_count);
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
        metric_name, stats->name, system_kind_str(stats->kind), kind,
        stats->is_enabled ? "true" : "false",
        stats->is_active ? "true" : "false");
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
        ecs_strbuf_append(reply, "%u\n", memstats[i].base_memory_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_used", "other", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].other_memory_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_used", "columns", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].columns_memory.used_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_used", "active_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].active_tables_memory.used_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_used", "inactive_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].inactive_tables_memory.used_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_used", "jobs", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].jobs_memory.used_bytes);
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
        ecs_strbuf_append(reply, "%u\n", memstats[i].base_memory_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "other", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].other_memory_bytes);    

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "columns", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].columns_memory.allocd_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "active_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].active_tables_memory.allocd_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "inactive_tables", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].inactive_tables_memory.allocd_bytes);

        system_mem_metric_header(reply, "flecs_system_memory_allocated", "jobs", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", memstats[i].jobs_memory.allocd_bytes);
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
        metric_name, stats->name);
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
        ecs_strbuf_append(reply, "%u\n", stats[i].tables_count);
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
        ecs_strbuf_append(reply, "%u\n", stats[i].entities_count);
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
        ecs_strbuf_append(reply, "%u\n", stats[i].memory.used_bytes);
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
        ecs_strbuf_append(reply, "%u\n", stats[i].memory.allocd_bytes);
    }
}

/* -- Table metrics -- */

static
void table_metric_header(
    ecs_world_t *world,
    ecs_strbuf_t *reply,
    const char *metric_name,
    EcsTableStats *stats)
{
    char *type_expr = ecs_type_to_expr(world, stats->type);

    ecs_strbuf_append(reply, "%s {type=\"%s\"} ", metric_name, type_expr);

    free(type_expr);
}

static
void PromHttpReplyTableColumnCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_column_count Number of columns in table\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_column_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_column_count", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].columns_count);
    }
}

static
void PromHttpReplyTableEntityCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_entity_count Number of entities (rows) in table\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_entity_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_entity_count", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].rows_count);
    }
}

static
void PromHttpReplyTableSystemMatchedCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_systems_matched_count Number of systems matched with table\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_systems_matched_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_systems_matched_count", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].systems_matched_count);
    }
}

static
void PromHttpReplyTableComponentMemoryAllocd(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_component_memory_allocd Memory allocated for entity data\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_component_memory_allocd gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_component_memory_allocd", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].component_memory.allocd_bytes);
    }
}

static
void PromHttpReplyTableComponentMemoryUsed(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_component_memory_used Memory used for entity data\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_component_memory_used gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_component_memory_used", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].component_memory.used_bytes);
    }
}

static
void PromHttpReplyTableEntityMemoryAllocd(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_entity_memory_allocd Memory allocated for entity data\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_entity_memory_allocd gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_entity_memory_allocd", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].entity_memory.allocd_bytes);
    }
}

static
void PromHttpReplyTableEntityMemoryUsed(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_entity_memory_used Memory allocated for entity data\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_entity_memory_used gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_entity_memory_used", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].entity_memory.used_bytes);
    }
}

static
void PromHttpReplyTableOtherMemory(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTableStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_table_other_memory Memory used for other table data\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_table_other_memory gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        table_metric_header(world, reply, "flecs_table_other_memory", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].other_memory_bytes);
    }
}

/* -- Type metrics -- */

static
void type_metric_header(
    ecs_world_t *world,
    ecs_strbuf_t *reply,
    const char *metric_name,
    EcsTypeStats *stats)
{
    char *type_expr = ecs_type_to_expr(world, stats->type);

    ecs_strbuf_append(reply, "%s {id=\"%s\",type=\"%s\",hidden=\"%s\"} ", 
        metric_name, stats->name, type_expr, stats->is_hidden ? "true" : "false");

    free(type_expr);
}

static
void type_entity_metric_header(
    ecs_world_t *world,
    ecs_strbuf_t *reply,
    const char *metric_name,
    const char *kind,
    EcsTypeStats *stats)
{
    char *type_expr = ecs_type_to_expr(world, stats->type);

    ecs_strbuf_append(reply, "%s {id=\"%s\",type=\"%s\",kind=\"%s\",hidden=\"%s\"} ", 
        metric_name, stats->name, type_expr, kind, stats->is_hidden ? "true" : "false");

    free(type_expr);
}

static
void PromHttpReplyTypeEntityCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTypeStats, stats, 1);

    ecs_strbuf_t *reply = rows->param;    
    ecs_world_t *world = rows->world;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_type_entity_count Number of entities in type\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_type_entity_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        type_entity_metric_header(world, reply, "flecs_type_entity_count", "", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].entities_count);
        type_entity_metric_header(world, reply, "flecs_type_entity_count", "childof", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].entities_childof_count);
        type_entity_metric_header(world, reply, "flecs_type_entity_count", "instanceof", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].entities_instanceof_count);
        type_entity_metric_header(world, reply, "flecs_type_entity_count", "component", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].components_count);
        type_entity_metric_header(world, reply, "flecs_type_entity_count", "column_system", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].col_systems_count);
        type_entity_metric_header(world, reply, "flecs_type_entity_count", "row_system", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].row_systems_count);        
    }    
}

static
void PromHttpReplyTypeEnabledSystemCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTypeStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_type_enabled_system_count Number of enabled systems in type\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_type_enabled_system_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        type_metric_header(world, reply, "flecs_type_enabled_system_count", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].enabled_systems_count);
    }
}

static
void PromHttpReplyTypeActiveSystemCount(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTypeStats, stats, 1); 

    ecs_world_t *world = rows->world;
    ecs_strbuf_t *reply = rows->param;

    /* Write headers once per reply */
    if (rows->table_offset == 0) {
        ecs_strbuf_appendstr(reply, "#HELP flecs_type_active_system_count Number of active systems in type\n");
        ecs_strbuf_appendstr(reply, "#TYPE flecs_type_active_system_count gauge\n");
    }

    uint32_t i;
    for (i = 0; i < rows->count; i ++) {
        type_metric_header(world, reply, "flecs_type_active_system_count", &stats[i]);
        ecs_strbuf_append(reply, "%u\n", stats[i].active_systems_count);
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

    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableColumnCount, 20);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableEntityCount, 21);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableSystemMatchedCount, 22);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableComponentMemoryAllocd, 23);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableComponentMemoryUsed, 24);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableEntityMemoryAllocd, 25);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableEntityMemoryUsed, 26);    
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTableOtherMemory, 27);

    ECS_COLUMN_ENTITY(rows, PromHttpReplyTypeEntityCount, 28);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTypeEnabledSystemCount, 29);
    ECS_COLUMN_ENTITY(rows, PromHttpReplyTypeActiveSystemCount, 30);

    ecs_strbuf_t *reply = rows->param;
    ecs_world_t *world = rows->world;

    /* Add allocation statistics */
    ecs_run(world, PromHttpReplyAllocCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add world statistics */
    ecs_run(world, PromHttpReplyWorldTableCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyWorldComponentCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyWorldEntityCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyWorldThreadCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyWorldTickCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyWorldTimeTotal, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyWorldTime, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyWorldTargetFps, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add system statistics */
    ecs_run(world, PromHttpReplySystemTimeSpent, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplySystemTicks, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplySystemEntitiesMatched, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplySystemTablesMatched, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add system memory statistics */
    ecs_run(world, PromHttpReplySystemMemoryUsed, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplySystemMemoryAllocd, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add component statistics */
    ecs_run(world, PromHttpReplyCompTableCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyCompEntityCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyCompMemoryUsed, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyCompMemoryAllocd, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");     

    /* Add table statistics */
    ecs_run(world, PromHttpReplyTableColumnCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n"); 
    ecs_run(world, PromHttpReplyTableEntityCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n"); 
    ecs_run(world, PromHttpReplyTableSystemMatchedCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");      
    ecs_run(world, PromHttpReplyTableComponentMemoryAllocd, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyTableComponentMemoryUsed, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyTableEntityMemoryAllocd, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyTableEntityMemoryUsed, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");    
    ecs_run(world, PromHttpReplyTableOtherMemory, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");

    /* Add type statistics */
    ecs_run(world, PromHttpReplyTypeEntityCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyTypeEnabledSystemCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");
    ecs_run(world, PromHttpReplyTypeActiveSystemCount, 0, reply);
    ecs_strbuf_appendstr(reply, "\n");    
}

void PrometheusHttpImport(
    ecs_world_t *world,
    int flags)
{
    ECS_MODULE(world, PrometheusHttp);

    /* -- Allocation metrics -- */

    ECS_SYSTEM(world, PromHttpReplyAllocCount,         EcsManual, [in] EcsAllocStats, SYSTEM.EcsHidden);

    /* -- World metrics -- */

    ECS_SYSTEM(world, PromHttpReplyWorldTableCount,     EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyWorldComponentCount, EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyWorldEntityCount,    EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyWorldThreadCount,    EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyWorldTickCount,      EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyWorldTimeTotal,      EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyWorldTime,           EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyWorldTargetFps,      EcsManual, [in] EcsWorldStats, SYSTEM.EcsHidden);

    /* -- System metrics -- */

    ECS_SYSTEM(world, PromHttpReplySystemTimeSpent,       EcsManual, [in] EcsSystemStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplySystemTicks,           EcsManual, [in] EcsSystemStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplySystemEntitiesMatched, EcsManual, [in] EcsSystemStats, SYSTEM.EcsHidden);    
    ECS_SYSTEM(world, PromHttpReplySystemTablesMatched,   EcsManual, [in] EcsSystemStats, SYSTEM.EcsHidden);

    /* -- System memory metrics -- */

    ECS_SYSTEM(world, PromHttpReplySystemMemoryUsed,   EcsManual, [in] EcsSystemStats, [in] EcsColSystemMemoryStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplySystemMemoryAllocd, EcsManual, [in] EcsSystemStats, [in] EcsColSystemMemoryStats, SYSTEM.EcsHidden);
    
    /* -- Component metrics -- */

    ECS_SYSTEM(world, PromHttpReplyCompTableCount,   EcsManual, [in] EcsComponentStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyCompEntityCount,  EcsManual, [in] EcsComponentStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyCompMemoryUsed,   EcsManual, [in] EcsComponentStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyCompMemoryAllocd, EcsManual, [in] EcsComponentStats, SYSTEM.EcsHidden);

    /* -- Table metrics -- */

    ECS_SYSTEM(world, PromHttpReplyTableColumnCount,           EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTableEntityCount,           EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTableSystemMatchedCount,    EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTableComponentMemoryUsed,   EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTableComponentMemoryAllocd, EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTableEntityMemoryUsed,      EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTableEntityMemoryAllocd,    EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);    
    ECS_SYSTEM(world, PromHttpReplyTableOtherMemory,           EcsManual, [in] EcsTableStats, SYSTEM.EcsHidden);

    /* -- Type metrics -- */

    ECS_SYSTEM(world, PromHttpReplyTypeEntityCount,           EcsManual, [in] EcsTypeStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTypeEnabledSystemCount,    EcsManual, [in] EcsTypeStats, SYSTEM.EcsHidden);
    ECS_SYSTEM(world, PromHttpReplyTypeActiveSystemCount,     EcsManual, [in] EcsTypeStats, SYSTEM.EcsHidden);

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
        .PromHttpReplyCompMemoryAllocd,
        
        .PromHttpReplyTableColumnCount,
        .PromHttpReplyTableEntityCount,
        .PromHttpReplyTableSystemMatchedCount,
        .PromHttpReplyTableComponentMemoryUsed,
        .PromHttpReplyTableComponentMemoryAllocd,
        .PromHttpReplyTableEntityMemoryUsed,
        .PromHttpReplyTableEntityMemoryAllocd,        
        .PromHttpReplyTableOtherMemory,
        
        .PromHttpReplyTypeEntityCount,
        .PromHttpReplyTypeEnabledSystemCount,
        .PromHttpReplyTypeActiveSystemCount,
        
        SYSTEM.EcsHidden);

    ECS_EXPORT_ENTITY(PromHttpReply);
}
