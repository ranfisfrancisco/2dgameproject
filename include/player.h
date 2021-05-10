#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "gfc_vector.h"
#include "gfc_types.h"
#include "gfc_audio.h"

enum player_state { PLAYER_IDLE, PLAYER_HURT, PLAYER_WALK, PLAYER_PUNCH, PLAYER_KICK, PLAYER_QCFP, PLAYER_QCFK, PLAYER_BFP, PLAYER_BFK, PLAYER_PK };

typedef struct {
	Entity* weapon;
	int powerUpTime;
	Sound* attackSound;
	Sound* hurtSound;
	int hitStreak;
}PlayerData;

/**
 * @brief Spawns a player
 * @param position The position the player will spawn at.
 */
void player_spawn(Vector2D position);

/**
 * @brief Returns an entity with player properties
 * @param position The position the entity will spawn at.
 */
Entity* player_allocate_entity(Vector2D position);

/**
 * @brief Return the position of the player
 * @return  Vector2D with the position of the player
 */
Vector2D player_get_position();

/**
 * @brief Returns the player's health
 * @return Integer indicating the player's health
 */
int player_get_health();

/**
 * @brief Returns the player's max health
 * @return Integer indicating the player's max health
 */
int player_get_max_health();

/**
 * @brief Attach a weapon entity to the player
 * @param ent The weapon type entity to attach to the player
 */
void player_attach_weapon(Entity* ent);

/**
 * @brief Get the position to draw the weapon at.
 * @return Vector2D indicating the position to draw the weapon at.
 */
Vector2D player_get_weapon_position();

/**
 * @brief Frees and discards held weapon.
 */
void player_discard_weapon();

/**
 * @brief Puts the player in a power up state
 * @param frameTime The number of frames the player should be powered up.
 */
void player_power_up(int frameTime);

/**
 * @brief Checks if a given rect collides with the player hurtbox
 * @return 1 if there is a collision, 0 otherwise
 */
int player_collison_check(SDL_Rect rect);

/**
 * @brief Changes the player's health
 * @param amount The amount to change by. Negative integers deecrease health
 */
void player_change_health(int amount);

void player_hurt(Entity* self, int damage);

#endif