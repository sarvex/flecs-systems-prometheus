#include <flecs_systems_prometheus.h>

typedef struct PromWorldCounters {
    double frame_total;
    double system_total;
    double merge_total;
} PromWorldCounters;

typedef struct PromSystemCounters {
    double time_spent;
    double ticks;
} PromSystemCounters;

typedef struct PrometheusCollect {
    ECS_DECLARE_COMPONENT(PromWorldCounters);
    ECS_DECLARE_COMPONENT(PromSystemCounters);
} PrometheusCollect;

void PrometheusCollectImport(
    ecs_world_t *world,
    int flags);

#define PrometheusCollectImportHandles(handles) \
    ECS_IMPORT_COMPONENT(handles, PromWorldCounters);\
    ECS_IMPORT_COMPONENT(handles, PromSystemCounters);
