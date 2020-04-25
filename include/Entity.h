#ifndef ENTITY_H
#define ENTITY_H
#include <SDL2/SDL.h>

#include <string>
#include <vector>

#include "game_math.h"

class Entity {
public:
    Entity();

    Entity(std::string const& texture_path, SDL_Rect const& src);

    virtual void render();
    virtual void update(float delta);

    virtual void set_world_pos(vec2f_t const& new_pos);

    virtual void set_local_pos(vec2f_t const& new_pos);

    virtual void set_relative_rotation(float new_rot);

    virtual void set_hidden(bool hide = true) { hidden = hide; }

    virtual void set_force(vec2f_t new_force) { force = new_force; }
    virtual void add_force(vec2f_t add) { force += add; }

    virtual void add_child(Entity* node);

    virtual void remove_child(Entity* node);

    virtual void face(Entity const& entity);
    virtual void face(vec2f_t const& vec);

    bool is_hidden() const { return hidden; }

    vec2f_t world_position() const { return world_pos; }
    vec2f_t local_position() const { return local_pos; }

    double screen_rotation() const { return texture_rotation; }
    double local_rotation() const { return relative_rotation; }

protected:
    void update_render_position();
    void apply_forces(float const delta_time);

    void update_children_positions();
    void update_relative_position();

    void rotate_children();

    std::vector<Entity*> children;
    Entity* parent = nullptr;

    vec2f_t world_pos;
    vec2f_t local_pos;
    double texture_rotation = 0.0f; // rotation in degrees
    float relative_rotation = 0.0f;
    bool hidden = false;

    SDL_Texture* tex = nullptr;
    SDL_Rect tex_src = {0};
    SDL_Rect draw_dest = {0};
    SDL_Point rot_pivot = {0};

    vec2f_t force = {0.0f, 0.0f};
};
#endif
