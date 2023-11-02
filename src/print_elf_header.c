/*
 *  Print the Elf executable header
 */

#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
#include <bsd/vis.h>


int main(int argc, char** argv)
{
    int i, fd;
    Elf *e;
    char* id;
    char bytes[5];
    size_t n;

    GElf_Ehdr ehdr;

    if (argc != 2)
        errx(EX_USAGE, "usage: %s file-name", argv[0]);

    if(elf_version(EV_CURRENT) == EV_NONE)
        errx(EX_SOFTWARE, "ELF library initialization failed: %s", elf_errmsg(-1));
    
    if((fd = open(argv[1], O_RDONLY, 0)) < 0)
        err(EX_NOINPUT, "open %s failed", argv[1]);
    
    if((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EX_SOFTWARE, "elf_begin() failed: %s.", elf_errmsg(-1));
    
    if(elf_kind(e) != ELF_K_ELF)
        errx(EX_DATAERR, "%s is not an ELF object.", argv[1]);
    
    if(gelf_getehdr(e, &ehdr) == NULL)
        errx(EX_SOFTWARE, "getehdr() failed: %s.", elf_errmsg(-1));
    
    if((i = gelf_getclass(e)) == ELFCLASSNONE)
        errx(EX_DATAERR, "getclass() failed: %s.", elf_errmsg(-1));
    
    printf("%s %d-bit ELF object\n", argv[1], i == ELFCLASS32 ? 32 : 64);

    if((id = elf_getident(e, NULL)) == NULL)
        errx(EX_SOFTWARE, "getident() failed: %s.", elf_errmsg(-1));

    printf("%3s e_ident[0..%1d] %7s", " ", EI_ABIVERSION, " ");
    for(i = 0; i <= EI_ABIVERSION; i++)
    {   
        vis(bytes , id[i], VIS_WHITE , 0);
        printf(" ['%s' %X]", bytes , id[i]);
    }
#define PRINT_FMT "    %-20s 0x%jx\n"
#define PRINT_FIELD(field) do { \
    printf(PRINT_FMT, #field, (uintmax_t)ehdr.field); \
}  while(0)

    printf("\nElf header:\n");
    PRINT_FIELD(e_type);
    PRINT_FIELD(e_machine);
    PRINT_FIELD(e_version);
    PRINT_FIELD(e_entry);
    PRINT_FIELD(e_phoff);
    PRINT_FIELD(e_shoff);
    PRINT_FIELD(e_flags);
    PRINT_FIELD(e_ehsize);
    PRINT_FIELD(e_phentsize);
    PRINT_FIELD(e_phnum);
    PRINT_FIELD(e_shentsize);
    PRINT_FIELD(e_shnum);
    PRINT_FIELD(e_shstrndx);

    if (elf_getshdrnum(e, &n) != 0)
        errx(EX_SOFTWARE, "getshdrnum() failed: %s.", elf_errmsg(-1));
    printf(PRINT_FMT, "(shnum)", n);

    if (elf_getshdrstrndx(e, &n) != 0)
        errx(EX_SOFTWARE, "getshdrstrndx() failed: %s.", elf_errmsg(-1));
    printf(PRINT_FMT, "(shstrndx)", n);

    if(elf_getphdrnum(e, &n) != 0)
        errx(EX_SOFTWARE, "getphdrnum() failed: %s.", elf_errmsg(-1));
    printf(PRINT_FMT, "(phnum)", n);

    elf_end(e);
    close(fd);
    exit(EX_OK);
}