#include "simple_logger.h"
#include "pickup.h"
#include "player.h"
#include "camera.h"

void pickup_update(Entity* self);
void container_hurt(Entity* self, int damage);
void explosive_hurt(Entity* self, int damage);
void pickup_set_hurtbox(Entity* self);
void pickup_die(Entity* self);

Entity* pickup_spawn(Vector2D position, enum enemy_type type) {
	Entity* ent;

	if (!entity_is_pickup(type) && !entity_is_interactable(type)) {
		slog("Attempted to spawn pickup/interactable as non-pickup/interactable type");
		return NULL;
	}

	ent = entity_new(type);
	if (!ent) {
		slog("failed to create entity for pickup/interactable");
		return NULL;
	}

	if (type == PICKUP_TYPE_MEDKIT || type == PICKUP_TYPE_FMEDKIT) {
		ent->sprite = gf2d_sprite_load_image("images/medkit.png");
	}
	else if (type == PICKUP_TYPE_KNIFE) {
		ent->sprite = gf2d_sprite_load_image("images/knife.png");
	}
	else if (type == PICKUP_TYPE_CROWBAR) {
		ent->sprite = gf2d_sprite_load_image("images/crowbar.png");
	}
	else if (type == PICKUP_TYPE_POWERUP) {
		ent->sprite = gf2d_sprite_load_image("images/chicken.png");
	}
	else if (type == INTERACTABLE_BOX || type == INTERACTABLE_METAL_BOX) {
		ent->sprite = gf2d_sprite_load_image("images/box.png");
	}
	else if (type == INTERACTABLE_EXPLOSIVE) {
		ent->sprite = gf2d_sprite_load_image("images/explosive_barrel.png");
	}
	else if (type == INTERACTABLE_CAR) {
		ent->sprite = gf2d_sprite_load_image("images/car.png");
	}
	else if (type == INTERACTABLE_TRASH_CAN) {
		ent->sprite = gf2d_sprite_load_image("images/trash_can.png");
	}
	
	if (ent->sprite == NULL) {
		slog("Failed to load sprite for pickup/interactable!");
		entity_free(ent);
		return NULL;
	}

	if (type == PICKUP_TYPE_KNIFE)
		ent->scale = vector2d(1.0 / 12, 1.0 / 12);
	else if (type == PICKUP_TYPE_CROWBAR)
		ent->scale = vector2d(0.5, 0.5);
	else if (type == INTERACTABLE_BOX || type == INTERACTABLE_METAL_BOX)
		ent->scale = vector2d(3, 3);
	else if (type == INTERACTABLE_CAR) {
		ent->scale = vector2d(2, 2);
	}
	else if (type == INTERACTABLE_TRASH_CAN) {
		ent->scale = vector2d(2, 2);
	}
	else
		ent->scale = vector2d(1, 1);

	if (type == PICKUP_TYPE_FMEDKIT) {
		ent->defaultColorShift = vector4d(100, 100, 100, 255);
	}
	else if (type == INTERACTABLE_METAL_BOX) {
		ent->defaultColorShift = vector4d(50, 50, 50, 255);
	}
	else {
		ent->defaultColorShift = vector4d(255, 255, 255, 255);
	}

	ent->colorShift = ent->defaultColorShift;
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	vector2d_copy(ent->drawPosition, position);
	ent->update = pickup_update;

	if (type == INTERACTABLE_BOX) {
		ent->hurt = container_hurt;
	}
	else if (type == INTERACTABLE_EXPLOSIVE) {
		ent->hurt = explosive_hurt;
	}

	pickup_set_hurtbox(ent);

	return ent;
}

void pickup_set_hurtbox(Entity* self) {
	Vector2D cameraOffset;

	cameraOffset = camera_get_offset();

	gfc_rect_set(self->hurtbox, self->drawPosition.x + cameraOffset.x, self->drawPosition.y + cameraOffset.y, self->sprite->frame_w * self->scale.x, self->sprite->frame_h * self->scale.y);
}

void pickup_update(Entity* self) {
	pickup_set_hurtbox(self);

	if (entity_is_weapon(self->type) && self->state == 1) {
		vector2d_copy(self->drawPosition, player_get_weapon_position());
		self->statePos++;
		if (self->statePos > 60 * 60)
			entity_free(self);

		if (self->health <= 0)
			pickup_die(self);

		return;
	}

	if (player_collison_check(self->hurtbox)) {
		if (self->type == PICKUP_TYPE_MEDKIT) {
			if (player_get_max_health() - player_get_health() > 1) {
				player_change_health(30);
				entity_free(self);
			}
		}
		else if (self->type == PICKUP_TYPE_FMEDKIT) {
			player_change_health(-30);
			entity_free(self);
		}
		else if (self->type == PICKUP_TYPE_POWERUP) {
			player_power_up(60 * 20);
			entity_free(self);
		}
		else if (self->type == PICKUP_TYPE_KNIFE || self->type == PICKUP_TYPE_CROWBAR) {
			player_attach_weapon(self);
		}
	}
}

void container_hurt(Entity* self, int damage){
	if (self->type == INTERACTABLE_METAL_BOX)
		return;
	self->health -= damage;

	if (self->health <= 0) {
		pickup_spawn(self->drawPosition, PICKUP_TYPE_MEDKIT);
		pickup_die(self);
	}
}

void explosive_hurt(Entity* self, int damage) {
	self->health -= damage;

	if (self->health <= 0) {
		self->hurtbox.w *= 1.3;
		self->hurtbox.x -= 20;
		if (player_collison_check(self->hurtbox)) {
			player_change_health(-30);
		}
		entity_free(self);
	}
}

void pickup_die(Entity* self) {
	director_add_score(self->deathScore);
	entity_free(self);
	return;
}