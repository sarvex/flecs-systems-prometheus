#include <flecs_systems_prometheus.h>

typedef struct PrometheusHttp {
    ECS_DECLARE_ENTITY(PromHttpReply);
} PrometheusHttp;

void PrometheusHttpImport(
    ecs_world_t *world,
    int flags);

#define PrometheusHttpImportHandles(handles) \
    ECS_IMPORT_ENTITY(handles, PromHttpReply)
