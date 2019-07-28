#include <linux/module.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#define DEVICE_MAJOR 0
#define DEVICE_NAME "ramdisk"
// #define SECTOR_SIZE 512
#define DISK_SIZE (3 * 1024 * 1024)
#define SECTOR_ALL (DISK_SIZE/SECTOR_SIZE)
static struct gendisk * p_disk;
static struct request_queue *p_queue;
static unsigned char mem_start[DISK_SIZE];

struct block_device_operations ramdisk_fops = {
  //.open = ramdisk_open,
  //.release = ramdisk_release,
  .owner = THIS_MODULE
};

static blk_qc_t ramdisk_make_request(struct request_queue* q, struct bio* bio);

static int ramdisk_init(void)
{
  p_queue = blk_alloc_queue(GFP_KERNEL);
  if (!p_queue) 
    return -1;
  blk_queue_make_request(p_queue, ramdisk_make_request);
  p_disk = alloc_disk(1);
  if (!p_disk) {
    blk_cleanup_queue(p_queue);
    return -1;
  }
  strcpy(p_disk->disk_name, DEVICE_NAME);
  p_disk->major = DEVICE_MAJOR;
  p_disk->first_minor = 0;
  p_disk->fops = &ramdisk_fops;
  p_disk->queue = p_queue;
  set_capacity(p_disk, SECTOR_ALL);
  add_disk(p_disk);
  return 0;
}

static void ramdisk_exit(void)
{
  del_gendisk(p_disk);
  put_disk(p_disk);
  blk_cleanup_queue(p_queue);
}

static blk_qc_t ramdisk_make_request(struct request_queue* q, struct bio* bio)
{
  struct bio_vec* bvec;
  int i;
  void* disk_mem;
  if ((bio->bi_sector * SECTOR_SIZE) + bio->bi_size > DISK_SIZE) {
    printk("ramdisk over flowed!\n");
    bio_endio(bio);
    return 0;
  }

  disk_mem = mem_start + bio->bi_sector * SECTOR_SIZE;
  /*bio_for_each_segment(bvec, bio, i) {
    void *iovec;
    iovec = kmap(bvec->bv_page) + bvec->bv_offset;
    switch (bio_data_dir(bio)) {
      // case READA:
      case READ:
        memcpy(iovec, disk_mem, bvec->bv_len);
        break;
      case WRITE:
        memcpy(disk_mem, iovec, bvec->bv_len);
        break;
      default:
        bio_endio(bio);
        kunmap(bvec->bv_page);
        return 0;
    }
    kunmap(bvec->bv_page);
    disk_mem += bvec->bv_len;
  }*/
  bio_endio(bio);
  return 0;
}

module_init(ramdisk_init);
module_exit(ramdisk_exit);
MODULE_LICENSE("GPL");
