// Translation archives are separate local products; the dump remains read-only.
#pragma once
static void kyoei_translation_mount(void) {
    const char* language=getenv("KYOEI_LANGUAGE");
    if (!language || !*language || !strcmp(language,"ja")) return;
    if (strcmp(language,"en") && strcmp(language,"pt-BR"))
        gz_stop(GZ_EXIT_RUNTIME_STOP,"STOP: unsupported Kyoei language: %s",language);
    const char* archive=getenv("KYOEI_TRANSLATION_ARCHIVE");
    wchar_t wide[1024];
    if (!archive || !*archive || gz_widen(archive,wide,1024))
        gz_stop(GZ_EXIT_RUNTIME_STOP,"STOP: translation archive not configured");
    DWORD attributes=GetFileAttributesW(wide);
    if (attributes==INVALID_FILE_ATTRIBUTES || (attributes&FILE_ATTRIBUTE_DIRECTORY))
        gz_stop(GZ_EXIT_RUNTIME_STOP,"STOP: translation archive missing: %s",archive);
    if (gz_file_mount("/app0/DAT/common.psarc",archive) || gz_file_mount("/hostapp/DAT/common.psarc",archive))
        gz_stop(GZ_EXIT_RUNTIME_STOP,"STOP: cannot mount Kyoei translation archive");
    gz_log("Kyoei language: %s (partial experimental translation); archive=%s",language,archive);
}
