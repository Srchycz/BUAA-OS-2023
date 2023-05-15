/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;
	u_int opt = DEV_DISK_OPERATION_READ;
	u_int res = 1;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
		uint32_t temp_off = begin + off;
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
		panic_on(syscall_write_dev(&temp_off, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
		panic_on(syscall_write_dev(&opt, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));
		panic_on(syscall_read_dev(dst + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
		panic_on(syscall_read_dev(&res, DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
		panic_on(res == 0);
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;
	u_int opt = DEV_DISK_OPERATION_WRITE;
	u_int res = 1;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		uint32_t temp_off = begin + off;
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, 4));
		panic_on(syscall_write_dev(&temp_off, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, 4));
		panic_on(syscall_write_dev(src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
		panic_on(syscall_write_dev(&opt, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, 4));
		panic_on(syscall_read_dev(&res, DEV_DISK_ADDRESS + DEV_DISK_STATUS, 4));
		panic_on(res == 0);
	}
}
//extra
int cnt[32];//pno
int mapSSD[32];//logic_no
int vis[32];//pno
void ssd_init() {
	for (int i = 0; i < 32; ++ i) {
		cnt[i] = 0;
		mapSSD[i] = -1;
		vis[i] = 0;
	}
}
int ssd_read(u_int logic_no, void *dst) {
	int p_no = mapSSD[logic_no];
	if (p_no == -1) return -1;
	ide_read(0, p_no, dst, 1);
	return 0;	
}
void ssd_write(u_int logic_no, void *src) {
	ssd_erase(logic_no);
	int num = 0x3f3f3f3f;
	int id = -1;//get min cnt pno
	for (int i = 0; i < 32; ++ i) {
		if (vis[i] == 0 && cnt[i] < num) {
			num = cnt[i];
			id = i;
		}
	}
	if (num >= 5) {
		int num2 = 0x3f3f3f3f;
		int id2 = -1;
		for (int i = 0; i < 32; ++ i) {
			if (vis[i] == 1 && cnt [i] < num2) {
				num2 = cnt[i];
				id2 = i;
			}
		}
		int temp[130];
		ide_read(0, id2, &temp, 1);
		ide_write(0, id, &temp, 1);
		vis[id] = 1;
		int lno = -1;
		for (int i = 0; i < 32; ++ i) {
			if (mapSSD[i] == id2) {
				lno = i;
				break;
			}
		}
		ssd_erase(lno);
		mapSSD[lno] = id;//change map
		id = id2;
	}
	ide_write(0, id, src, 1);
	vis[id] = 1;
	mapSSD[logic_no] = id;
}
void ssd_erase(u_int logic_no) {
	int pno = mapSSD[logic_no];
	if (pno == -1) return;
	int temp[130];
	for (int i = 0; i < 130; ++i) {
		temp[i] = 0;
	}
	ide_write(0, pno, &temp, 1);
	mapSSD[logic_no] = -1;
	++cnt[pno];
	vis[pno] = 0;
}

