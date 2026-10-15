"""Derive the calendar runtime from the fixed snapshot with checked POSIX anchors."""
import argparse
from pathlib import Path


def adapt(source):
    source = source.replace('\r\n', '\n')
    def replace(old, new):
        nonlocal source
        if source.count(old) != 1:
            raise ValueError('Review Linux RTC anchor: ' + old[:80])
        source = source.replace(old, new)
    replace('#include <windows.h>', '#include <time.h>\n#include <stdlib.h>')
    replace('''    FILETIME t;
    GetSystemTimePreciseAsFileTime(&t);
    uint64_t since1601 = (uint64_t)t.dwHighDateTime << 32 | t.dwLowDateTime;
    return UNIX_EPOCH_TICKS + (since1601 - UINT64_C(116444736000000000)) / 10;''', '''    struct timespec t;
    if (clock_gettime(CLOCK_REALTIME, &t)) abort();
    return UNIX_EPOCH_TICKS + (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_nsec / 1000;''')
    replace('''    TIME_ZONE_INFORMATION tz;
    DWORD mode = GetTimeZoneInformation(&tz);
    return -(int64_t)(tz.Bias + (mode == TIME_ZONE_ID_DAYLIGHT ? tz.DaylightBias : 0)) * 60000000;''', '''    time_t now = time(NULL);
    struct tm local;
    if (now == (time_t)-1 || !localtime_r(&now, &local)) abort();
    return (int64_t)local.tm_gmtoff * 1000000;''')
    replace('the host clock supplies "now" and the Windows time zone\n * supplies local time.',
            'clock_gettime supplies "now" and localtime_r supplies the Linux time zone.')
    return source


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    args.output.write_text(adapt(args.source.read_text(encoding='utf8')), encoding='utf8')


if __name__ == '__main__':
    main()
