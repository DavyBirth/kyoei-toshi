/* PC mouse turning for the game camera. CUSA03562 reads the camera turn
 * limits from camera_data.luc (PadSpeedX/Y 50-100 deg/s, accel/brake 0.3-0.5 s)
 * into the active camera: global 0x1386ad0 -> +0x28 -> +0x1f8 -> +0x18.
 * Fields (verified in the setters at 0x64c640..0x650da0):
 *   X: speed +0x64c, accel time +0x654, accel rate +0x658, brake time +0x65c, brake rate +0x660
 *   Y: speed +0x684, accel time +0x68c, accel rate +0x690, brake time +0x694, brake rate +0x698
 * While the mouse turns the camera the limits are raised and acceleration is
 * immediate, so turning speed follows the mouse like a PC game. Controller
 * input gets the original values back (PS4 feel preserved).
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_mouse_camera.h"
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include "gz.h"

static unsigned char *base;
static uintptr_t camera;
static float original[10], applied[10];
static const unsigned offsets[10] = {0x64c, 0x654, 0x658, 0x65c, 0x660, 0x684, 0x68c, 0x690, 0x694, 0x698};

static int readable(uintptr_t address, size_t n) {
    MEMORY_BASIC_INFORMATION info;
    if (!address || !VirtualQuery((void *)address, &info, sizeof info) || info.State != MEM_COMMIT ||
        (info.Protect & (PAGE_NOACCESS | PAGE_GUARD)) || !(info.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY)))
        return 0;
    return address + n <= (uintptr_t)info.BaseAddress + info.RegionSize;
}
static uintptr_t pointer(uintptr_t address) {
    uintptr_t value = 0;
    if (readable(address, 8)) memcpy(&value, (void *)address, 8);
    return value;
}
static uintptr_t active_camera(void) {
    if (!base) return 0;
    uintptr_t cam = pointer(pointer(pointer(pointer((uintptr_t)base + 0x1386ad0) + 0x28) + 0x1f8) + 0x18);
    return readable(cam + 0x640, 0x60) ? cam : 0;
}
static int plausible(const float *v) {
    for (int i = 0; i < 10; ++i) if (!(v[i] > 0 && v[i] < 1e7f)) return 0;
    return 1;
}
static void restore(void) {
    if (!camera || camera != active_camera()) { camera = 0; return; }
    float now[10];
    for (int i = 0; i < 10; ++i) memcpy(&now[i], (void *)(camera + offsets[i]), 4);
    /* Only undo what we wrote; a camera the game re-configured keeps its values. */
    if (!memcmp(now, applied, sizeof now))
        for (int i = 0; i < 10; ++i) memcpy((void *)(camera + offsets[i]), &original[i], 4);
    camera = 0;
}

void kyoei_mouse_camera_configure(void *image, uint64_t size) {
    if (image && size > 0x1386ad8) base = image;
}

void kyoei_mouse_camera_boost(int mouse) {
    static int forced = -1;
    if (forced < 0) { const char *v = getenv("KYOEI_TEST_MOUSE_CAMERA"); forced = v && *v == '1'; }
    mouse |= forced;
    if (!mouse) { restore(); return; }
    uintptr_t cam = active_camera();
    if (!cam) { camera = 0; return; }
    float now[10];
    for (int i = 0; i < 10; ++i) memcpy(&now[i], (void *)(cam + offsets[i]), 4);
    if (cam == camera && !memcmp(now, applied, sizeof now)) return;
    if (!plausible(now)) return;
    memcpy(original, now, sizeof now);   /* new camera, or the game re-applied its data */
    const float speed_scale = 6.f, time = 0.001f;
    float x = now[0] * speed_scale, y = now[5] * speed_scale;
    float v[10] = {x, time, x / time, time, x / time, y, time, y / time, time, y / time};
    for (int i = 0; i < 10; ++i) memcpy((void *)(cam + offsets[i]), &v[i], 4);
    memcpy(applied, v, sizeof v);
    camera = cam;
    gz_log("Kyoei mouse camera: turn limit %.3f -> %.3f, accel %.2f s -> immediate", original[0], v[0], original[1]);
}
