#include <MEL/Communications/Http.hpp>
#include <MEL/Logging/Log.hpp>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <limits>


namespace
{
    // Convert a string to lower case
    std::string to_lower(std::string str)
    {
        for (std::string::iterator i = str.begin(); i != str.end(); ++i)
            *i = static_cast<char>(std::tolower(*i));
        return str;
    }
}


namespace mel
{
////////////////////////////////////////////////////////////
Http::Request::Request(const std::string& uri, Method method, const std::string& body)
{
    set_method(method);
    set_uri(uri);
    set_http_version(1, 0);
    set_body(body);
}


////////////////////////////////////////////////////////////
void Http::Request::set_field(const std::string& field, const std::string& value)
{
    fields_[to_lower(field)] = value;
}


////////////////////////////////////////////////////////////
void Http::Request::set_method(Http::Request::Method method)
{
    method_ = method;
}


////////////////////////////////////////////////////////////
void Http::Request::set_uri(const std::string& uri)
{
    uri_ = uri;

    // Make sure it starts with a '/'
    if (uri_.empty() || (uri_[0] != '/'))
        uri_.insert(0, "/");
}


////////////////////////////////////////////////////////////
void Http::Request::set_http_version(unsigned int major, unsigned int minor)
{
    major_version_ = major;
    minor_version_ = minor;
}


////////////////////////////////////////////////////////////
void Http::Request::set_body(const std::string& body)
{
    body_ = body;
}


////////////////////////////////////////////////////////////
std::string Http::Request::prepare() const
{
    std::ostringstream out;

    // Convert the method to its string representation
    std::string method;
    switch (method_)
    {
        case Get:    method = "GET";    break;
        case Post:   method = "POST";   break;
        case Head:   method = "HEAD";   break;
        case Put:    method = "PUT";    break;
        case Delete: method = "DELETE"; break;
    }

    // Write the first line containing the request type
    out << method << " " << uri_ << " ";
    out << "HTTP/" << major_version_ << "." << minor_version_ << "\r\n";

    // Write fields
    for (FieldTable::const_iterator i = fields_.begin(); i != fields_.end(); ++i)
    {
        out << i->first << ": " << i->second << "\r\n";
    }

    // Use an extra \r\n to separate the header from the body
    out << "\r\n";

    // Add the body
    out << body_;

    return out.str();
}


////////////////////////////////////////////////////////////
bool Http::Request::has_field(const std::string& field) const
{
    return fields_.find(to_lower(field)) != fields_.end();
}


////////////////////////////////////////////////////////////
Http::Response::Response() :
status_      (ConnectionFailed),
major_version_(0),
minor_version_(0)
{

}


////////////////////////////////////////////////////////////
const std::string& Http::Response::get_field(const std::string& field) const
{
    FieldTable::const_iterator it = fields_.find(to_lower(field));
    if (it != fields_.end())
    {
        return it->second;
    }
    else
    {
        static const std::string empty = "";
        return empty;
    }
}


////////////////////////////////////////////////////////////
Http::Response::Status Http::Response::get_status() const
{
    return status_;
}


////////////////////////////////////////////////////////////
unsigned int Http::Response::get_major_http_version() const
{
    return major_version_;
}


////////////////////////////////////////////////////////////
unsigned int Http::Response::get_minor_http_version() const
{
    return minor_version_;
}


////////////////////////////////////////////////////////////
const std::string& Http::Response::get_body() const
{
    return body_;
}


////////////////////////////////////////////////////////////
void Http::Response::parse(const std::string& data)
{
    std::istringstream in(data);

    // Extract the HTTP version from the first line
    std::string version;
    if (in >> version)
    {
        if ((version.size() >= 8) && (version[6] == '.') &&
            (to_lower(version.substr(0, 5)) == "http/")   &&
             isdigit(version[5]) && isdigit(version[7]))
        {
            major_version_ = version[5] - '0';
            minor_version_ = version[7] - '0';
        }
        else
        {
            // Invalid HTTP version
            status_ = InvalidResponse;
            return;
        }
    }

    // Extract the status code from the first line
    int status;
    if (in >> status)
    {
        status_ = static_cast<Status>(status);
    }
    else
    {
        // Invalid status code
        status_ = InvalidResponse;
        return;
    }

    // Ignore the end of the first line
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Parse the other lines, which contain fields, one by one
    parse_fields(in);

    body_.clear();

    // Determine whether the transfer is chunked
    if (to_lower(get_field("transfer-encoding")) != "chunked")
    {
        // Not chunked - just read everything at once
        std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), std::back_inserter(body_));
    }
    else
    {
        // Chunked - have to read chunk by chunk
        std::size_t length;

        // Read all chunks, identified by a chunk-size not being 0
        while (in >> std::hex >> length)
        {
            // Drop the rest of the line (chunk-extension)
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Copy the actual content data
            std::istreambuf_iterator<char> it(in);
            std::istreambuf_iterator<char> itEnd;
            for (std::size_t i = 0; ((i < length) && (it != itEnd)); i++)
                body_.push_back(*it++);
        }

        // Drop the rest of the line (chunk-extension)
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Read all trailers (if present)
        parse_fields(in);
    }
}


