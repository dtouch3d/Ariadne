#define MAX_STR 100

/* funcname is evaluated at compile time with __func__ macro by the caller */
static void
show_linenum(void* wrapcxt, const char* funcname)
{
    app_pc addr = drwrap_get_retaddr(wrapcxt);
    module_data_t* modinfo = dr_lookup_module(addr);

    if (modinfo == NULL)
        return;

    /* Stupid approach to ignore calls from other sources */
    const char* appname = dr_get_application_name();
    if (strstr(modinfo->full_path, appname) == NULL)
        return;

    drsym_info_t sym;
    char name[MAX_STR];
    char file[MAX_STR];
    sym.struct_size = sizeof(sym);
    sym.name = name;

    sym.name_size = sizeof(name);
    sym.file_size = sizeof(file);
    sym.file = file;

    drsym_lookup_address(modinfo->full_path, addr-modinfo->start, &sym, DRSYM_DEFAULT_FLAGS);
    dr_printf("[+] %s at %s:%d\n", funcname, sym.file, sym.line);
}