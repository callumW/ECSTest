#include "Game.h"
#include "EntityManager.h"
#include "Globals.h"
#include "PhysicsManager.h"
#include "RenderManager.h"
#include "TimingSystem.h"
#include "UISystem.h"
#include "game_math.h"
#include "input.h"
#include "loading_helpers.h"

#include <cmath>
#include <iostream>

float const PLAYER_MOVEMENT_SPEED = 250.0f;
float const PLAYER_BULLET_SPEED = 1000.0f;
float const PLAYER_FIRE_RATE = 1.0f / 10.0f;


Game::Game()
{
    auto player_pair = EntityManager::get().new_entity();
    assert(player_pair.second != nullptr);
    m_player_id = player_pair.first;

    player_pair.second->components |= RENDER;

    auto render_comp = RenderManager::get().new_render_component();

    render_comp->owner_id = m_player_id;
    render_comp->texture = load_texture("./assets/G.bmp");
    render_comp->dst_rect.w = 40;
    render_comp->dst_rect.h = 40;
    render_comp->src_rect = render_comp->dst_rect;
    render_comp->pivot_point = {render_comp->src_rect.w / 2, render_comp->src_rect.h / 2};

    UISystem::get().add_text({0, 0, 160, 32}, "Terminal Reboot");
}

Game::~Game() {}

void Game::render()
{
    RenderManager::get().render_all();
    UISystem::get().render();
}

void Game::update(Uint32 delta)
{
    float delta_f = static_cast<float>(delta) / 1000.0f;

    TimingSystem::get().update(delta_f);

    update_player(delta_f);

    PhysicsManager::get().simulate(delta_f);

    EntityManager::get().update_dirty_entities(delta_f);
}

void Game::update_player(float delta)
{
    auto player = EntityManager::get().find_entity(m_player_id);
    assert(player != nullptr);
    if (INPUT.KEY_W) {
        player->world_transform.position.y -= PLAYER_MOVEMENT_SPEED * delta;
    }
    else if (INPUT.KEY_S) {
        player->world_transform.position.y += PLAYER_MOVEMENT_SPEED * delta;
    }

    if (INPUT.KEY_A) {
        player->world_transform.position.x -= PLAYER_MOVEMENT_SPEED * delta;
    }
    else if (INPUT.KEY_D) {
        player->world_transform.position.x += PLAYER_MOVEMENT_SPEED * delta;
    }

    vec2f_t mouse_pos = {static_cast<float>(INPUT.mouse_x), static_cast<float>(INPUT.mouse_y)};

    player->face(mouse_pos);

    static float time_since_last_fire = 99999.0f;

    if (INPUT.mouse_left_click && time_since_last_fire > PLAYER_FIRE_RATE) {
        time_since_last_fire = 0.0f;
        vec2f_t const force =
            (mouse_pos - player->world_transform.position).normalised() * PLAYER_BULLET_SPEED;

        fire_bullet(player->world_transform.position, force);
    }
    else {
        time_since_last_fire += delta;
    }
}

void Game::fire_bullet(vec2f_t const& loc, vec2f_t const& force)
{
    auto bullet_pair = EntityManager::get().new_entity();

    auto bullet_ent = bullet_pair.second;
    auto bullet_id = bullet_pair.first;

    assert(bullet_ent != nullptr);

    bullet_ent->components |= RENDER | PHYSICS;

    // need physics
    // needs render component

    auto render_comp = RenderManager::get().new_render_component();
    render_comp->owner_id = bullet_id;
    render_comp->texture = load_texture("./assets/bullet.bmp");
    render_comp->src_rect.w = 5;
    render_comp->src_rect.h = 5;
    render_comp->dst_rect = render_comp->src_rect;
    render_comp->dst_rect.x = static_cast<int>(loc.x) - 2;
    render_comp->dst_rect.y = static_cast<int>(loc.y) - 2;
    render_comp->pivot_point = {2, 2};

    auto physics_comp = PhysicsManager::get().new_physics_component(bullet_id);
    physics_comp->position = loc;
    physics_comp->net_force = force;

    TimingSystem::timer_task_t remove_task{
        1.0f,
        [=](float delta, entity_id_t const& id) { EntityManager::get().schedule_destruction(id); },
        bullet_id};

    TimingSystem::get().schedule_task(remove_task);
}

void Game::cleanup()
{
    EntityManager::get().clear_dead_entities();
    EntityManager::get().clear_moved_entities();

    EntityManager::get().process_dead_entities();

    PhysicsManager::get().cleanup();
    RenderManager::get().cleanup();
    TimingSystem::get().cleanup();
}
