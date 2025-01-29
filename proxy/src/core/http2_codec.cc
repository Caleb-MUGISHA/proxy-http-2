#include <nghttp2/nghttp2.h>
#include <functional>
#include <string>
#include <vector>

class Http2Codec {
public:
  // Callback types
  using HeaderCallback = std::function<void(const std::string&, const std::string&)>;
  using DataCallback = std::function<void(int32_t stream_id, const std::vector<uint8_t>&)>;
  using FrameCallback = std::function<void(const nghttp2_frame&)>;

  Http2Codec() {
    nghttp2_session_callbacks_new(&callbacks_);
    
    // Setup core callbacks
    nghttp2_session_callbacks_set_on_begin_headers_callback(callbacks_, on_begin_headers);
    nghttp2_session_callbacks_set_on_header_callback(callbacks_, on_header);
    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks_, on_frame_received);
    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks_, on_data_chunk_received);
    nghttp2_session_callbacks_set_on_stream_close_callback(callbacks_, on_stream_closed);

    nghttp2_session_server_new(&session_, callbacks_, this);
    nghttp2_session_callbacks_del(callbacks_);
    callbacks_ = nullptr;
  }

  ~Http2Codec() {
    if (session_) {
      nghttp2_session_del(session_);
    }
  }

  int process_data(const uint8_t* data, size_t len) {
    int rv = nghttp2_session_mem_recv(session_, data, len);
    if (rv < 0) {
      handle_error(rv);
    }
    return rv;
  }

  // Callback setters
  void set_header_callback(HeaderCallback cb) { header_cb_ = cb; }
  void set_data_callback(DataCallback cb) { data_cb_ = cb; }
  void set_frame_callback(FrameCallback cb) { frame_cb_ = cb; }

private:
  // Static callback wrappers
  static int on_begin_headers(nghttp2_session* session,
                              const nghttp2_frame* frame,
                              void* user_data) {
    auto* self = static_cast<Http2Codec*>(user_data);
    if (self->frame_cb_) {
      self->frame_cb_(*frame);
    }
    return 0;
  }

  static int on_header(nghttp2_session* session,
                       const nghttp2_frame* frame,
                       const uint8_t* name, size_t namelen,
                       const uint8_t* value, size_t valuelen,
                       uint8_t flags, void* user_data) {
    auto* self = static_cast<Http2Codec*>(user_data);
    if (self->header_cb_ && frame->hd.type == NGHTTP2_HEADERS) {
      self->header_cb_(
          std::string(reinterpret_cast<const char*>(name), namelen),
          std::string(reinterpret_cast<const char*>(value), valuelen));
    }
    return 0;
  }

  static int on_frame_received(nghttp2_session* session,
                               const nghttp2_frame* frame,
                               void* user_data) {
    auto* self = static_cast<Http2Codec*>(user_data);
    if (self->frame_cb_) {
      self->frame_cb_(*frame);
    }
    return 0;
  }

  static int on_data_chunk_received(nghttp2_session* session,
                                    uint8_t flags,
                                    int32_t stream_id,
                                    const uint8_t* data,
                                    size_t len,
                                    void* user_data) {
    auto* self = static_cast<Http2Codec*>(user_data);
    if (self->data_cb_) {
      self->data_cb_(stream_id, {data, data + len});
    }
    return 0;
  }

  static int on_stream_closed(nghttp2_session* session,
                              int32_t stream_id,
                              uint32_t error_code,
                              void* user_data) {
    auto* self = static_cast<Http2Codec*>(user_data);
    // Handle stream closure if needed
    return 0;
  }

  void handle_error(int error_code) {
    // Implement error handling strategy
    const char* error_str = nghttp2_strerror(error_code);
    fprintf(stderr, "HTTP/2 error: %s\n", error_str);
  }

  nghttp2_session* session_ = nullptr;
  nghttp2_session_callbacks* callbacks_ = nullptr;
  
  // Callback storage
  HeaderCallback header_cb_;
  DataCallback data_cb_;
  FrameCallback frame_cb_;
};
