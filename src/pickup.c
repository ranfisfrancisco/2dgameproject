#include "simple_logger.h"
#include "pickup.h"
#include "player.h"

void pickup_update(Entity* self);

Entity* pickup_spawn(Vector2D position, enum enemy_type type) {
	Entity* ent;

	ent = entity_new();
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
	else {
		slog("Attempted to spawn pickup as non-pickup type");
		entity_free(ent);
		return NULL;
	}

	vector2d_copy(ent->drawPosition, position);
	ent->type = type;

	if (type == PICKUP_TYPE_KNIFE)
		ent->scale = vector2d(1.0/12, 1.0/12);
	else
		ent->scale = vector2d(1, 1);

	if (type == PICKUP_TYPE_MEDKIT)
		ent->colorShift = vector4d(255, 255, 255, 255);
	else if (type == PICKUP_TYPE_FMEDKIT) {
		ent->colorShift = vector4d(100, 100, 100, 255);
	}
	else if (type == PICKUP_TYPE_KNIFE) {
		ent->colorShift = vector4d(255, 255, 255, 255);
	}

	ent->update = pickup_update;
	gfc_rect_set(ent->hurtbox, ent->drawPosition.x, ent->drawPosition.y, ent->sprite->frame_w * ent->scale.x, ent->sprite->frame_h * ent->scale.y);

	return ent;
}

void pickup_update(Entity* self) {
	gfc_rect_set(self->hurtbox, self->drawPosition.x, self->drawPosition.y, self->sprite->frame_w * self->scale.x, self->sprite->frame_h * self->scale.y);

	if (self->type == PICKUP_TYPE_KNIFE && self->state == 1) {
		vector2d_copy(self->drawPosition, player_get_weapon_position());
		return;
	}

	if (player_collison_check(self->hurtbox)) {
		if (self->type == PICKUP_TYPE_MEDKIT) {
			player_change_health(30);
			entity_free(self);
		}
		else if (self->type == PICKUP_TYPE_FMEDKIT) {
			player_change_health(-5);
			entity_free(self);
		}
		else if (self->type == PICKUP_TYPE_KNIFE) {
			self->state = 1;
		}
	}
}