////////////////////////////////////////////////////////////
void Http::Response::parse_fields(std::istream &in)
{
    std::string line;
    while (std::getline(in, line) && (line.size() > 2))
    {
        std::string::size_type pos = line.find(": ");
        if (pos != std::string::npos)
        {
            // Extract the field name and its value
            std::string field = line.substr(0, pos);
            std::string value = line.substr(pos + 2);

            // Remove any trailing \r
            if (!value.empty() && (*value.rbegin() == '\r'))
                value.erase(value.size() - 1);

            // Add the field
            fields_[to_lower(field)] = value;
        }
    }
}


////////////////////////////////////////////////////////////
Http::Http() :
host_(),
port_(0)
{

}


////////////////////////////////////////////////////////////
Http::Http(const std::string& host, unsigned short port)
{
    set_host(host, port);
}


////////////////////////////////////////////////////////////
void Http::set_host(const std::string& host, unsigned short port)
{
    // Check the protocol
    if (to_lower(host.substr(0, 7)) == "http://")
    {
        // HTTP protocol
        host_name_ = host.substr(7);
        port_     = (port != 0 ? port : 80);
    }
    else if (to_lower(host.substr(0, 8)) == "https://")
    {
        // HTTPS protocol -- unsupported (requires encryption and certificates and stuff...)
        LOG(Error) << "HTTPS protocol is not supported by sf::Http" << std::endl;
        host_name_ = "";
        port_     = 0;
    }
    else
    {
        // Undefined protocol - use HTTP
        host_name_ = host;
        port_     = (port != 0 ? port : 80);
    }

    // Remove any trailing '/' from the host name
    if (!host_name_.empty() && (*host_name_.rbegin() == '/'))
        host_name_.erase(host_name_.size() - 1);

    host_ = IpAddress(host_name_);
}


////////////////////////////////////////////////////////////
Http::Response Http::send_request(const Http::Request& request, Time timeout)
{
    // First make sure that the request is valid -- add missing mandatory fields
    Request toSend(request);
    if (!toSend.has_field("From"))
    {
        toSend.set_field("From", "user@sfml-dev.org");
    }
    if (!toSend.has_field("User-Agent"))
    {
        toSend.set_field("User-Agent", "libsfml-network/2.x");
    }
    if (!toSend.has_field("Host"))
    {
        toSend.set_field("Host", host_name_);
    }
    if (!toSend.has_field("Content-Length"))
    {
        std::ostringstream out;
        out << toSend.body_.size();
        toSend.set_field("Content-Length", out.str());
    }
    if ((toSend.method_ == Request::Post) && !toSend.has_field("Content-Type"))
    {
        toSend.set_field("Content-Type", "application/x-www-form-urlencoded");
    }
    if ((toSend.major_version_ * 10 + toSend.minor_version_ >= 11) && !toSend.has_field("Connection"))
    {
        toSend.set_field("Connection", "close");
    }

    // Prepare the response
    Response received;

    // Connect the socket to the host
    if (connection_.connect(host_, port_, timeout) == Socket::Done)
    {
        // Convert the request to string and send it through the connected socket
        std::string requestStr = toSend.prepare();

        if (!requestStr.empty())
        {
            // Send it through the socket
            if (connection_.send(requestStr.c_str(), requestStr.size()) == Socket::Done)
            {
                // Wait for the server's response
                std::string receivedStr;
                std::size_t size = 0;
                char buffer[1024];
                while (connection_.receive(buffer, sizeof(buffer), size) == Socket::Done)
                {
                    receivedStr.append(buffer, buffer + size);
                }

                // Build the Response object from the received data
                received.parse(receivedStr);
            }
        }

        // Close the connection
        connection_.disconnect();
    }

    return received;
}

} // namespace mel
