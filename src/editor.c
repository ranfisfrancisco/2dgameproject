#include "editor.h"
#include "simple_logger.h" 
#include "entity.h"
#include "camera.h"

static Entity* editor;

void editor_init() {

}

void editor_update() {
	//int up, down, left, right;
	//Vector2D cameraNewPosition, playerNewPosition;
	//SDL_Rect testHurtbox;
	//const Uint8* keys;

	//keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

	//playerNewPosition = player->drawPosition;

	//up = keys[SDL_SCANCODE_W];
	//right = keys[SDL_SCANCODE_D];
	//down = keys[SDL_SCANCODE_S];
	//left = keys[SDL_SCANCODE_A];

	////TODO: Redo boundary check
	//if (left) {
	//	playerNewPosition.x -= player->speed;
	//	//if (player->drawPosition.x < 0)
	//	//	player->drawPosition.x = 0;

	//}
	//else if (right) {
	//	playerNewPosition.x += player->speed;
	//	//if (player->drawPosition.x > 1200 - 2 * player->sprite->frame_w)
	//	//	player->drawPosition.x = 1200 - 2 * player->sprite->frame_w;

	//}
	//if (up) {
	//	playerNewPosition.y -= player->speed;
	//	//if (player->drawPosition.y < 0 + player->sprite->frame_h / 2)
	//	//	player->drawPosition.y = 0 + player->sprite->frame_h / 2;

	//}
	//else if (down) {
	//	playerNewPosition.y += player->speed;
	//	//if (player->drawPosition.y > 1200 - 5 * player->sprite->frame_h)
	//	//	player->drawPosition.y = 1200 - 5 * player->sprite->frame_h;
	//}

	//player_set_hurtbox(&testHurtbox, &playerNewPosition);

	////check hurtbox overlap with interactbles
	//if (entity_manager_player_interactable_collision(testHurtbox))
	//	return;

	//player->drawPosition = playerNewPosition;

	//cameraNewPosition.x = player->drawPosition.x - 1200 / 2;
	//cameraNewPosition.y = player->drawPosition.y - 720 / 2;
	//camera_set_position(cameraNewPosition);
	//director_snap_camera();
}