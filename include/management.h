#ifndef FS_H
#define FS_H

#define MAX_DISK_NAME 32
#define MAX_FILENAME 32
#define MAX_FILES 128
#define BLOCK_SIZE 4096
#define MAX_BLOCKS 25600
#define MIN_BLOCKS 1
#define HEADER_SIZE 31808

typedef struct {
    char name[MAX_FILENAME];
    int size;
    int start_block;
    int block_count;
    int valid;
} FileEntry;

typedef struct {
    FileEntry files[MAX_FILES];
    int file_count;
} Directory;

typedef struct {
    char disk_name[MAX_DISK_NAME];
    unsigned long size;

    unsigned int max_filename;
    unsigned int max_files;
    unsigned int block_size;
    unsigned int max_blocks;
    unsigned int header_size;

    Directory dir;
    char block_map[MAX_BLOCKS];
} DiskHeader;

void create_disk(const char *name, int block_size);
void delete_disk(const char *name);
void copy_in(const char *disk_name, const char *host_file);
void copy_out(const char *disk_name, const char *file_name, const char *output);
void remove_file(const char *disk_name, const char *file_name);
void list_directory(const char *disk_name);
void show_map(const char *disk_name);
void load_header(FILE *disk, DiskHeader *header);
void save_header(FILE *disk, const DiskHeader *header);
void about_drive(const char *disk_name);

#endif
