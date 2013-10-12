#include "utils.h"
#include <lv2/sysfs.h>
#include <string>
#include <stdio.h>

#define HDD_DEVICE			0x0101000000000007ULL

using namespace std;

uint32_t reboot_sys(){
	lv2syscall4(379,0x200,0,0,0);
	return_to_user_prog(u32);
}

typedef struct{
	uint32_t firmare_version; //Ex: 0x03055000
	uint8_t res1[4];
	uint64_t platform_id;
	uint32_t unknown;
	uint8_t res2[4];
} platform_info;

typedef struct
{
    uint8_t     name[7];
    uint8_t     unknown01;
    uint32_t    unknown02;
    uint32_t    zero01;
    uint32_t    unknown03;
    uint32_t    unknown04;
    uint8_t     zero02[16];
    uint64_t    total_sectors;
    uint32_t    sector_size;
    uint32_t    unknown05;
    uint8_t     writable;
    uint8_t     unknown06[3];
    uint32_t    unknown07;
} __attribute__((packed)) device_info_t;
 
s32 lv2_get_platform_info(platform_info *info){
	lv2syscall1(387, (uint64_t) info);
	return_to_user_prog(s32);
}

static inline int sys_storage_get_device_info(uint64_t device, device_info_t *device_info){
    lv2syscall2(609, device, (uint64_t) device_info);
    return_to_user_prog(int);
}


const char* get_ps3_version(){
	platform_info info;
	lv2_get_platform_info(&info); //0x03055000
	static char buf[50];
	sprintf(buf, "%02X", info.firmare_version);
	sprintf(buf, "%c.%c%c", buf[0], buf[2], buf[3]);
	
	return (const char*)buf;
}

unsigned long int get_ps3_free_space(){
	u32 blockSize;
	u64 freeBlocks;
	sysFsGetFreeSize("/dev_hdd0/", &blockSize, &freeBlocks);
	return ((u64)blockSize)*freeBlocks;
}

