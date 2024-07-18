#include "MIMETypes.hpp"
#include "Json.hpp"


namespace http
{
    namespace server
    {
        namespace mime_types
        {
            static const std::map<const std::string, std::string> mime = json::read_json_as_map("MIME.json");

            std::string extension_to_type(const std::string &extension)
            {
                std::map<const std::string, std::string>::const_iterator it = mime.find(extension);
                if (it != mime.cend())
                {
                    return it->second;
                }
                else
                {
                    return "text/plain";
                }
            }

        } // namespace mime_types
    }     // namespace server
} // namespace http