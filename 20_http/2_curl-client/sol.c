#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char* data;
  size_t length;
} buffer_t;

static size_t
callback_function(
    char* ptr,
    size_t chunk_size,
    size_t nmemb,
    void* user_data
) {
  buffer_t* buffer = user_data;
  size_t total_size = chunk_size * nmemb;
  memcpy(buffer->data, ptr, total_size);
  buffer->length += total_size;
  return total_size;
}

void print_title(buffer_t* source) {
  char start_tag[] = "<title>";
  char end_tag[] = "</title>";
  char* title_start = strstr(source->data, start_tag) + strlen(start_tag);
  char* title_end = strstr(source->data, end_tag);
  if (title_start != title_end) {
    write(STDOUT_FILENO, title_start, title_end - title_start);
    return;
  }
  char start_tag_t[] = "<TITLE>";
  char end_tag_t[] = "</TITLE>";
  title_start = strstr(source->data, start_tag_t) + strlen(start_tag_t);
  title_end = strstr(source->data, end_tag_t);
  if (title_start != title_end) {
    write(STDOUT_FILENO, title_start, title_end - title_start);
    return;
  }
}

int main(int argc, char* argv[]) {
  CURL* curl = curl_easy_init();
  if (curl) {
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_function);
    buffer_t buffer;
    buffer.data = calloc(100 * 1024 * 1024, 1);
    buffer.length = 0;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
    res = curl_easy_perform(curl);
    print_title(&buffer);
    free(buffer.data);
    curl_easy_cleanup(curl);
  }
}

