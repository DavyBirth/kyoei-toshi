/* Host-test build of the pause photo row (no game image). */
#define KYOEI_PAUSE_PHOTO_TEST 1
#include "kyoei_pause_photo.c"
unsigned kyoei_input_current_mode(void) { return 0; }
void kyoei_mouse_camera_boost(int mouse) { (void)mouse; }
