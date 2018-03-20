#include "platform.h"
#include "textrange.h"
#include "string.h"

#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

void platform_make_path(String* path)
{
  mkdir(string_get(path), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void platform_make_full_path(String* path)
{
  Range *span_path;
  Range *span_subpath;

  range_create(&span_path);
  range_create(&span_subpath);

  span_path->begin = string_get(path);
  span_path->end = span_path->begin + path->length;

  span_subpath->begin = span_path->begin;
  span_subpath->end = span_path->begin; // Not a typo

  while (span_subpath->end < span_path->end) {
    if (*span_subpath->end == '\\' || *span_subpath->end == '/') {
      String *tmp;
      tmp = range_make_string(span_subpath);

      platform_make_path(tmp);

      string_destroy(&tmp);
    }

    ++span_subpath->end;
  }

  platform_make_path(path);

  range_destroy(&span_subpath);
  range_destroy(&span_path);
}

void platform_run_dirscan(String* path, PlatformFileCallback f_cb, PlatformDirCallback d_cb, void* userdata)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(string_get(path))))
        return;

    while ((entry = readdir(dir)) != 0) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if (d_cb)
                d_cb(entry->d_name, userdata);
        } else {
            f_cb(entry->d_name, entry->d_reclen, userdata);
        }
    }
    closedir(dir);
}
