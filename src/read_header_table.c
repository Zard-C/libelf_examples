/*
 *  Print the ELF Program Header Table in an ELF object
 */
#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
#include <bsd/vis.h>
#include <elf.h>

void
print_ptype(size_t pt)
{
    char* s;
    #define C(V) case PT_##V:  s = #V; break
    switch (pt) {
    C(NULL); C(LOAD); C(DYNAMIC); C(INTERP); C(NOTE); C(SHLIB); C(PHDR);
    C(TLS); C(SUNWBSS); C(SUNWSTACK);
    // C(SUNW_UNWIND);  solaris 
    // C(SUNWDTRACE);   //solaris
    // C(SUNWCAP);      solaris
    default: s = "unknown"; break;
    }
    printf(" \"%s\"", s);
    #undef C  
}

int 
main(int argc, char **argv)
{
    int i, fd;
    Elf *e;
    char *id;
    char bytes[5]={0};
    size_t n;

    GElf_Phdr phdr;

    if(argc != 2)
        errx(EX_USAGE, "%s file-name", argv[0]);
    
    if(elf_version(EV_CURRENT) == EV_NONE)
        errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));

    if((fd = open(argv[1], O_RDONLY, 0)) < 0)
        err(EX_NOINPUT, "open \"%s\" failed", argv[1]);
    
    if((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));
    
    if(elf_kind(e) != ELF_K_ELF)
        errx(EX_DATAERR, "\"%s\" is not an ELF object.", argv[1]);
    
    if(elf_getphdrnum(e, &n) != 0)
        errx(EX_SOFTWARE, "elf_getphdrnum() failed: %s.", elf_errmsg(-1));
    
    for( i = 0; i < n; i++)
    {
        if(gelf_getphdr(e, i, &phdr) != &phdr)
            errx(EX_SOFTWARE, "getphdr() failed: %s.", elf_errmsg(-1));
        printf("Program Header[%d]:\n", i);
        #define PRINT_FMT    "    %-20s 0x%jx\n"
        #define PRINT_FILED(N) do { \
            printf(PRINT_FMT, #N, (uintmax_t)phdr.N); \
        } while(0)
        #define NL() do { printf("\n"); } while(0)

        PRINT_FILED(p_type);        NL();
        print_ptype(phdr.p_type);   NL();
        PRINT_FILED(p_offset);      NL();
        PRINT_FILED(p_vaddr);       NL();
        PRINT_FILED(p_paddr);       NL();
        PRINT_FILED(p_filesz);      NL();
        PRINT_FILED(p_memsz);       NL();
        PRINT_FILED(p_flags);       NL();
        printf(" [");
        if(phdr.p_flags & PF_X) printf(" execute");
        if(phdr.p_flags & PF_R) printf(" read");
        if(phdr.p_flags & PF_W) printf(" write");
        printf(" ]\n");             NL();
        PRINT_FILED(p_align);       NL();
    }

    elf_end(e);
    close(fd);
    return 0;
}