class LeastConnLoadBalancer : public LoadBalancer {
public:
  UpstreamHostPtr select_host(const Request& request) override {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::min_element(hosts_.begin(), hosts_.end(),
      [](const auto& a, const auto& b) {
        return a->active_requests < b->active_requests;
      });
    
    if (it != hosts_.end()) {
      (*it)->active_requests++;
      return *it;
    }
    
    throw NoHostAvailableException();
  }

  void release_host(UpstreamHost& host) override {
    std::lock_guard<std::mutex> lock(mutex_);
    host.active_requests--;
  }
};
