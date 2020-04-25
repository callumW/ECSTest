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
    if (!hidden) {
        if (tex) {
            SDL_RenderCopyEx(g_renderer, tex, &tex_src, &draw_dest, texture_rotation, &rot_pivot,
                             SDL_FLIP_NONE);
        }

        for (auto& child : children) {
            child->render();
        }
    }
}

void Entity::update(float delta)
{
    apply_forces(delta);
    for (auto& child : children) {
        child->update(delta);
    }
}

void Entity::set_world_pos(vec2f_t const& new_pos)
{
    // world_pos = new_pos;
    // if (parent != nullptr) {
    //     local_pos = world_pos - parent->world_pos;
    // }
    // else {
    //     local_pos = world_pos;
    // }
    // update_render_position();
    // update_children_positions();

    // work out new local pos
    // set local pos

    if (parent != nullptr) {
        set_local_pos(new_pos - parent->world_pos);
    }
    else {
        set_local_pos(new_pos);
    }
}

void Entity::set_local_pos(vec2f_t const& new_pos)
{
    local_pos = new_pos;

    if (parent != nullptr) {
        relative_rotation = parent->local_pos.angle(local_pos);
        std::cout << "Relative rotation: " << relative_rotation << std::endl;
        texture_rotation = parent->texture_rotation + relative_rotation;
        std::cout << "local pos rotated: " << local_pos.get_rotated(relative_rotation);
        world_pos = parent->world_pos + local_pos.get_rotated(relative_rotation);
    }
    else {
        relative_rotation = new_pos.angle();
        texture_rotation = relative_rotation;
        world_pos = local_pos;
    }

    // Need to recalculate rotation
    update_render_position();
    update_children_positions();
}

void Entity::set_rotation(double new_rot) { texture_rotation = new_rot; }

void Entity::add_child(Entity* node)
{
    if (node) {
        children.push_back(node);
        node->parent = this;
        node->set_local_pos(node->local_pos);
    }
}

void Entity::update_render_position()
{
    std::cout << "Update render pos from " << draw_dest.x << ", " << draw_dest.y << " -> ";
    draw_dest.x = static_cast<int>(world_pos.x) - (tex_src.w / 2);
    draw_dest.y = static_cast<int>(world_pos.y) - (tex_src.h / 2);
    std::cout << draw_dest.x << ", " << draw_dest.y << std::endl;
}

void Entity::apply_forces(float const delta_time)
{
    if (force.length() != 0.0f) {
        vec2f_t new_pos = world_pos + (force * delta_time);
        set_world_pos(new_pos);
    }
}

void Entity::update_children_positions()
{
    for (auto& child : children) {
        child->update_relative_position();
    }
}

void Entity::update_relative_position() { set_local_pos(local_pos); }

void Entity::remove_child(Entity* node)
{
    if (node == nullptr) {
        return;
    }

    auto end = children.end();
    for (auto it = children.begin(); it != end; it++) {
        if (*it == node) {
            children.erase(it);
            (*it)->parent = nullptr;
            return;
        }
    }
}

void Entity::rotate_children()
{
    for (auto& child : children) {
        // local position remains constant... (well it's length does anyway)
    }
}
