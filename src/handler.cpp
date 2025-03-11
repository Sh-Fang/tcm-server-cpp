#include "handler.h"
#include "json.hpp"

using json = nlohmann::json;

// 解析 JSON 输入
Input parseInput(const json& j) {
    return { j["stream_path"], j["query_path"] };
}

// 处理 HTTP 请求
void handleRequest(const httplib::Request& req, httplib::Response& res) {
    try {
        json request_json = json::parse(req.body);
        Input input = parseInput(request_json);

        Output output = Matching(input.stream_path,input.query_path);

        json response_json;
        response_json["result"] = output.result;

        res.set_content(response_json.dump(), "application/json");
    } catch (const std::exception&) {
        res.status = 400;
        res.set_content("{\"error\": \"Invalid request\"}", "application/json");
    }
}

Output Matching(std::string& stream_path, std::string& query_path) {
    Output output;
    output.result = "Matching: " + stream_path + " - " + query_path;
    return output;
}

void logRequest(const httplib::Request& req, const httplib::Response& res) {
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    // 使用 std::localtime (非线程安全)
    std::tm* tm_info = std::localtime(&in_time_t);  

    // 打印日志
    std::cout << "[" << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S") << "] "
              << "Client: " << req.remote_addr << " | "
              << "Method: " << req.method << " | "
              << "Path: " << req.path << " | "
              << "Status: " << res.status << " | "
              << "Body: " << res.body << std::endl;
}