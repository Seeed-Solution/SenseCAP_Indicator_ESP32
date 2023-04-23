
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include "lv_port_fs.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Create a type to store the required data about your file.
 * If you are using a File System library
 * it already should have a File type.
 * For example FatFS has `FIL`. In this case use `typedef FIL file_t`*/
typedef  FILE * file_t;

/*Similarly to `file_t` create a type for directory reading too */
typedef  DIR * dir_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static const char *TAG = "lv_fs";

static bool fs_ready_cb(lv_fs_drv_t * drv);
static void* fs_open (lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek (lv_fs_drv_t * drv,  void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open (lv_fs_drv_t * drv, const char *path);
static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * rddir_p, char *fn);
static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static esp_err_t lv_fs_init_result = LV_FS_RES_HW_ERR;
static char f_path[256];
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/
#define LV_FS_PATH_PREFIX   "/spiffs"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
esp_err_t lv_port_fs_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS as demo assets storage.");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = LV_FS_PATH_PREFIX,
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ret;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    fs_init();

    return ESP_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your Storage device and File system. */
static void fs_init(void)
{
	/*---------------------------------------------------
	 * Register the file system interface  in LittlevGL
	 *--------------------------------------------------*/

	/* Add a simple drive to open images */
	static lv_fs_drv_t fs_drv;                         /*A driver descriptor*/
	lv_fs_drv_init(&fs_drv);

	fs_drv.letter = 'S';
    //fs_drv.cache_size = 1024*5;
    fs_drv.user_data = NULL;
    fs_drv.ready_cb = fs_ready_cb;
	fs_drv.open_cb = fs_open;
	fs_drv.close_cb = fs_close;
	fs_drv.read_cb = fs_read;
	fs_drv.write_cb = fs_write;
	fs_drv.seek_cb = fs_seek;
	fs_drv.tell_cb = fs_tell;

	fs_drv.dir_close_cb = fs_dir_close;
	fs_drv.dir_open_cb = fs_dir_open;
	fs_drv.dir_read_cb = fs_dir_read;
	lv_fs_drv_register(&fs_drv);
}

static bool fs_ready_cb(lv_fs_drv_t * drv)
{
    return true;
} 

static void * fs_open (lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "wb";
	else if(mode == LV_FS_MODE_RD) flags = "rb";
	else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "rb+";

    sprintf(f_path, LV_FS_PATH_PREFIX "/%s", path);
    
    FILE* f = fopen(f_path, flags);

    if (NULL == f) {
        ESP_LOGE(TAG, "fopen %s fail", f_path);
        return NULL;
    }

    fseek(f, 0, SEEK_SET);
    return f;
}

static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p)
{
    (void) drv;		/*Unused*/

	file_t * fp = file_p;        /*Just avoid the confusing casings*/

    if (NULL == fp) {
        ESP_LOGE(TAG, "NULL file pointer");
        return LV_FS_RES_NOT_EX;
    }

    if (EOF == fclose(fp)) {
        ESP_LOGE(TAG, "Failed close file pointer");
        return LV_FS_RES_NOT_EX;
    }

    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    (void) drv;		/*Unused*/
    file_t * fp = file_p;        /*Just avoid the confusing casings*/
	*br = fread(buf, 1, btr, fp);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    (void) drv;		/*Unused*/
    file_t * fp = file_p;        /*Just avoid the confusing casings*/
	*bw = fwrite(buf, 1, btw, fp);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek (lv_fs_drv_t * drv,  void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    (void) drv;		/*Unused*/
	file_t * fp = file_p;        /*Just avoid the confusing casings*/

    int  i = 0;
    if( whence == LV_FS_SEEK_END ){
        i = SEEK_END;
    } else if( whence == LV_FS_SEEK_CUR ){
        i = SEEK_CUR;
    } else {
        i = SEEK_SET;
    }
	fseek(fp, pos, i);
	return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    (void) drv;		/*Unused*/

    file_t * fp = file_p;        /*Just avoid the confusing casings*/
	*pos_p = ftell(fp);

    return LV_FS_RES_OK;
}



static void * fs_dir_open (lv_fs_drv_t * drv,const char *path)
{
    (void) drv;		/*Unused*/

    return  opendir(path);
}

static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * rddir_p, char *fn)
{
    struct dirent *entry;
	do {
		entry = readdir(rddir_p);

		if(entry) {
			if(entry->d_type == DT_DIR) sprintf(fn, "/%s", entry->d_name);
			else strcpy(fn, entry->d_name);
		} else {
			strcpy(fn, "");
		}
	} while(strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);

    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * rddir_p)
{
    (void) drv;		/*Unused*/
    closedir(rddir_p);

    return LV_FS_RES_OK;
}

/*end of file */