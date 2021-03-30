#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

typedef enum entity_type { NO_TYPE, PLAYER_TYPE,
ENEMY_TYPE_1, ENEMY_TYPE_2, ENEMY_TYPE_3, ENEMY_TYPE_4, ENEMY_TYPE_5, 
BOSS_TYPE_1, BOSS_TYPE_2,
PICKUP_TYPE_MEDKIT, PICKUP_TYPE_FMEDKIT, PICKUP_TYPE_KNIFE, PICKUP_TYPE_CROWBAR, PICKUP_TYPE_POWERUP,
INTERACTABLE_BOX, INTERACTABLE_METAL_BOX, INTERACTABLE_EXPLOSIVE, INTERACTABLE_CAR, INTERACTABLE_TRASH_CAN };

enum facing_side { FACE_RIGHT, FACE_LEFT };

#define NUM_OF_ENTITY_TYPES 19

typedef struct Entity_s{
	Bool _inuse;
	Vector2D drawPosition;
	Vector2D realPosition;
	Sprite *sprite;
	Vector2D scale;
	SDL_Rect hurtbox;
	int attackHit;
	int baseDamage;
	float frame;
	Vector3D rotation;
	Vector4D colorShift;
	Vector4D defaultColorShift;
	void (*update)(struct Entity_s* self);
	void (*think)(struct Entity_s* self);
	void (*draw)(struct Entity_s* self);
	void (*free)(struct Entity_s* self);
	void (*hurt)(struct Entity_s* self, int damage);
	void* data;
	enum entity_type type;
	int side;
	Vector2D flip;
	int state;
	int statePos;
	int health;
	int maxHealth;
	int defaultSpeed;
	int speed;
	int deathScore;
} Entity;

/**
 * @brief Initialize entity manager. Must be done before using any other entity manager constructs.
 * @param max_entitites The max number of entitities the entity manager can support
 */
void entity_manager_init(Uint32 max_entities);

/**
 * @brief Set all bytes in the entity manager to 0, erasing all entities.
 */
void entity_manager_reset_all();

/**
 * @brief Free the memory used by the entity manager
 */
void entity_manager_free();

/**
 * @brief Return the number of entities in use of a given type.
 * @param type The type of entity to be counted
 * @return The population count for that type
 */
int entity_get_population(enum entity_type type);

/**
 * @brief Return the number of enemy entities in use.
 * @return The number of enemy entities.
 */
int entity_get_enemy_population();

/**
 * @brief Get the position of the center of the enemy.
 * @param ent The entity whose real position you want
 * @return The position as a Vector2D
 */
Vector2D entity_real_position(Entity* ent);

/**
 * @brief Calls update on all entities
 */
void entity_manager_update_entities();

/**
 * @brief Calls think on all entities
 */
void entity_manager_think_entities();

/**
 * @brief Calls draw on all entities
 */
void entity_manager_draw_entities();

/**
 * @brief Checks an attack hitbox against relevant entity's hurtboxes
 * @param playerHitBox The SDL_RECT representing the hitbox
 * @param damage The amount of damage for the enemy to take
 * @return The number of entities that were hit
 */
int entity_manager_player_attack_collison(SDL_Rect playerHitbox, int damage);

/**
 * @brief Check if an interactable's hurtbox collides with the given hurtbox
 * @param hurtbox The hurtbox to be checked against
 * @return The number of collisions
 */
int entity_manager_player_interactable_collision(SDL_Rect hurtbox);

/**
 * @brief Create a new entity of a given type. Do not use this to spawn new entities directly. Use enemy_spawn for enemies in enemy.h, etc.
 * @param type The entity type to be created
 * @return The pointer to the new entity
 */
Entity* entity_new(enum entity_type type);

/**
 * @brief Free the given entity
 * @param ent The entity to be freed
 */
void entity_free(Entity *ent);

/**
 * @brief Draw the given entity
 * @param ent The entity to be drawn
 */
void entity_draw(Entity *ent);

/**
 * @brief Returns whether or not the given entity is a player entity
 * @param type The type to be checked
 * @return 1 if the entity is a player, 0 otherwise
 */
int entity_is_player(enum entity_type type);

/**
 * @brief Returns whether or not the given entity is an enemy entity
 * @param type The type to be checked
 * @return 1 if the entity is an enemy, 0 otherwise
 */
int entity_is_enemy(enum entity_type type);

/**
 * @brief Returns whether or not the given entity is a boss entity
 * @param type The type to be checked
 * @return 1 if the entity is an enemy, 0 otherwise
 */
int entity_is_boss(enum entity_type type);

/**
 * @brief Returns whether or not the given entity is a pickup entity
 * @param type The type to be checked
 * @return 1 if the entity is a pickup, 0 otherwise
 */
int entity_is_pickup(enum entity_type type);

/**
 * @brief Returns whether or not the given entity is a weapon entity
 * @param type The type to be checked
 * @return 1 if the entity is a weapon, 0 otherwise
 */
int entity_is_weapon(enum entity_type type);

/**
 * @brief Returns whether or not the given entity is an interactable entity
 * @param type The type to be checked
 * @return 1 if the entity is an interactable, 0 otherwise
 */
int entity_is_interactable(enum entity_type type);

/**
 * @brief Returns whether or not the given entity is an interactable object
 * @param type The type to be checked
 * @return 1 if the entity is an object, 0 otherwise
 */
int entity_is_object(enum entity_type type);

/**
 * @brief Draws hurtboxes for all entities.
 */
void entity_debug_draw_hurtboxes();

#endif