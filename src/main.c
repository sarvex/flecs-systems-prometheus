#include <flecs_systems_prometheus.h>
#include "http.h"

typedef struct http_metrics_t {
    ecs_entity_t PromHttpReply;
} http_metrics_t;

http_metrics_t* http_metrics_ctx(
    ecs_entity_t PromHttpReply)
{
    http_metrics_t *result = ecs_os_malloc(sizeof(http_metrics_t));
    result->PromHttpReply = PromHttpReply;
    return result;
}

static
bool http_metrics_action(
    ecs_world_t *world,
    ecs_entity_t entity,
    EcsHttpEndpoint *endpoint,
    EcsHttpRequest *request,
    EcsHttpReply *reply) 
{
    ecs_strbuf_t reply_body = ECS_STRBUF_INIT;
    http_metrics_t *ctx = endpoint->ctx;

    ecs_run(world, ctx->PromHttpReply, 0, &reply_body);

    reply->body = ecs_strbuf_get(&reply_body);

    return true;
}

static
void PromStart(ecs_rows_t *rows) {
    ecs_world_t *world = rows->world;

    ECS_COLUMN(rows, EcsPrometheus, service, 1);
    ECS_IMPORT_COLUMN(rows, FlecsComponentsHttp, 2);
    ECS_COLUMN_ENTITY(rows, PromHttpReply, 3);

    int i;
    for (i = 0; i < rows->count; i ++) {
        /* Create HTTP server instance for port */
        ecs_entity_t server = rows->entities[i];

        ecs_set(world, server, EcsHttpServer, {.port = service[i].port});
        
        /* Create metrics endpoint */
        ecs_entity_t e_metrics = ecs_new_child(world, server, 0);
        ecs_set(world, e_metrics, EcsHttpEndpoint, {
            .url = "metrics",
            .action = http_metrics_action,
            .synchronous = true,
            .ctx = http_metrics_ctx(PromHttpReply)
        });

        ecs_os_log("prometheus: service running on :%u", service[i].port);
    }
}

void FlecsSystemsPrometheusImport(
    ecs_world_t *world,
    int flags)
{
    /* Public imports */
    ECS_IMPORT(world, FlecsStats, 0);
    ECS_IMPORT(world, FlecsComponentsHttp, 0);

    /* Private imports */
    ECS_IMPORT(world, PrometheusHttp, 0);

    /* Self */
    ECS_MODULE(world, FlecsSystemsPrometheus);

    ECS_COMPONENT(world, EcsPrometheus);

    ECS_SYSTEM(world, PromStart, EcsOnSet, 
        EcsPrometheus, 
        $.FlecsComponentsHttp,
         .PromHttpReply);
    
    ECS_EXPORT_COMPONENT(EcsPrometheus);
}