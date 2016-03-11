#ifndef __DEV_FILE_H__
#define __DEV_FILE_H__



size_t read_char_dev_file(int dev_id, char *buf, int len);

size_t write_char_dev_file(int dev_id, char *buf, int len);

#endif /* __DEV_FILE_H__ */