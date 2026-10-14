/* Parse a complete fixed-size mount-point field, never atoi a prefix. */
#pragma once
static int kyoei_save_slot_number(const char *point,size_t capacity) {
    if(!point || capacity<11 || memcmp(point,"/savedata",9)) return -1;
    unsigned value=0;size_t offset=9;
    if(point[offset]<'0' || point[offset]>'9') return -1;
    if(point[offset]=='0' && point[offset+1]) return -1;
    for(;offset<capacity && point[offset];++offset) {
        if(point[offset]<'0' || point[offset]>'9') return -1;
        value=value*10+(unsigned)(point[offset]-'0');
        if(value>=16) return -1;
    }
    return offset<capacity?(int)value:-1;
}
