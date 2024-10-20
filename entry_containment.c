
int entry(int argc, char **argv) {
	
	// This is how we (optionally) configure the window.
	// To see all the settable window properties, ctrl+f "struct Os_Window" in os_interface.c
	window.title = STR("Minimal Game Example");
	window.point_width = 1280;
	window.point_height = 720;
	window.x = 200;
	window.y = 200;
	window.clear_color = hex_to_rgba(0x2a2d3aff);
	
	Gfx_Image *anim_sheet = load_image_from_disk(STR("Bob_16x16.png"), get_heap_allocator());
	assert(anim_sheet, "Could not open oogabooga/examples/male_animation.png");

	u32 number_of_columns = 24;
	u32 number_of_rows = 7;
	u32 total_number_of_frames = number_of_columns * number_of_rows;

	u32 anim_frame_width  = anim_sheet->width  / number_of_columns;
	u32 anim_frame_height = anim_sheet->height / number_of_rows;

	u32 anim_start_frame_x = 2;
	u32 anim_start_frame_y = 1;
	u32 anim_end_frame_x = 6;
	u32 anim_end_frame_y = 2;
	u32 anim_start_index = anim_start_frame_y * number_of_columns + anim_start_frame_x;
	u32 anim_end_index   = anim_end_frame_y   * number_of_columns + anim_end_frame_x;
	u32 anim_number_of_frames = max(anim_end_index, anim_start_index)-min(anim_end_index, anim_start_index)+1;
	
	// Sanity check configuration
	assert(anim_end_index > anim_start_index, "The last frame must come before the first frame");
	assert(anim_start_frame_x < number_of_columns, "anim_start_frame_x is out of bounds");
	assert(anim_start_frame_y < number_of_rows, "anim_start_frame_y is out of bounds");
	assert(anim_end_frame_x < number_of_columns, "anim_end_frame_x is out of bounds");
	assert(anim_end_frame_y < number_of_rows, "anim_end_frame_y is out of bounds");

// Calculate duration per frame in seconds
	float32 playback_fps = 4;
	float32 anim_time_per_frame = 1.0 / playback_fps;
	float32 anim_duration = anim_time_per_frame * (float32)anim_number_of_frames;
	
	float32 anim_start_time = os_get_elapsed_seconds();
	
	Vector2 player_pos = v2(0,0);
	float player_speed = 200.0f; // pixels per second

	float64 last_time = os_get_elapsed_seconds();
	while (!window.should_close) {
		reset_temporary_storage();
		

		if (is_key_just_pressed(KEY_ESCAPE)){
			window.should_close = true;
		}
		
		float64 now = os_get_elapsed_seconds();
		float64 delta = now-last_time;
		last_time = now;
		
		draw_frame.projection = m4_make_orthographic_projection(window.pixel_width * -0.5, window.pixel_width * 0.5, window.pixel_height * -0.5, window.pixel_height * 0.5, -1, 10);
		
		// Float modulus to "loop" around the timer over the anim duration
		float32 anim_elapsed = fmodf(now - anim_start_time, anim_duration);
		
		// Get current progression in animation from 0.0 to 1.0
		float32 anim_progression_factor = anim_elapsed / anim_duration;
		
		u32 anim_current_index = anim_number_of_frames * anim_progression_factor;
		u32 anim_absolute_index_in_sheet = anim_start_index + anim_current_index;
		
		u32 anim_index_x = anim_absolute_index_in_sheet % number_of_columns;
		u32 anim_index_y = anim_absolute_index_in_sheet / number_of_columns + 1;
		
		u32 anim_sheet_pos_x = anim_index_x * anim_frame_width;
		u32 anim_sheet_pos_y = (number_of_rows - anim_index_y) * anim_frame_height; // Remember, Y inverted.
		
	
		
		
		// Visualize sprite sheet animation
		Vector2 sheet_pos = v2(-window.width/2+40, -window.height/2+40);
		Vector2 sheet_size = v2(anim_sheet->width, anim_sheet->height);
		Vector2 frame_pos_in_sheet = v2(anim_sheet_pos_x, anim_sheet_pos_y);
		Vector2 frame_size = v2(anim_frame_width, anim_frame_height);
		draw_rect(sheet_pos, sheet_size, COLOR_BLACK); // Draw black background
		draw_rect(v2_add(sheet_pos, frame_pos_in_sheet), frame_size, COLOR_WHITE); // Draw white rect on current frame
		draw_image(anim_sheet, sheet_pos, sheet_size, COLOR_WHITE); // Draw the seet
		
			Vector2 input_axis = v2(0, 0);
		if (is_key_down('A')) {
			input_axis.x -= 1.0;
		}
		if (is_key_down('D')) {
			input_axis.x += 1.0;
		}
		if (is_key_down('S')) {
			input_axis.y -= 1.0;
		}
		if (is_key_down('W')) {
			input_axis.y += 1.0;
		}

		if (v2_length(input_axis) > 0) {
			input_axis = v2_normalize(input_axis);
		}

		// update player position
		Vector2 movement = v2_mulf(input_axis, player_speed * delta);
		player_pos = v2_add(player_pos, movement);

		 // Draw the player at the updated position
        Draw_Quad *player_quad = draw_image(anim_sheet, player_pos, v2(anim_frame_width*4, anim_frame_height*4), COLOR_WHITE);
        player_quad->uv.x1 = (float32)(anim_sheet_pos_x)/(float32)anim_sheet->width;
        player_quad->uv.y1 = (float32)(anim_sheet_pos_y)/(float32)anim_sheet->height;
        player_quad->uv.x2 = (float32)(anim_sheet_pos_x+anim_frame_width) /(float32)anim_sheet->width;
        player_quad->uv.y2 = (float32)(anim_sheet_pos_y+anim_frame_height)/(float32)anim_sheet->height;

		os_update();
		gfx_update();
	}

	return 0;
}