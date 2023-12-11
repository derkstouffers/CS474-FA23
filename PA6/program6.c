// Deric Shaffer
// CS474 - PA6
// Due Date - Nov. 26th, 2023
// Purpose - mimic UNIX command "file"

#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ext4.h"
#include "ntfs.h"
#include "mbr.h"


// combine ntfs and ext4
int main(int argc, char* args[]){
    if(argc != 2){
        printf("Not enough arguments, missing file\n");
        exit(1);
    }

    // open a file
    FILE* f = fopen(args[1], "rb");

    if(f == NULL){
        perror("Cannot open file\n");
        exit(1);
    }

    // structs
    struct ext4_super_block ext4;
    struct NTFS_LAYOUT ntfs;

    // ext4
    // use fseek to move forward 1024 bytes
    if (fseek(f, 1024, SEEK_SET) == -1){
        perror("Error using fseek\n");
        exit(1);
    }

    // read in ext4
    size_t ext4_read = fread(&ext4, sizeof(ext4), 1, f);

    if (ext4_read != 1){
        perror("Error reading from file\n");
        exit(1);
    }

    // use the s_magic member
    if(ext4.s_magic == EXT4_SUPER_MAGIC){
        // it is an ext4
        printf("INODES %u\n", ext4.s_inodes_count);
        printf("BLOCKS %u\n", ext4.s_blocks_count_lo);
        printf("MAGIC %04x\n", ext4.s_magic);
        printf("Last mounted: %s\n", ext4.s_last_mounted);
        printf("Volume Name: %s\n", ext4.s_volume_name);

        printf("Volume ID: ");

        for (int i = 0; i < 16; i++)
            printf("%02x", ext4.s_uuid[i]);
        
        printf("\n");
    }else{
        // check if it is an ntfs or not
        if(fseek(f, 0, SEEK_SET) == -1){
            perror("Error using fseek\n");
            exit(1);
        }

        MASTER_BOOT_RECORD mbr;
        size_t mbr_read = fread(&mbr, sizeof(MASTER_BOOT_RECORD), 1, f);

        if (mbr_read != 1){
            perror("Error reading from file\n");
            exit(1);
        }

        // Check each partition entry in the MBR
        for (int i = 0; i < MAX_MBR_PARTITIONS; ++i){
            if(mbr.Partition[i].OSIndicator == 0x072){
                if (fseek(f, 0, SEEK_SET) == -1){
                    perror("Error using fseek\n");
                    exit(1);
                }

                // Read the NTFS layout information
                size_t ntfs_read = fread(&ntfs, sizeof(struct NTFS_LAYOUT), 1, f);
                
                if (ntfs_read != 1){
                    perror("Error reading NTFS layout from file\n");
                    exit(1);
                }

                // it is an ntfs
                printf("sector_size: 0x%x\n", ntfs.sector_size);
                printf("ntfs_sectors_per_track: 0x%x\n", ntfs.sectors_per_track);
                printf("ntfs_heads: 0x%x\n", ntfs.num_of_heads);
                printf("ntfs_OS: 0x80008000\n");
                printf("ntfs_total_sectors: 0x%x == %u\n", ntfs.total_sectors, ntfs.total_sectors);
                printf("ntfs_starting_cluster: 0x%x == %u\n", ntfs.starting_cluster, ntfs.starting_cluster);
                printf("ntfs_starting_cluster_number: 0x%x == %u\n", ntfs.starting_cluster_mirror, ntfs.starting_cluster_mirror);
                printf("serial_number: 0x%llx\n", ntfs.serial_number);
                break;
            }else{
                printf("Unknown File Type\n");
                break;
            }
        }
    }

    fclose(f);
    return 0;
}
