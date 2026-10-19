#include "kyoei_upload_source.h"
#include <cassert>
#include <cstring>
#include <cstdio>
int main(){
 constexpr size_t size=1024*1024;
 HANDLE section=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,size,nullptr);assert(section);
 auto* guest=static_cast<unsigned char*>(MapViewOfFile(section,FILE_MAP_ALL_ACCESS,0,0,size));assert(guest);
 auto* backing=static_cast<unsigned char*>(MapViewOfFile(section,FILE_MAP_ALL_ACCESS,0,0,size));assert(backing);
 auto* out=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(out);
 for(size_t i=0;i<size;++i)guest[i]=static_cast<unsigned char>((i*67)^(i>>7));
 auto select=[&](size_t bytes,int enabled=1){return kyoei_upload_source((uintptr_t)guest,backing,bytes,enabled);};
 assert(select(size)==guest);std::memcpy(out,select(size),size);assert(!std::memcmp(out,backing,size));
 assert(select(size,0)==backing);assert(select(255*1024)==backing);
 DWORD old;assert(VirtualProtect(guest,size,PAGE_READONLY,&old));assert(select(size)==guest);
 assert(VirtualProtect(guest,size,PAGE_NOACCESS,&old));assert(select(size)==backing);
 std::memcpy(out,select(size),size);assert(!std::memcmp(out,backing,size));
 assert(VirtualProtect(guest,size,PAGE_READWRITE|PAGE_GUARD,&old));assert(select(size)==backing);
 MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(guest,&info,sizeof(info)));assert(info.Protect&PAGE_GUARD);
 assert(VirtualProtect(guest,size,PAGE_READWRITE,&old));
 assert(VirtualProtect(guest+size/2,size/2,PAGE_READONLY,&old));assert(select(size)==backing);assert(select(size/2)==guest);
 assert(select(size+4096)==backing);assert(VirtualProtect(guest,size,PAGE_READWRITE,&old));
 // Copy-on-write and private allocations cannot be assumed to alias backing.
 auto* cow=MapViewOfFile(section,FILE_MAP_COPY,0,0,size);assert(cow);
 assert(kyoei_upload_source((uintptr_t)cow,backing,size,1)==backing);
 assert(kyoei_upload_source((uintptr_t)out,backing,size,1)==backing);
 UnmapViewOfFile(cow);UnmapViewOfFile(backing);UnmapViewOfFile(guest);CloseHandle(section);VirtualFree(out,0,MEM_RELEASE);
 std::puts("UPLOAD SOURCE PASS: exact bytes, read-only, no-access/guard, split range, COW/private, threshold and disabled fallback");
}
