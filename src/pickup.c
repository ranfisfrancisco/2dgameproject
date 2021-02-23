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
	if (type == PICKUP_TYPE_1) {
		ent->sprite = gf2d_sprite_load_image("images/medkit.png");
	}
	else {
		slog("Attempted to spawn pickup as non-pickup type");
		entity_free(ent);
		return NULL;
	}

	vector2d_copy(ent->drawPosition, position);
	ent->type = type;
	ent->scale = vector2d(1, 1);
	ent->colorShift = vector4d(255, 255, 255, 255);
	ent->update = pickup_update;
	gfc_rect_set(ent->hurtbox, ent->drawPosition.x, ent->drawPosition.y, ent->sprite->frame_w * ent->scale.x, ent->sprite->frame_h * ent->scale.y);

	return ent;
}

void pickup_update(Entity* self) {
	gfc_rect_set(self->hurtbox, self->drawPosition.x, self->drawPosition.y, self->sprite->frame_w * self->scale.x, self->sprite->frame_h * self->scale.y);

	if (player_collison_check(self->hurtbox)) {
		player_change_health(30);
		entity_free(self);
	}
}