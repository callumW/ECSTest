#include "Entity.h"
#include "Globals.h"
#include "loading_helpers.h"

#include <cassert>
#include <cmath>
#include <iostream>

Entity::Entity()
{
    tex = load_default_texture();

    tex_src.w = 40;
    tex_src.h = 40;

    draw_dest.w = 40;
    draw_dest.h = 40;

    rot_pivot.x = tex_src.w / 2;
    rot_pivot.y = tex_src.h / 2;
}

Entity::Entity(std::string const& texture_path, SDL_Rect const& src)
{
    tex = load_texture(texture_path);
    tex_src = src;

    draw_dest.w = tex_src.w;
    draw_dest.h = tex_src.h;

    rot_pivot.x = tex_src.w / 2;
    rot_pivot.y = tex_src.h / 2;
}

void Entity::render()
{
    if (!is_hidden) {
        if (tex) {
            draw_dest.x = static_cast<int>(world_transform.position.x) - rot_pivot.x;
            draw_dest.y = static_cast<int>(world_transform.position.y) - rot_pivot.y;
            SDL_RenderCopyEx(g_renderer, tex, &tex_src, &draw_dest,
                             static_cast<double>(world_transform.rotation), &rot_pivot,
                             SDL_FLIP_NONE);
        }
    }
    SceneNode::render();
}

void Entity::update(float delta)
{
    apply_forces(delta);
    SceneNode::update(delta);
}


void Entity::apply_forces(float const delta_time)
{
    if (force.length() != 0.0f) {
        world_transform.position = world_transform.position + (force * delta_time);
        recalc_local_transform();
    }
}

void Entity::face(Entity const& entity)
{
    float new_rotation = (entity.world_transform.position - world_transform.position).angle();
    world_transform.rotation = new_rotation;
    recalc_local_transform();
}

void Entity::face(vec2f_t const& vec)
{
    float new_rotation = (vec - world_transform.position).angle();
    world_transform.rotation = new_rotation;
    recalc_local_transform();
}
