// Kyoei: the core's private enum has the same name as the Windows SDK one (the core only
// skips it for MinGW). Rename it here, as the Godzilla shim does, without editing the copy.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <windows.h>
#define _QUEUE_USER_APC_FLAGS KyoeiInternalQueueUserApcFlags
#define QUEUE_USER_APC_FLAGS KyoeiInternalQueueUserApcFlagsType
#include "../../../third_party/gow3-gpu/shadps4/common/ntapi.h"
#undef QUEUE_USER_APC_FLAGS
#undef _QUEUE_USER_APC_FLAGS
