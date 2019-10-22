#ifndef FLECS_SYSTEMS_PROMETHEUS_H
#define FLECS_SYSTEMS_PROMETHEUS_H

/* This generated file contains includes for project dependencies */
#include "flecs-systems-prometheus/bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsPrometheus {
    uint16_t port;
} EcsPrometheus;

typedef struct FlecsSystemsPrometheus {
    ECS_DECLARE_COMPONENT(EcsPrometheus);
} FlecsSystemsPrometheus;

void FlecsSystemsPrometheusImport(
    ecs_world_t *world,
    int flags);

#define FlecsSystemsPrometheusImportHandles(handles) \
    ECS_IMPORT_COMPONENT(handles, EcsPrometheus);

#ifdef __cplusplus
}
#endif

#endif
