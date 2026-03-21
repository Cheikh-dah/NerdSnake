#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <curl/curl.h>
#include "json.hpp"
#include "dotenv.h"

class GeminiClient {
private:
  std::string apiKey;
  std::string model;
    
  static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    size_t totalSize = size * nmemb;
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
  }
  
  std::string cleanJsonResponse(const std::string& raw) {
    // Remove markdown code blocks
    std::string cleaned = std::regex_replace(raw, std::regex("```json\\s*"), "");
    cleaned = std::regex_replace(cleaned, std::regex("```"), "");
    
    // Trim whitespace
    cleaned = std::regex_replace(cleaned, std::regex("^\\s+|\\s+$"), "");
    
    // Find JSON array/object (using regex with multiline support)
    std::smatch match;
    // Use regex::extended with multiline flag
    std::regex jsonPattern(R"((\{[\s\S]*\}|\[[\s\S]*\]))");
    if (std::regex_search(cleaned, match, jsonPattern)) {
      cleaned = match.str(0);
    }
    
    // Fix trailing commas
    cleaned = std::regex_replace(cleaned, std::regex(",\\s*([\\]\\}])"), "$1");
    
    return cleaned;
  }
  
public:
  GeminiClient(const std::string& key, const std::string& mdl = "gemini-2.5-flash")
    : apiKey(key), model(mdl) {}
  
  nlohmann::json generateContent(const std::string& prompt) {
    std::string response = makeApiRequest(prompt);
    std::string cleaned = cleanJsonResponse(response);
    
    try {
      return nlohmann::json::parse(cleaned);
    } catch (const nlohmann::json::parse_error& e) {
      std::cerr << "Failed to parse JSON!" << std::endl;
      std::cerr << "Error: " << e.what() << std::endl;
      std::cerr << "Cleaned JSON text: " << cleaned << std::endl;
      throw;
    }
  }
  
  std::string makeApiRequest(const std::string& prompt) {
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + 
      model + ":generateContent?key=" + apiKey;
    
    nlohmann::json requestBody = {
      {"contents", {{
	    {"parts", {{{"text", prompt}}}}
	  }}}
    };
    
    std::string requestStr = requestBody.dump();
    
    CURL* curl = curl_easy_init();
    std::string response;
    
    if (curl) {
      struct curl_slist* headers = nullptr;
      headers = curl_slist_append(headers, "Content-Type: application/json");
      
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestStr.c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
      
      CURLcode res = curl_easy_perform(curl);
      if (res != CURLE_OK) {
	std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	return "";
      }
      
      curl_easy_cleanup(curl);
      curl_slist_free_all(headers);
    }
    
    try {
      auto jsonResponse = nlohmann::json::parse(response);
      return jsonResponse["candidates"][0]["content"]["parts"][0]["text"];
    } catch (...) {
      return response;
    }
  }
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <topic>" << std::endl;
    return 1;
  }
  
  std::string topic = argv[1];
  std::string prompt = "Generate 20 science terms about " + topic + 
    " with definitions in JSON. Return ONLY a JSON array of objects with 'term' and 'definition' fields.";
    
  const char* apiKeyEnv = std::getenv("GEMINI_API_KEY");
  std::string apiKeyStr;
  
  if (apiKeyEnv) {
    apiKeyStr = apiKeyEnv;
  } else {
    auto env = LoadEnv(".env");
    auto it = env.find("GEMINI_API_KEY");
    if (it != env.end()) {
      apiKeyStr = it->second;
    }
  }
    
  if (apiKeyStr.empty()) {
    std::cerr << "Please set GEMINI_API_KEY environment variable or create a .env file" << std::endl;
    return 1;
  }
  
  try {
    GeminiClient client(apiKeyStr);
    nlohmann::json terms = client.generateContent(prompt);

    if (terms.contains("error")) {
      std::cerr << "API Error: " << terms["error"]["message"] << std::endl;
      return 1;
    }
      
    std::cout << "=== RAW RESPONSE ===" << std::endl;
    std::cout << terms.dump(2) << std::endl;
      
    std::ofstream file("terms.json");
    if (file.is_open()) {
      file << terms.dump(4) << std::endl;
      file.close();
      std::cout << "terms.json generated successfully!" << std::endl;
    } else {
      std::cerr << "Failed to open terms.json for writing" << std::endl;
      return 1;
    }
      
    if (terms.is_array()) {
      std::cout << "\nFirst 3 terms:" << std::endl;
      for (size_t i = 0; i < std::min(terms.size(), size_t(3)); ++i) {
        std::cout << "- " << terms[i]["term"] << ": " 
                  << terms[i]["definition"] << std::endl;
      }
    }
      
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
    
  return 0;
}
