#include <linux/types.h>

#ifndef _NTFS_H_
#define _NTFS_H_

/*
 * Structure of the super block
 */
#pragma pack(1)
struct NTFS_LAYOUT {
/*00*/
    __u8    dontcare[11];   // 0x00 - 0x0A
    __le16  sector_size; // 0x0B-0x0C the NTFS sector_size
    __u8    sectors_per_cluster; // 0x0D
    __u8    reservered0e; // 0x0E
    __u8    reservered0f; // 0x0F

/// Missing
    __u8    zero_1; // 0x10
    __le16  zero_2; // 0x11-0x12
    __le16  not_used_1; // 0x13-0x14
    __u8    media_desc; // 0x15
    __le16  zero_3; // 0x16-17
    __le16  sectors_per_track; // 0x18-0x19
    __le16  num_of_heads; // 0x1A-0x1B
    __le32  hidden; // 0x1C-0x1F
    __le64  not_used_2; // 0x20-0x23
    __le64  os_preset; // 0x24-0x27
    __le16  starting_cluster; // 0x30-0x37
    __le16  starting_cluster_mirror; // 0x38-0x39
    __le16  total_sectors; // 0x28-0x29
    __u8    clusters_per_record; // 0x40
    __le16  zero_4; // 0x41-0x43
    __s8    clusters_per_buffer; // 0x44
    __le16  not_used_3; // 0x45-0x47
    __le16  serial_number; // 0x48-0x49
    __u8    zero5; // 0x50
};

#endif