"""Derive kernel semaphore/event code with native pthread synchronization."""
import argparse
from pathlib import Path


def adapt(source):
    source = source.replace('\r\n', '\n')
    source = source.replace('on Windows SRW locks and condition variables.',
                            'on POSIX mutexes and monotonic condition variables.')
    replacements = {
        '#include <windows.h>': '#include "posix_wait.h"',
        'SRWLOCK_INIT': 'PTHREAD_MUTEX_INITIALIZER',
        'SRWLOCK': 'pthread_mutex_t',
        'CONDITION_VARIABLE': 'pthread_cond_t',
        'AcquireSRWLockExclusive': 'host_lock',
        'ReleaseSRWLockExclusive': 'host_unlock',
        'InitializeSRWLock': 'host_mutex_init',
        'InitializeConditionVariable': 'host_cond_init',
        'WakeAllConditionVariable': 'host_broadcast',
        'SleepConditionVariableSRW': 'host_wait',
        'GetTickCount64': 'host_milliseconds',
        'ULONGLONG': 'uint64_t',
        'DWORD': 'uint64_t',
        'INFINITE': 'POSIX_INFINITE',
    }
    # Replace complete identifiers: SRWLOCK also appears inside API names.
    import re
    for old, new in replacements.items():
        if old == '#include <windows.h>':
            if source.count(old) != 1: raise ValueError('Review semaphore include')
            source = source.replace(old, new)
        else:
            source, count = re.subn(r'\b' + re.escape(old) + r'\b', new, source)
            if not count: raise ValueError('Review semaphore identifier: ' + old)
    for old, new in [
        ('if (!--s->active && s->deleted) free(s);',
         'if (!--s->active && s->deleted) { host_check(pthread_cond_destroy(&s->changed)); free(s); }'),
        ('if (!s->active) free(s);',
         'if (!s->active) { host_check(pthread_cond_destroy(&s->changed)); free(s); }'),
        ('    free(e);',
         '    host_check(pthread_cond_destroy(&e->changed));\n    host_check(pthread_mutex_destroy(&e->lock));\n    free(e);'),
    ]:
        if source.count(old) != 1: raise ValueError('Review semaphore lifetime: ' + old)
        source = source.replace(old, new)
    return source


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    args.output.write_text(adapt(args.source.read_text(encoding='utf8')), encoding='utf8')


if __name__ == '__main__': main()
