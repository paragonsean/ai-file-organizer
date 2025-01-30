#ifndef LLMCLIENT_HPP
#define LLMCLIENT_HPP

#include <string>

class LLMClient {
public:
    LLMClient(const std::string &api_key);
    std::string categorize_file(const std::string& file_name, const std::string& file_type);

private:
    std::string api_key;
    std::string send_api_request(std::string json_payload);
    std::string make_payload(const std::string &file_name, const std::string &file_type);
};

#endif
