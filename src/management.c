#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "management.h"

void load_header(FILE *disk, DiskHeader *header) {
    fseek(disk, 0, SEEK_SET);
    fread(header, sizeof(DiskHeader), 1, disk);
}

void save_header(FILE *disk, DiskHeader *header) {
    fseek(disk, 0, SEEK_SET);
    fwrite(header, sizeof(DiskHeader), 1, disk);
}

void about_drive(const char *disk_name) {
    FILE *disk = fopen(disk_name, "rb");
    if (!disk) {
        perror("Could not open virtual disk");
        return;
    }

    DiskHeader header;
    load_header(disk, &header);

    printf("\033[34;1m=======================================\033[0m\n");
    printf("\033[34;1mSummary information about virtual drive\033[0m\n");
    printf("\033[34;1m=======================================\033[0m\n\n");

    printf("=======================================\n");
    printf("\033[0;34m              General Info             \033[0m\n");
    printf("=======================================\n");

    printf("\033[0;34mDrive name:\033[0m %s\n", header.disk_name);
    printf("\033[0;34mDrive size [bytes]:\033[0m %lu \n", header.size);
    printf("\033[0;34mSaved files:\033[0m %d \n", header.dir.file_count);

    fclose(disk);
}

void create_disk(const char *name, int size) {
    FILE *f = fopen(name, "wb");
    if (!f) {
        perror("Error creating disk");
        exit(1);
    }

    DiskHeader header;
    strcpy(header.disk_name, name);
    header.size = (unsigned long)size;
    header.max_filename = MAX_FILENAME;
    header.max_files = MAX_FILES;
    header.block_size = BLOCK_SIZE;
    header.max_blocks = MAX_BLOCKS;
    header.header_size = HEADER_SIZE;
    fwrite(&header, sizeof(DiskHeader), 1, f);

    int remaining = size - sizeof(DiskHeader);
    char zero = 0;
    for (int i = 0; i < remaining; i++) fwrite(&zero, 1, 1, f);

    fclose(f);
    printf("Disk %s created (%d bytes).\n", name, size);
}

void delete_disk(const char *name) {
    if (remove(name) == 0)
        printf("Disk %s deleted.\n", name);
    else
        perror("Error deleting disk");
}

void copy_in(const char *disk_name, const char *host_file) {
    FILE *disk = fopen(disk_name, "rb+");
    if (!disk) {
        perror("Could not open virtual disk");
        return;
    }

    FILE *src = fopen(host_file, "rb");
    if (!src) {
        perror("Could not open host file");
        fclose(disk);
        return;
    }

    struct stat st;
    stat(host_file, &st);
    int file_size = st.st_size;
    int blocks_needed = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    DiskHeader header;
    load_header(disk, &header);

    if (header.dir.file_count >= MAX_FILES) {
        printf("Directory full.\n");
        fclose(disk); fclose(src);
        return;
    }

    int start_block = -1;
    int found = 0;
    for (int i = 0; i <= MAX_BLOCKS - blocks_needed; i++) {
        int ok = 1;
        for (int j = 0; j < blocks_needed; j++) {
            if (header.block_map[i + j]) {
                ok = 0;
                break;
            }
        }
        if (ok) {
            start_block = i;
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Not enough space on virtual disk.\n");
        fclose(disk); fclose(src);
        return;
    }

    fseek(disk, sizeof(DiskHeader) + start_block * BLOCK_SIZE, SEEK_SET);
    char buffer[BLOCK_SIZE];
    int remaining = file_size;
    while (remaining > 0) {
        int to_read = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        fread(buffer, 1, to_read, src);
        fwrite(buffer, 1, to_read, disk);
        remaining -= to_read;
    }

    for (int i = 0; i < blocks_needed; i++)
        header.block_map[start_block + i] = 1;

    FileEntry *entry = &header.dir.files[header.dir.file_count++];
    strncpy(entry->name, host_file, MAX_FILENAME);
    entry->size = file_size;
    entry->start_block = start_block;
    entry->block_count = blocks_needed;
    entry->valid = 1;

    save_header(disk, &header);
    printf("Copied '%s' into virtual disk (%d bytes, blocks %d-%d).\n",
           host_file, file_size, start_block, start_block + blocks_needed - 1);

    fclose(disk);
    fclose(src);
}

void copy_out(const char *disk_name, const char *file_name, const char *output) {
    FILE *disk = fopen(disk_name, "rb");
    if (!disk) {
        perror("Could not open virtual disk");
        return;
    }

    FILE *dst = fopen(output, "wb");
    if (!dst) {
        perror("Could not create output file");
        fclose(disk);
        return;
    }

    DiskHeader header;
    load_header(disk, &header);

    FileEntry *entry = NULL;
    for (int i = 0; i < header.dir.file_count; i++) {
        if (header.dir.files[i].valid && strcmp(header.dir.files[i].name, file_name) == 0) {
            entry = &header.dir.files[i];
            break;
        }
    }

    if (!entry) {
        printf("File '%s' not found on virtual disk.\n", file_name);
        fclose(disk); fclose(dst);
        return;
    }

    fseek(disk, sizeof(DiskHeader) + entry->start_block * BLOCK_SIZE, SEEK_SET);
    char buffer[BLOCK_SIZE];
    int remaining = entry->size;

    while (remaining > 0) {
        int to_read = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        fread(buffer, 1, to_read, disk);
        fwrite(buffer, 1, to_read, dst);
        remaining -= to_read;
    }

    printf("Copied '%s' from virtual disk to '%s'.\n", file_name, output);
    fclose(disk);
    fclose(dst);
}

void list_directory(const char *disk_name) {
    FILE *disk = fopen(disk_name, "rb");
    if (!disk) {
        perror("Could not open virtual disk");
        return;
    }

    DiskHeader header;
    load_header(disk, &header);

    printf("Directory contents:\n");
    for (int i = 0; i < header.dir.file_count; i++) {
        FileEntry *f = &header.dir.files[i];
        if (f->valid)
            printf("- %s (%d B), blocks: %d-%d\n", f->name, f->size, f->start_block, f->start_block + f->block_count - 1);
    }

    fclose(disk);
}

void remove_file(const char *disk_name, const char *file_name) {
    FILE *disk = fopen(disk_name, "rb+");
    if (!disk) {
        perror("Could not open virtual disk");
        return;
    }

    DiskHeader header;
    load_header(disk, &header);

    FileEntry *entry = NULL;
    for (int i = 0; i < header.dir.file_count; i++) {
        if (header.dir.files[i].valid && strcmp(header.dir.files[i].name, file_name) == 0) {
            entry = &header.dir.files[i];
            break;
        }
    }

    if (!entry) {
        printf("File '%s' not found on virtual disk.\n", file_name);
        fclose(disk);
        return;
    }

    for (int i = 0; i < entry->block_count; i++)
        header.block_map[entry->start_block + i] = 0;

    entry->valid = 0;
    printf("File '%s' removed from virtual disk.\n", file_name);

    save_header(disk, &header);
    fclose(disk);
}

void show_map(const char *disk_name) {
    FILE *disk = fopen(disk_name, "rb");
    if (!disk) {
        perror("Could not open virtual disk");
        return;
    }

    DiskHeader header;
    load_header(disk, &header);

    printf("Disk block map:\n");
    for (int i = 0; i < MAX_BLOCKS; i++) {
        printf("Block %4d: %s\n", i, header.block_map[i] ? "USED" : "FREE");
    }

    fclose(disk);
}
