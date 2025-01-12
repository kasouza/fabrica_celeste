#include "fabrica/world/raycast.h"
#include "fabrica/math/vec3f.h"
#include "fabrica/math/vec3i.h"
#include "fabrica/utils/face.h"
#include "fabrica/world/block.h"
#include "fabrica/world/world.h"

#include <float.h>
#include <math.h>
#include <stdint.h>

void fabrica_raycast(fabrica_World *world, const fabrica_Vec3F *pos,
                     const fabrica_Vec3F *dir, fabrica_RaycastHit *out_hit,
                     float max_length) {
    assert(world != NULL);
    assert(pos != NULL);
    assert(dir != NULL);
    assert(out_hit != NULL);

    out_hit->hit = false;

    fabrica_Vec3I block_pos = {
        .x = (int)pos->x,
        .y = (int)pos->y,
        .z = (int)pos->z,
    };

    float delta_dist_x = dir->x == 0 ? FLT_MAX : fabs(1 / dir->x);
    float delta_dist_y = dir->y == 0 ? FLT_MAX : fabs(1 / dir->y);
    float delta_dist_z = dir->z == 0 ? FLT_MAX : fabs(1 / dir->z);

    float side_dist_x;
    float side_dist_y;
    float side_dist_z;

    int32_t step_x;
    int32_t step_y;
    int32_t step_z;

    if (dir->x < 0) {
        side_dist_x = (pos->x - block_pos.x) * delta_dist_x;
        step_x = -1;
    } else {
        side_dist_x = (block_pos.x + 1 - pos->x) * delta_dist_x;
        step_x = 1;
    }

    if (dir->y < 0) {
        side_dist_y = (pos->y - block_pos.y) * delta_dist_y;
        step_y = -1;
    } else {
        side_dist_y = (block_pos.y + 1 - pos->y) * delta_dist_y;
        step_y = 1;
    }

    if (dir->z < 0) {
        side_dist_z = (pos->z - block_pos.z) * delta_dist_z;
        step_z = -1;
    } else {
        side_dist_z = (block_pos.z + 1 - pos->z) * delta_dist_z;
        step_z = 1;
    }

    fabrica_Face face = fabrica_Face_FRONT;
    float ray_length = 0;
    bool has_passed_through_block = false;

    for (int i = 0; i < FABRICA_RAYCAST_MAX_ITERATIONS; ++i) {
        if (ray_length > max_length) {
            return;
        }

        fabrica_Block *block = fabrica_world_get_block(world, &block_pos);
        if (block == NULL) {
            // If the ray is out of the bounds of the world and has
            // passed through blocks, stop iterating
            // But if the ray is out of the bound but has NOT passed through
            // any blocked, it means we are shooting it from a location outside
            // of the world and we want to keep iterating because it may intersect with
            // a block
            if (has_passed_through_block) {
                return;
            }
        } else {
            has_passed_through_block = true;

            const fabrica_BlockTypeInfo *info =
                fabrica_block_get_type_info(block->type);

            if (info->tangible) {
                out_hit->hit = true;
                out_hit->block_pos = block_pos;
                out_hit->face = face;

                fabrica_Vec3F ray;
                fabrica_vec3f_scale(dir, ray_length, &ray);
                fabrica_vec3f_add(pos, &ray, &out_hit->pos);

                return;
            }
        }

        if (side_dist_x < side_dist_y && side_dist_x < side_dist_z) {
            ray_length += side_dist_x;
            side_dist_x += delta_dist_x;
            block_pos.x += step_x;
            face = dir->x < 0 ? fabrica_Face_RIGHT : fabrica_Face_LEFT;

        } else if (side_dist_y < side_dist_x && side_dist_y < side_dist_z) {
            ray_length += side_dist_y;
            side_dist_y += delta_dist_y;
            block_pos.y += step_y;
            face = dir->y < 0 ? fabrica_Face_TOP : fabrica_Face_BOTTOM;

        } else {
            ray_length += side_dist_z;
            side_dist_z += delta_dist_z;
            block_pos.z += step_z;
            face = dir->z < 0 ? fabrica_Face_BACK : fabrica_Face_FRONT;
        }
    }
}
