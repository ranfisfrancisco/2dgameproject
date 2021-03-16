#include "simple_logger.h"
#include "pickup.h"
#include "player.h"
#include "camera.h"

void pickup_update(Entity* self);
void container_hurt(Entity* self, int damage);

Entity* pickup_spawn(Vector2D position, enum enemy_type type) {
	Entity* ent;

	ent = entity_new(type);
	if (!ent) {
		slog("failed to create entity for pickup");
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
	else if (type == INTERACTABLE_BOX) {
		ent->sprite = gf2d_sprite_load_image("images/box.png");
	}
	else {
		slog("Attempted to spawn pickup as non-pickup type");
		entity_free(ent);
		return NULL;
	}

	vector2d_copy(ent->drawPosition, position);
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;

	if (type == PICKUP_TYPE_KNIFE)
		ent->scale = vector2d(1.0/12, 1.0/12);
	else if (type == PICKUP_TYPE_CROWBAR)
		ent->scale = vector2d(0.5, 0.5);
	else if (type == INTERACTABLE_BOX)
		ent->scale = vector2d(3, 3);
	else
		ent->scale = vector2d(1, 1);

	if (type == PICKUP_TYPE_FMEDKIT) {
		ent->colorShift = vector4d(100, 100, 100, 255);
	}
	else {
		ent->colorShift = vector4d(255, 255, 255, 255);
	}

	ent->update = pickup_update;
	if (type == INTERACTABLE_BOX) {
		ent->hurt = container_hurt;
	}
	gfc_rect_set(ent->hurtbox, ent->drawPosition.x, ent->drawPosition.y, ent->sprite->frame_w * ent->scale.x, ent->sprite->frame_h * ent->scale.y);

	return ent;
}

void pickup_set_hurtbox(Entity* self) {
	Vector2D cameraOffset;

	cameraOffset = camera_get_offset();

	gfc_rect_set(self->hurtbox, self->drawPosition.x + cameraOffset.x, self->drawPosition.y + cameraOffset.y, self->sprite->frame_w * self->scale.x, self->sprite->frame_h * self->scale.y);
}

void pickup_update(Entity* self) {
	pickup_set_hurtbox(self);

	if ((self->type == PICKUP_TYPE_KNIFE || self->type == PICKUP_TYPE_CROWBAR) && self->state == 1) {
		vector2d_copy(self->drawPosition, player_get_weapon_position());
		self->statePos++;
		if (self->statePos > 60 * 60)
			entity_free(self);

		if (self->health <= 0)
			entity_free(self);

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
			player_change_health(-5);
			entity_free(self);
		}
		else if (self->type == PICKUP_TYPE_POWERUP) {
			player_power_up(60 * 20);
			entity_free(self);
		}
		else if (self->type == PICKUP_TYPE_KNIFE || self->type == PICKUP_TYPE_CROWBAR) {
			self->state = 1;
			player_attach_weapon(self);
		}
	}
}

void container_hurt(Entity* self, int damage){
	self->health -= damage;

	if (self->health <= 0) {
		pickup_spawn(self->drawPosition, PICKUP_TYPE_MEDKIT);
		entity_free(self);
	}
}