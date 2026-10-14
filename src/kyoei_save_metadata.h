/* Included after Param/open_utf8 in the adapted folder backend. */
/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
static void kyoei_discard_empty_save(const char *host,const char *meta) {
    wchar_t wide[1024];
    // No recursive cleanup: a directory containing any data is preserved.
    if(!gz_widen(meta,wide,1024)) RemoveDirectoryW(wide);
    if(!gz_widen(host,wide,1024)) RemoveDirectoryW(wide);
}
static int write_param(const char *meta,const Param *p) {
    static volatile LONG sequence;
    char path[760];wchar_t target[1024],temporary[1100];
    int length=snprintf(path,sizeof(path),"%s\\param.bin",meta);
    if(length<0 || (size_t)length>=sizeof(path) || gz_widen(path,target,1024)) return -1;
    HANDLE file=INVALID_HANDLE_VALUE;
    for(unsigned attempt=0;attempt<16;++attempt) {
        if(swprintf(temporary,1100,L"%ls.tmp-%lu-%lu",target,(unsigned long)GetCurrentProcessId(),
                    (unsigned long)InterlockedIncrement(&sequence))<0) return -1;
        file=CreateFileW(temporary,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
        if(file!=INVALID_HANDLE_VALUE) break;
        if(GetLastError()!=ERROR_FILE_EXISTS && GetLastError()!=ERROR_ALREADY_EXISTS) return -1;
    }
    if(file==INVALID_HANDLE_VALUE) return -1;
    Param copy=*p;copy.mtime=(int64_t)time(NULL);
    DWORD written=0;
    int okay=WriteFile(file,&copy,sizeof(copy),&written,NULL) && written==sizeof(copy);
    if(okay) okay=FlushFileBuffers(file)!=0;
    if(!CloseHandle(file)) okay=0;
    if(okay) okay=MoveFileExW(temporary,target,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
    if(!okay) DeleteFileW(temporary);
    return okay?0:-1;
}
