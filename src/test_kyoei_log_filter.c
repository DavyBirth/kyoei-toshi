#include <stdio.h>
#include "kyoei_log_filter.h"
#define CHECK(x) do { if(!(x)) return __LINE__; } while(0)
int main(void) {
    _putenv_s("KYOEI_MEMORY_TRACE","");
    CHECK(!kyoei_log_visible("GPU memory mapped: %p"));
    CHECK(!kyoei_log_visible("GPU memory unmapped: %p"));
    CHECK(!kyoei_log_visible("Runtime: mapped direct memory %p"));
    CHECK(!kyoei_log_visible("Runtime: mapped flexible memory %p"));
    CHECK(kyoei_log_visible("Kyoei map failure: %p"));
    CHECK(kyoei_log_visible("Runtime: memory report"));
    CHECK(kyoei_log_visible("Exit code %d"));
    _putenv_s("KYOEI_MEMORY_TRACE","1");
    CHECK(kyoei_log_visible("GPU memory mapped: %p"));
    puts("LOG FILTER PASS: mapping trace opt-in; failures and reports preserved");
    return 0;
}
